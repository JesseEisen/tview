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

struct fileinfo_grep{
	char name[128];
	char location[16];
	char content[128];
};

struct fileinfo_find{
	char name[128];
	char path[128];
	char type[32];
};

struct LS_view {
	struct fileinfo *fileinfo;
	int  fileno;
};

struct Grep_view{
	struct fileinfo_grep *grepinfo;
	int lineidx;
};

struct Find_view{
	struct fileinfo_find *findinfo;
	int lineidx;
};

enum find_command{
	FIND_DEFAULT,
	FIND_WITH_PATH,
	FIND_WITH_DEPTH_MAX,
	FIND_WITH_DEPTH_MIN,
	FIND_WITH_TYPE,
	FIND_FULL,
	FIND_REVERSE,
};

typedef struct{
	int type;
	char *command;
}FindCmd;

/*some function define*/
void RenderLs(int);
void RenderGrep(void);
void RenderFind(void);
void Draw_LS_OutPut(void);
void Draw_Grep_OutPut(void);
void quit(int);
char * right_trim(char *);
void Init_Screen(void);
int get_line_attr(enum line_type);


#endif /*nwin.h*/
