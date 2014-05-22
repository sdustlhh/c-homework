/**************************************************************
 * Copyright:   WHICC
 * File name:   base.c
 * Description: the main file, contain the main() function, program
                start here.
 * Author:      wangjie
 * Version:     1.0
 * Date:        2013-11-25
 * History:     none
***************************************************************/


#include "base.h"
#include "cal.h"

int main(int argc, char *argv[])
{
	char buf[BUFSIZE];
	int len = 0;

	/* handle the commandline parameters */
	if (1 != argc)
	{
		if (strcmp("-v", argv[1]) == 0)
		{
			show_ver();
		}
		else if (strcmp("-h",argv[1]) == 0)
		{
			show_help();
		}
		else 
		{
		    bad_param();
		}
	}

	/* handle the command when program running */
	while(1)
	{
		memset(buf, 0, BUFSIZE);
		printf("[tiny_calcultor]& ");
		if (fgets(buf, BUFSIZE, stdin) == NULL)
		{
		    return 0;
		}

		/* pre_treat the buf */
		len = strlen(buf);
		buf[len-1] = '\0';
		len = strlen(buf);

		if (strncmp(buf, "help", 4) == 0)
		{
		    do_help();
			continue;
		}
		if (strncmp(buf, "large", 5) == 0)
		{
			do_large();
            continue;
		}
		if (strncmp(buf, "quit", 4) == 0)
		{
			break;
		}
		if (isdigit(buf[0]) != 0 || '(' == buf[0])
		{
			do_expre(buf, len);
			continue;
		}
		printf("invalid command, see help to get usage\n");
	}

	return 0;
}
