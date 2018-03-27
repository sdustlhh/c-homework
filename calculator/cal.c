/**************************************************************
 * Copyright:   WHICC
 * File name:   base.c
 * Description: this file consit of definitions of function which
                do the calculate jops.
 * Version:     1.0
 * Date:        2013-11-25
 * History:     none
***************************************************************/



#include "cal.h"
#include "base.h"

/***************************************************************
 *  Function name: mid_to_last
 *  argv:          buf,
 				   length
 *  return:        1 if successed,
                   0 if failed
 *  Description:   switch the mid expression to last expression
***************************************************************/
int mid_to_last(char *buf, int length)
{
	char tmp[BUFSIZE];
	char *ptr = buf;
	char *cursor = tmp;
	int index = 0;
    int is_num_flag = FALSE;
    int pare_count = 0;

    /* work as a stack */
	char symbol[STACKSIZE];
	int sym_top = 0;

	memset(tmp, 0, BUFSIZE);
	memset(symbol, 0, STACKSIZE);
	strcpy(tmp, ptr);

	/* start to switch */
	while ('\0' != *cursor)
	{
		/* handle the space char */
		if (' ' == *cursor)
		{
			++cursor;
		    continue;
		}

		/* handle  number situation */
	    if (isdigit(*cursor) != 0)
		{
		    if ('0' == *cursor && FALSE == is_num_flag)
		    {
	            printf("illegal expression\n");
		        return FALSE;		        
		    }
			ptr[index] = *cursor;
			++index;
			++cursor;
			is_num_flag = TRUE;
			continue;
		}
        
		/* marked when a number end */
		if (0 != index && TRUE == is_num_flag)
		{
			ptr[index] = '?';
			++index;
			is_num_flag = FALSE;
		}

        /* handle '+' and '-' situation */
		if ('+' == *cursor || '-' == *cursor)
		{
		    while (0 != sym_top)
			{
				--sym_top;
			    if ('(' != symbol[sym_top])
				{
				    ptr[index] = symbol[sym_top];
					++index;
					continue;
				}
				++sym_top;
				break;
			}
			
			symbol[sym_top] = *cursor;
			++sym_top;
			++cursor;
			continue;
		}

        /* handle '*', '%' and '(' situation */
		if ('*' == *cursor || '/' == *cursor || '(' == *cursor)
		{
		    symbol[sym_top] = *cursor;
			++sym_top;
			
			if ('(' == *cursor)
			{
			    ++pare_count;
			}
			++cursor;
			continue;
		}

		/* handle ')' situation i */
		if (')' == *cursor)
		{
		    --pare_count;
		    while (0 != sym_top && '(' != symbol[--sym_top])
			{
			    ptr[index] = symbol[sym_top];
                ++index;
				++cursor;
				continue;
			}
			continue;
		}

		/* unexpected input */
		printf("illegal expression\n");
		return FALSE;
	} 

    /* the parenttheses should be even */
    if (0 != pare_count)
    {
	    printf("illegal expression\n");
		return FALSE;
	}

	/* every number end with a '?' */
	ptr[index++] = '?';
	
	/* complete the expression */
	while (0 != sym_top)
	{
	    ptr[index] = symbol[sym_top-1];
		++index;
		--sym_top;
	}
	memset(ptr+index, 0, BUFSIZE-index);
	
	return TRUE;
}

