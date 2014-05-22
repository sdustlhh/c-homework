#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#define GLOBALMEM_SIZE    0x1000
#define MEM_CLEAR         0x1
#define GLOBALMEM_MAJOR   250 

static int globalmem_major = GLOBALMEM_MAJOR;

/* device struct globalmem */
struct globalmem_dev{
    struct cdev cdev;
    unsigned char mem[GLOBALMEM_SIZE];
};

struct globalmem_dev *globalmem_devp;

/**************************************************************
 * Function name: globalmem_open
 * argv:          inode,
                  filp
 * return:        0 if sucess
 * Description:   file open function
**************************************************************/
int globalmem_open(struct inode *inode, struct file *filp)
{
    filp->private_data = globalmem_devp;
    return 0;
}

/**************************************************************
 * Function name: globalmem_release
 * argv:          inode
                  filp
 * return:        0 if sucess
 * Description:   file release function (do nothing)
**************************************************************/
int globalmem_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/**************************************************************
 * Function name: globalmem_ioctl
 * argv:          inodep,
                  filp,
                  cmd,
                  arg,
 * return:        0 if sucess
                  errno if not sucess
 * Description:   ioctl function the device driver
**************************************************************/
static int globalmem_ioctl(struct inode *inodep, struct file *filp,
                          unsigned int cmd, unsigned long arg)
{
    struct globalmem_dev *dev = filp->private_data;
    
    switch (cmd)
    {
        case MEM_CLEAR:
            memset(dev->mem, 0, GLOBALMEM_SIZE);
            printk(KERN_INFO "globalmem is set to zero\n");
            break;
        default:
            return - EINVAL;
    }
    
    return 0;
    
}

/**************************************************************
 * Function name: globalmem_read
 * argv:          filp,
                  buf,
                  count,
                  ppos,
 * return:        number of bytes already read
 * Description:   read function for the device driver 
**************************************************************/
static ssize_t globalmem_read(struct file *filp, char __user *buf,
                             size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct globalmem_dev *dev = filp->private_data;
    
    if (p >= GLOBALMEM_SIZE)
    {
        return 0;
    }
    if (count > GLOBALMEM_SIZE - p)
    {
        count = GLOBALMEM_SIZE - p;
    }
    
    if (copy_to_user(buf, (void *)(dev->mem + p), count))
    {
        ret = - EFAULT;
    }
    else 
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "read %d bytes(s) from 0x%lx\n", count, p);
    }
    
    return ret;
}


/**************************************************************
 * Function name: globalmem_write
 * argv:          filp,
                  buf,
                  count,
                  ppos,
 * return:        numbers of bytes already write
 * Description:   the device driver module initial function
**************************************************************/
static ssize_t globalmem_write(struct file *filp, const char __user *buf,
                               size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct globalmem_dev *dev = filp->private_data;
    
    if (p >= GLOBALMEM_SIZE)
    {
        return 0;
    }
    
    if (count >= GLOBALMEM_SIZE - p)
    {
        count = GLOBALMEM_SIZE - p;
    }
    
    if (copy_from_user(dev->mem + p, buf, count))
    {
        ret = - EFAULT;
    }
    else 
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "written %d byte(s) from 0x%lx\n", count, p);
    }
    
    return ret;
}

/**************************************************************
 * Function name: globalmem_llseek
 * argv:          filp,
                  offset,
                  orig
 * return:        new offset  if sucess
                  errno if not sucess
 * Description:   the device driver module initial function
**************************************************************/
static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig)
{
    loff_t ret;
    
    switch (orig)
    {
        /* from begin */
        case 0:
            if (offset < 0)
            {
                ret = - EINVAL;
                break;
            }
            if ((unsigned int)offset > GLOBALMEM_SIZE)
            {
                ret = - EINVAL;
                break;
            }
            filp->f_pos = (unsigned int)offset;
            ret = filp->f_pos;
            break;
        /* from current pos */
        case 1:
            if ((filp->f_pos + offset) > GLOBALMEM_SIZE)
            {
                ret = - EINVAL;
                break;
            }
            if ((filp->f_pos + offset) < 0)
            {
                ret = - EINVAL;
                break;
            }
            filp->f_pos += offset;
            ret = filp->f_pos;
            break;
        default:
            ret = - EINVAL;
    }
    
    return ret;
}

/* ops struct */
static const struct file_operations globalmem_fops = {
    .owner = THIS_MODULE,
    .llseek = globalmem_llseek,
    .read = globalmem_read,
    .write = globalmem_write,
    .unlocked_ioctl = globalmem_ioctl,
    .open = globalmem_open,
    .release = globalmem_release,
};


/**************************************************************
 * Function name: globalmem_setup_cdev
 * argv:          none
 * return:        none
 * Description:   initial and register the cdev
**************************************************************/
static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)
{
    int err, devno = MKDEV(globalmem_major, index);
    
    cdev_init(&dev->cdev, &globalmem_fops);
    dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&dev->cdev, devno, 1);
    if (err)
    {
        printk(KERN_NOTICE "Error %d adding globalmem %d", err, index);
    }
}

/**************************************************************
 * Function name: globalmem_init
 * argv:          none
 * return:        0 if sucess
                  errno if not sucess
 * Description:   the device driver module initial function
**************************************************************/
static int __init globalmem_init(void)
{
    int result;
    dev_t devno = MKDEV(globalmem_major, 0);
    
    if (globalmem_major)
    {
        result = register_chrdev_region(devno, 1, "globalmem");
    }
    else 
    {
        result = alloc_chrdev_region(&devno, 0, 1, "globalmem");
        globalmem_major = MAJOR(devno);
    }
    if (result < 0)
    {
        return result;
    }
    
    globalmem_devp = kmalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
    if (!globalmem_devp)
    {
        result = - ENOMEM;
        goto fail_malloc;
    }
    memset(globalmem_devp, 0, sizeof(struct globalmem_dev));
    
    globalmem_setup_cdev(globalmem_devp, 0);
    return 0;
    
fail_malloc:
    unregister_chrdev_region(devno, 1);
    return result;           
}

/**************************************************************
 * Function name: globalmem_exit
 * argv:          none
 * return:        none
 * Description:   the device driver module exit function
**************************************************************/
static void __exit globalmem_exit(void)
{
    cdev_del(&globalmem_devp->cdev);
    kfree(globalmem_devp);
    unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);
}

module_param(globalmem_major, int, S_IRUGO);

module_init(globalmem_init);
module_exit(globalmem_exit);

MODULE_AUTHOR("wangjie");
MODULE_LICENSE("GPL");









