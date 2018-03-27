/**************************************************************
 * Copyright:   WHICC
 * File name:   base.c
 * Description: this file consit of definitions of many basic 
                function
 * Version:     1.0
 * Date:        2013-11-25
 * History:     none
***************************************************************/

#include "base.h"

/***************************************************************
 *  Function name: print_frame
 *  Argv:          none
 *	Return:        none
 *	Discription:   print a frame to wrap the message
***************************************************************/
void print_frame(void)
{
	int i =0;
	for (i = 0; i < 81; ++i)
	{
	    printf("\033[34m*\033[0m");
	}	
	putchar('\n');
}

/***************************************************************
 *  Function name: show_ver
 *  Argv:          none
 *	Return:        none
 *	Description:   show the version information about the program
***************************************************************/
void show_ver(void)
{
	print_frame();
	printf("\033[34mtiny calculator\n\033[0m");
	printf("\033[34mversion 1.0\n\033[0m");
	printf("\033[34mby wangjie\n\033[0m");
	print_frame();
}

/***************************************************************
 *  Function name: show_help
 *  Argv:          none
 *	Return:        none
 *	Description:   show some help information
***************************************************************/
void show_help(void)
{
	print_frame();
	printf("\033[34mhere is all the valid command parameters:\n\033[0m");
	printf("\033[34m-v: show the version informations about the program\n\033[0m");
	printf("\033[34m-h: get help about the usage of shell command line\n\033[0m");
	print_frame();
}

/***************************************************************
 *  Function name: bad_param
 *  Argv:          none
 *	Return:        none
 *  Description:   show some tips when a bad parameter passed in
***************************************************************/
void bad_param(void)
{
    print_frame();
	printf("\033[34minvalid parameter\n\033[0m");
	printf("\033[34muse '-h' in shell to get usage\n\033[0m");
	printf("\033[34muse 'help' to get more information when running\n\033[0m");
	printf("\033[34mnow the program is running anyway\n\033[0m");
	print_frame();
}

/***************************************************************
 *  Function name: do_help
 *  argv:          none
 *  return:        none
 *  Description:   show the usage of the program
***************************************************************/
void do_help()
{
	print_frame();
	printf("\033[34mthis calculator could only handle non-negative integers\n\033[0m");
	printf("\033[34mtype a expression and the result will return\n\033[0m");
	printf("\033[34mbelow are all the commands:\n\033[0m");
	printf("\033[34m	'large': if you want to calculate large numbers, like longer than fifty\n\033[0m");
	printf("\033[34m	'quit' : to quit the program\n\033[0m");
	print_frame();
}

/***************************************************************
 *  Function name: do_cal
 *  Argv:          a,
 		           b,
				   c
 *	Return:        ret,
 *  Description:   do the calculattion
***************************************************************/
int do_cal(int a, int b, char c, int *flag)
{
	int ret = 0;
    switch (c)
	{
	    case '+':
		    ret = a + b;
			break;
		case '-':
			ret = a - b;
			break;
		case '*':
		    ret = a * b;
			break;
		case '/':
		    ret = a / b;
			break;
		default:
		    printf("unknown symbol\n");
			*flag = FALSE;
			break;
	}

	return ret;
}

/***************************************************************
 * Funciton name: swap
 * Argv:          char *a,
                  char *b,
 * Return:        none
 * Descirption:   exchange the value of *a and *b 
***************************************************************/
void swap(char *a,char *b)
{ 
   char temp;
   
   temp = *a;
   *a = *b;
   *b = temp;
}

/***************************************************************
 * Funciton name: reverse
 * Argv:          char a[],
 * Return:        none
 * Descirption:   reverse the string 
***************************************************************/
void reverse(char a[])
{
	int len = strlen(a);
	int i = 0;
	
	for( ;i<len/2;i++)
	{
        swap(&a[i],&a[len-1-i]);
    }
}

/***************************************************************
 * Funciton name: num_check
 * Argv:          char a[],
 * Return:        0   if a is zero
                  1   if a is not zero
                  -1  if a is not legal number  
 * Descirption:   check a given number 
***************************************************************/
int num_check(char a[])
{
    if ('0' == a[0] && '\0' == a[1])
    {
        return 0;
    }
    if ('0' == a[0] && '\0' != a[1])
    {
        return -1;
    }
    return 1;
}


/***************************************************************
 * Funciton name: num_cmp
 * Argv:          char num_a[],
                  char num_b[],
 * Return:        1   if num_a > num_b
                  -1  if num_a < num_b
                  0   if num_a == num_b
 * Descirption:   cmpare two numbers in string 
***************************************************************/
int num_cmp(char a[], char b[])
{
    int len_a = strlen(a);
    int len_b = strlen(b);
    
    if (len_a > len_b)
    {
        return 1;
    }
    if (len_a < len_b)
    {
        return -1;
    }
    int index = 0;
    while ('\0' != a[index])
    {
        if (a[index] > b[index])
        {
            return 1;
        }
        if (a[index] < b[index])
        {
            return -1;
        }
        ++index;
    }
    return 0;
}


/***************************************************************
 * Funciton name: align
 * Argv:          char a[],
                  char b[],
 * Return:        none
 * Descirption:   string a and b represent numbers,so they must be 
                  the same length.  
***************************************************************/
void align(char a[],char b[])
{
   int i = 0;
   int len_a = strlen(a);
   int len_b = strlen(b);
   
   /* b is longer, then make a's length equal to b's with 0 */
   if (len_a < len_b)
   {  
       for (i = len_a; i < len_b; ++i)
       {
           a[i] = '0';
       }
	   a[i] = '\0';
   }
   /* a is longer, then make b's length equal to a's with 0 */
   else if (len_a > len_b)
   {
	  for (i = len_b;i < len_a;++i) 
	  {
	      b[i] = '0';
	  }
	  b[i] = '\0';
   }
   /* a and b is the same length, do nothing */
}



