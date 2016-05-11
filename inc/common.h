#ifndef __COMMON_H__
#define __COMMON_H__


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<ncurses.h>
#include<unistd.h>
#include<assert.h>
#include<ctype.h>

/*some essention macros*/
#define BOOL   short
#define IN
#define OUT
#define FALSE  0
#define TRUE   1


#define array_size(x)  (sizeof(x)/sizeof(x[0]))
#define cat_command(x) CMD_##x



static inline 
UPPER_STR(char *s)
{
	char *p = s;
	while(p)
	{
		*p = toupper(*p);
		p++;
	}
}


/*define some common used commands*/
enum CommCommand{
	CMD_LS,
	CMD_FIND,
	CMD_GREP,
	CMD_REVERSE,
};


struct CmdConvertTbl{
	char *name;
	int  value;
};




#endif /*head define*/