/***************************************************************
 *  Function name: do_expre
 *  argv:          buf,
 				   length
 *  return:        none
 *  Description:   calculate the expression
***************************************************************/
void do_expre(char *buf, int length)
{
	if (FALSE == mid_to_last(buf, length))
	{
	    return ;
	}
//	printf("%s\n", buf);

    /* work as a stack */
	int number[STACKSIZE];
	int num_top = 0;
	memset(number, 0, STACKSIZE);

	int tmp = 0;
	char *ptr = buf;

	int success_flag = TRUE;
	/* traversel the expression to calculate */
	int is_num_flag = FALSE;
	while ('\0' != *ptr)
	{
		if (isdigit(*ptr) != 0)
		{
		    tmp = tmp * 10 + (*ptr - '0');
			++ptr;
			is_num_flag = TRUE;
			continue;
		}
		
		if (TRUE == is_num_flag)
		{
			number[num_top] = tmp;
			++num_top;
			tmp = 0;
			is_num_flag = FALSE;
		}
		/* ingored the '?' */
		if ('?' == *ptr)
		{
			++ptr;
			continue;
		}
		
		/* get number from stack and calculate when *ptr is a symbol */
		int num_one = number[--num_top];
		int num_two = number[--num_top];
		tmp = do_cal(num_two, num_one, *ptr, &success_flag);
        if (FALSE == success_flag)
		{
		    break;
		}
		number[num_top] = tmp;
		tmp = 0;
		++num_top;
		++ptr;
	}

    if (TRUE == success_flag)
	{
	    printf("%d\n", number[--num_top]);
	}
	else 
	{
	    printf("can't calcultion\n");
	}
}

/***************************************************************
 * Funciton name: add
 * Argv:          char num_a[],
                  char num_b[],
                  char sum[],
 * Return:        none
 * Descirption:   do addition, return the result in argv sum[] 
***************************************************************/
void add(char num_a[], char num_b[], char sum[])
{
    int index = 0;
    int tmp_sum = 0;
    int carry = 0;
    
    /* handle the special case */
    int flag_a = num_check(num_a);
    int flag_b = num_check(num_b);
    if (-1 == flag_a || -1 == flag_b)
    {
        printf("illegal number\n");
        return ;
    }
    if (0 == flag_a)
    {
        strcpy(sum, num_b);
        return ;
    }	
    if (0 == flag_b)
    {
        strcpy(sum, num_a);
        return ;
    }
	    
    /* pre-treat the numbers */
    reverse(num_a);
    reverse(num_b);
    align(num_a, num_b);
    
    /* begin addition */
    for (index = 0; '\0' != num_a[index]; ++index)
    {
        tmp_sum = (num_a[index] - '0') + (num_b[index] - '0') + carry;
        carry = tmp_sum / 10;
        tmp_sum = tmp_sum % 10;
        sum[index] = tmp_sum + '0';
    }
    if (1 == carry)
    {
        sum[index] = '1';
        sum[++index] = '\0';
    }
    else 
    {
        sum[index] = '\0';
    }
   
    reverse(sum); 
}


/***************************************************************
 * Funciton name: num_inc
 * Argv:          char a[],
 * Return:        none
 * Descirption:   increase one to a number 
***************************************************************/
void num_inc(char a[])
{
    char num_tmp[NUMSIZE] = {0};
    char one[NUMSIZE] = {0};
    
    one[0] = '1';
    add(a, one, num_tmp);
    
    strcpy(a, num_tmp);
}


