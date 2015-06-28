#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>    // requese_irq
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/mm.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <asm/io.h>      //ioreamap()
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/signal.h>
#include <linux/proc_fs.h> 
#include <linux/fs.h>   //struct file

#define IORESOURCE_MEM		0x00000200
#define DMA_INT_MASK        (*(unsigned long *)(Vbase+0xc20))
#define DMA_INT_STAT		(*(unsigned long *)(Vbase+0xc28))
#define DMA_CMD_FIFO        (*(unsigned long *)(Vbase+0xc30))
#define DMA_BUFF_SIZE       4096

#define DRV_MODULE_NAME     "test_pci"

unsigned long Pbase = 0;
unsigned long Vbase = 0;
unsigned long len   = 0;

unsigned long dmaVaddr   = 0;
dma_addr_t    dmaBusAddr = 0;



static irqreturn_t test_irq(int irq, void *dev_id)
{
	
	return IRQ_HANDLED;
}

static int test_net_open(struct net_device *dev)
{
	int ret = 0;
	ret = request_irq(dev->irq, test_irq, 0, "test_pci", dev);
	if (ret < 0){
		printk("request irq failed\n");
		return -1;
	}
	
	return 0;
}

static int test_net_close(struct net_device *dev)
{

	free_irq(dev->irq, dev);
	return 0;
}

static int test_start_xmit(struct net_device *dev)
{
	return 0;
}

static int test_change_mtu(struct net_device *netdev, int new_mtu)
{
	return 0;
}					 

static int test_set_mac_addr(struct net_device *netdev, void *p)
{
	return 0;
}

static void test_net_timeout(struct net_device *dev)
{
	;
}

static const struct net_device_ops test_netdev_ops = {
	.ndo_open		= test_net_open,
	.ndo_stop		= test_net_close,
	.ndo_start_xmit		= test_start_xmit,
	.ndo_tx_timeout		= test_net_timeout,
	.ndo_set_mac_address	= test_set_mac_addr,
	.ndo_change_mtu		= test_change_mtu,	
};

static int __devinit 
test_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct net_device *netdev = NULL;
	int err;
	unsigned short val = 0;
	
	netdev = alloc_etherdev(4);
	if (netdev == NULL)
	{
		printk("alloc_etherdev failed\n");
		return -1;
	}
	
	SET_NETDEV_DEV(netdev, &pdev->dev);
//	ether_setup(netdev);
	netdev->netdev_ops = &test_netdev_ops;
	
	netdev->irq               = pdev->irq;   //irq 

	pci_set_drvdata(pdev, netdev);
	
	
	if ((err = register_netdev(netdev))){
		printk("register_netdev failed\n");
		pci_set_drvdata(pdev, NULL);
		free_netdev(netdev);
		return err;
	}
	
	if ((err = pci_enable_device(pdev))){
		printk("pci_enable_device failed");
		pci_set_drvdata(pdev, NULL);
		free_netdev(netdev);
		return err;
	}
	
	if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
		printk("pci_resource_flags failed\n");
		pci_disable_device(pdev);
		pci_set_drvdata(pdev, NULL);
		free_netdev(netdev);
		return -2;		
	}
	
	err = pci_request_regions(pdev, DRV_MODULE_NAME);
	if (err) {
		printk("pci_request_regions failed\n");
		pci_disable_device(pdev);
		pci_set_drvdata(pdev, NULL);
		free_netdev(netdev);
		return -2;
	}

	Pbase = pci_resource_start(pdev, 0);
	len   = pci_resource_len(pdev, 0);
	Vbase = (unsigned long)ioremap(Pbase, len);
	
	/* print the board memory */
	printk("Pbase addr: 0x%lx\n", Pbase);
	printk("memory size: %ld\n", len);
	printk("Vbase addr: 0x%lx\n", Vbase);
	/* end */
	
	pci_set_master(pdev);
	
	/* read pcie config */
	printk("read the VID and DID:\n");
	pci_read_config_word(pdev, 0x0, &val);
	printk("VID: %d\n", val);
	pci_read_config_word(pdev, 0x2, &val);
	printk("DID: %d\n", val);
	/* end */
	
	/* alloc  dma buffer */
	dmaVaddr = (unsigned long)pci_alloc_consistent(pdev, DMA_BUFF_SIZE, &dmaBusAddr);
	
	
	if ((err = register_netdev(netdev))){
		printk("register_netdev failed\n");
		pci_free_consistent(pdev, DMA_BUFF_SIZE, (void*)dmaVaddr, dmaBusAddr);
		if (Vbase){
			iounmap((void*)Vbase);
			Vbase = 0;
		}
		pci_release_regions(pdev);		
		pci_disable_device(pdev);
		pci_set_drvdata(pdev, NULL);
		free_netdev(netdev);
		return err;
	}

	
	
	return 0;
}

static void __devexit
test_remove(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	
	unregister_netdev(dev);
	
	pci_free_consistent(pdev, DMA_BUFF_SIZE, (void*)dmaVaddr, dmaBusAddr);
	
	if (Vbase){
		iounmap((void*)Vbase);
		Vbase = 0;
	}
	
	pci_release_regions(pdev);		
	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);
	free_netdev(dev);
}

/* dma function */
void test_dma_write(void)
{
	unsigned long DES0 = 0;
	unsigned long DES1 = 1;
	unsigned int  i    = 0;
//	unsigned int  j    = 0;
	char tmpBuf[512] = {0};
	
	for (i = 0; i < 512; i++){
		tmpBuf[i] = i % 16;
	}
	for (i = 0; i < 512; i++) {
		printk("%2d ", tmpBuf[i]);
		if ((i+1) % 16 == 0)
			printk("\n");
	}
	
	DMA_CMD_FIFO = DES0;
	DMA_CMD_FIFO = DES1;
	
	
}

void test_dma_read(void)
{

}

/* proc sys */
ssize_t test_read(char *page, char **start, off_t offset, int count, 
                  int *eof, void *data)
{
	return 0;
}

ssize_t test_write(struct file *filp, const char __user *buff, unsigned long len, void *data)
{
	char kBuf[10];
	int val;
	int cnt = len > 10 ? len : 10;
	
	if (copy_from_user(kBuf, buff, cnt)){
		return -1;
	}
	val = kBuf[0] - '0';
	
	switch (val) {
		case 0:
			break;
		case 1:
			test_dma_write();
			break;
		case 2:
			test_dma_read();
			break;
		default:
			break;
	}
	
	return cnt;
}

/* end */

static struct pci_device_id test_pci_tbl[] = {
	{ PCI_DEVICE(0x1, 0x2) },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, test_pci_tbl);

static struct pci_driver test_pci_driver = {
	.name		= "tset_pci",
	.id_table	= test_pci_tbl,
	.probe		= test_probe,
	.remove		= test_remove,
};

static int __init pci_init(void)
{
	create_proc_entry("test_pci", 0666, NULL);
	return pci_register_driver(&test_pci_driver);
}

static void __exit pci_exit(void)
{
	pci_unregister_driver(&test_pci_driver);
	remove_proc_entry("test_pci", NULL);
}

module_init(pci_init);
module_exit(pci_exit);

MODULE_AUTHOR("root");
MODULE_LICENSE("GPL");



