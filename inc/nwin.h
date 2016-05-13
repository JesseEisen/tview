#ifndef __NWIN_H__
#define __NWIN_H__

#include "common.h"

#if  __GNUC__ >= 3
#define __NORETURN  __attribute__((__noreturn__))
#else
#define __NORETURN
#endif

/*some default value*/
#define COLOR_DEFAULT  (-1)

/* line type    	String	 Foreground    Background  Attributes
 *-----------  	  --------  -------------  ---------- ----------- */
/*UI colors */
#define LINE_INFO \
LINE(DEFAULT,		"", 	COLOR_DEFAULT, COLOR_DEFAULT, A_NORMAL),\
LINE(CURSOR,		"", 	COLOR_WHITE,   COLOR_GREEN,   A_BOLD),\
LINE(STATUS,		"", 	COLOR_GREEN,   COLOR_DEFAULT, 0),\
LINE(DELIMITER,		"", 	COLOR_MAGENTA, COLOR_DEFAULT, 0),\
LINE(TITLE_FOCUS,	"", 	COLOR_WHITE,   COLOR_BLUE,    A_BOLD),\
LINE(FILE_NAME,		"", 	COLOR_BLUE,    COLOR_DEFAULT, 0),\
LINE(FILE_LINUM,	"", 	COLOR_GREEN,   COLOR_DEFAULT, 0),\
LINE(FILE_LINCON,	"", 	COLOR_DEFAULT, COLOR_DEFAULT, 0),\
LINE(ERR,		    "", 	COLOR_RED,     COLOR_DEFAULT, 0),\


enum line_type{
#define LINE(type, line, fg, bg, attr) \
	LINE_##type
	LINE_INFO
#undef LINE
};

struct line_infos{
	const char *line;   /*The start of line to match*/
	int linelen;		/*Size of string to match*/
	int fg, bg, attr;   /*Color and text attribution for the lines*/
};

struct fileinfo{
	char name[128]; 
	char type[32];  /*filetype: dir/regular/pipe/...*/
	char size[32];
};

struct LS_view {
	struct fileinfo *fileinfo;
	int  fileno;
};

#endif /*nwin.h*/