/***************************************************************
 * Funciton name: sub
 * Argv:          char num_a[],
                  char num_b[],
                  char remain[],
 * Return:        none
 * Descirption:   do subtraction, return the result in argv remain[] 
****************************************************************/
void sub(char num_a[], char num_b[], char remain[])
{
	char big[NUMSIZE] = {0};
	char small[NUMSIZE] = {0};
	
	/* handle the special case */
    int flag_a = num_check(num_a);
    int flag_b = num_check(num_b);
    if (-1 == flag_a || -1 == flag_b)
    {
        printf("illegal number\n");
        return ;
    }
    if (0 == flag_a)
    {
        remain[0] = '-';
        strcpy(++remain, num_b);
        return ;
    }
    if (0 == flag_b)
    {
        strcpy(remain, num_a);
        return ;
    }
	
	/* find the bigger number, put it in big[],and small one in small[] */
    int flag = num_cmp(num_a, num_b);
    switch (flag)
    {
        case 1:
            strcpy(big, num_a);
            strcpy(small, num_b);
            break;
        case -1:
            strcpy(big, num_b);
            strcpy(small, num_a);
            break;
        case 0:
            remain[0] = '0';
            return ;
        default:
            break;
    }
    
	/* pre-treat numbers */
	reverse(big);
	reverse(small);
	align(big, small);
	
	/* begin subtraction */
	int borrow = 0;
	int index = 0;
	int bit_a = 0;
	int bit_b = 0;
	int bit_r = 0;
	for (index = 0; '\0' != big[index]; ++index)
	{
	    bit_a = big[index] - '0';
	    bit_b = small[index] - '0';
	    if (0 != bit_a && (bit_a - borrow) >= bit_b)
	    {
	        bit_r = bit_a - borrow - bit_b;
	        borrow = 0;
	    }
	    else if (0 != bit_a && (bit_a - borrow) < bit_b)
	    {
	        bit_r = bit_a - borrow + 10 - bit_b;
	        borrow = 1; 
	    }
	    else if (0 == bit_a && 0 == bit_b && 0 == borrow)
	    {
	        bit_r = bit_a - bit_b;
	        borrow = 0;
	    }
	    else 
	    {
	        bit_r = bit_a + 10 - bit_b - borrow;
	        borrow = 1;
	    }
	    remain[index] = bit_r + '0';
	}
	
	/* regulate the result */
	while ('0' == remain[--index])
	{
	       ;
	}
	if (-1 == flag)
	{
	    remain[++index] = '-';
	    remain[++index] = '\0';
    }
    remain[++index] = '\0';
    reverse(remain);
}


/***************************************************************
 * Funciton name: mul
 * Argv:          char num_a[],
                  char num_b[],
                  char product[],
 * Return:        none
 * Descirption:   do multiply, return the result in argv product[] 
***************************************************************/
void mul(char num_a[], char num_b[], char product[])
{
    int index_a = 0;
    int index_b = 0;
    int carry = 0;
    int bit_p = 0;
    int count = 0;
    char bit_pro[RESSIZE] = {0};
    char tmp_pro[RESSIZE] = {0};
    
    /* handle the special case */
    int flag_a = num_check(num_a);
    int flag_b = num_check(num_b);
    if (-1 == flag_a || -1 == flag_b)
    {
        printf("illegal number\n");
        return ;
    }
    if (0 == flag_a || 0 == flag_b)
    {
        product[0] = '0';
        return ;
    }
    
    
    /* pre-treat numbers */
    reverse(num_a);
    reverse(num_b);
    
    /* begin multiply */
    for (index_b = 0; '\0' != num_b[index_b]; ++index_b)
    {
        for (index_a = 0; '\0' != num_a[index_a]; ++index_a)
        {
            bit_p = (num_b[index_b] - '0') * (num_a[index_a] - '0') + carry;
            carry = bit_p / 10;
            bit_p = bit_p % 10;
            bit_pro[index_a] = bit_p + '0';
        }
        if (0 != carry)
        {
            bit_pro[index_a++] = carry + '0';
        }
        reverse(bit_pro);
        for (count = 0; count != index_b; ++count)
        {
            bit_pro[index_a] = '0';
            ++index_a;
        }
        strcpy(tmp_pro, product);
        memset(product, 0, RESSIZE);
        add(bit_pro, tmp_pro, product);
        
        /* clear the buffers */
        memset(tmp_pro, 0, RESSIZE);
        memset(bit_pro, 0, RESSIZE);   
    }
}

