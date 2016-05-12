#include "common.h"
#include "debug.h"
#include "nwin.h"
#include <unistd.h>

struct line_infos line_info[] = {
#define LINE(type, line, fg, bg, attr) \
	{ (line), sizeof(line),(fg),(bg), (attr)}
	LINE_INFO
#undef LINE
};

BOOL cursed = FALSE;
WINDOW *status_win;

void __NORETURN
quit(int sig)
{
	if(cursed)
		endwin();

	exit(0);
}

void __NORETURN 
view_err(const char *err, ...)
{
	va_list args;

	endwin();
	va_start(args,err);
	fputs("error:", stderr);
	vfprintf(stderr,err, args);
	fputs("\n",stderr);
	va_end(args);

	exit(1);
}

inline int get_line_attr(enum line_type type)
{
	assert(type < array_size(line_info));
	return COLOR_PAIR(type) | line_info[type].attr;
}


void
init_colors(void)
{
	int default_bg = COLOR_BLACK;
	int default_fg = COLOR_WHITE;
	enum line_type type;

	start_color();

	if(use_default_colors() != ERR){
		default_bg = -1;
		default_fg = -1;
	}


	for(type=0; type < array_size(line_info); type++)
	{
		struct line_infos *info = &line_info[type];
		int bg = info->bg == COLOR_DEFAULT ? default_bg : info->bg;
		int fg = info->fg == COLOR_DEFAULT ? default_fg : info->fg;

		init_pair(type,fg,bg);
	}
}

void 
Init_Screen(void)
{
	int x,y;


	if(isatty(STDIN_FILENO)){
		cursed = !!initscr();
	}else {
		FILE *io = fopen("/dev/tty","r+");
		cursed = !!newterm(NULL,io,io); //
	}

	if(!cursed)
		view_err("Failed to initialize curses");

	nonl();   /*tell curses not: to do NL->CR/NL on output*/
	cbreak(); /*take input chars one at a time, no wait for*/
	noecho(); /*don't encho input*/
	leaveok(stdscr,TRUE);

	if(has_colors())
		init_colors();

	getmaxyx(stdscr,y,x);
	curs_set(0);

	status_win = newwin(1,0,y-1,0);
	if(!status_win)
		view_err("Failed to create status window");

	keypad(status_win, TRUE); /*here can input some key*/
	wbkgdset(status_win,get_line_attr(LINE_STATUS));
}



