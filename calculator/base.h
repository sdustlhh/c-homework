/**************************************************************
 * Copyright:   WHICC
 * File name:   base.h
 * Description: declare some basic function, these function are
                used in base.c, cal.c, main.c, and also define
				some macros.
 * Version:     1.0
 * Date:        2013-11-25
 * History:     none
***************************************************************/

#ifndef _BASE_H_
#define _BASE_H_

#define TRUE    1
#define FALSE   0

#include <stdio.h>
#include <string.h>

void print_frame(void);
void show_ver(void);
void show_help(void);
void bad_param(void);
void do_help(void);
int do_cal(int a, int b, char c, int *flag);


void swap(char *a, char *b);
void reverse(char *a);
int num_check(char a[]);
int num_cmp(char a[], char b[]);
void align(char a[], char b[]);

#endif