/***************************************************************
 * Funciton name: div
 * Argv:          char num_a[],
                  char num_b[],
                  char product[],
 * Return:        none
 * Descirption:   do division, return the result to stdout 
***************************************************************/
void div(char num_a[], char num_b[])
{
    char remain[NUMSIZE] = {0};
    char quot[NUMSIZE] = {0};
    char mid_rem[NUMSIZE] = {0};
    char tmp_rem[NUMSIZE] = {0};
    
    /* handle the special case */
    int flag_a = num_check(num_a);
    int flag_b = num_check(num_b);
    if (-1 == flag_a || -1 == flag_b)
    {
        printf("illegal number\n");
        return ;
    }
    if (0 == flag_a)
    {
        remain[0] = '0';
        quot[0] = '0';
        printf("quotient:  %s\n", quot);
        printf("remainder: %s\n", remain);
        return ;
    }
    if (0 == flag_b)
    {
        printf("zero can't be div\n");
        return ;
    }
    
    /* begin division */
    int flag = num_cmp(num_a, num_b);
    /* the two numbers are equal */
    if (0 == flag)
    {
        remain[0] = '0';
        quot[0] = '1';
        printf("quotient:  %s\n", quot);
        printf("remainder: %s\n", remain);
        return ;
    }
    /* numerator is smaller than denomerator */
    if (-1 == flag)
    {
        strcpy(remain, num_a);
        quot[0] = '0';
        printf("quotient:  %s\n", quot);
        printf("remainder: %s\n", remain);
        return ;
    }
    /* normal case */
    strcpy(mid_rem, num_a);
    while (num_cmp(mid_rem, num_b) > 0)
    {
        num_inc(quot);
        sub(mid_rem, num_b, tmp_rem);
        strcpy(mid_rem, tmp_rem);
        memset(tmp_rem, 0, NUMSIZE);
    }
    /* remainder is 0 */
    if (num_cmp(mid_rem, num_b) == 0)
    {
        num_inc(quot);
        remain[0] = '0';
    }
    /* remainder is not 0 */
    if (num_cmp(mid_rem, num_b) < 0)
    {
        strcpy(remain, mid_rem);
    }
    
    printf("quotient:  %s\n", quot);
    printf("remainder: %s\n", remain);
};



/***************************************************************
 * Funciton name: do_large
 * Argv:          none
 * Return:        none
 * Descirption:   do large number calculation
***************************************************************/
void do_large()
{
    char num_a[NUMSIZE] = {0};
	char num_b[NUMSIZE] = {0};
	char result[RESSIZE] = {0};
	
	int choice = 0;
    /* to store the '\n' which get from scanf() */
	char c = 0;
	/* to store the return value of scanf to avoid warning */
	unsigned int ret_s = 0;

	printf("choose a operation:\n1 for add\n2 for sub\n3 for mul\n4 for div\n0 for exit\n");
	ret_s = scanf("%d%c",&choice, &c);

loop:   switch (choice)
		{
	       case 1:  
	           printf("input number a: ");
               ret_s = scanf("%s%c", num_a, &c);
               printf("input number b: ");
               ret_s =  scanf("%s%c", num_b, &c);
               add(num_a, num_b, result);
		       printf("%s\n", result); 
			   break;
	       case 2:  
	           printf("input    minuend: ");
               ret_s = scanf("%s%c", num_a, &c);
               printf("input subtarctor: ");
               ret_s = scanf("%s%c", num_b, &c);
               sub(num_a, num_b, result);
		       printf("%s\n", result); 
			   break;
	       case 3:  
	           printf("input number a: ");
               ret_s = scanf("%s%c", num_a, &c);
               printf("input number b: ");
               ret_s = scanf("%s%c", num_b, &c);
               mul(num_a, num_b, result);
		       printf("%s\n", result); 
			   break;
	       case 4:
	           printf("input the   numerator: ");
               ret_s = scanf("%s%c", num_a, &c);
               printf("input the denomerator: ");
               ret_s = scanf("%s%c", num_b, &c);
		       div(num_a, num_b); 
			   break;
		   case 0:  
		       break;
		   default:
		       printf("error! input again! please do input numbers!!\n");  
			   ret_s = scanf("%d",&choice);
			   goto loop;
		}	
}



