/**************************************************************
 * Copyright:   WHICC
 * File name:   cal.h
 * Description: declare some important function, these function
                do most calculate jop. they are used in cal.c, 
				also define	some macros.
 * Version:     1.0
 * Date:        2013-11-25
 * History:     none
***************************************************************/

#ifndef _CAL_H_
#define _CAL_H_

#define STACKSIZE     50 
#define BUFSIZE       150

#define NUMSIZE    100
#define RESSIZE       1000

#include <stdio.h>
#include <string.h>
#include <ctype.h>

void do_help();
int mid_to_last(char *buf, int length);
void do_expre(char *buf, int length);

void do_large();
void add(char num_a[], char num_b[], char sum[]);
void num_inc(char a[]);
void sub(char num_a[], char num_b[], char remain[]);
void mul(char num_a[], char num_b[], char product[]);
void div(char num_a[], char num_b[]);
#endif
