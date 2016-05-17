#include "common.h"
#include "debug.h"
#include "nwin.h"

#define FIND_LINELEN  64
#define VIM_CMD_FIND "vim %s/%s"

extern int find_type;
extern char **argvs;
extern char level;
extern int g_current;
extern int g_change;
extern WINDOW* status_win;
extern char vim_cmd[BUFSIZ];


FindCmd fc[] = {
	{FIND_DEFAULT,"find . -name %s"},
	{FIND_WITH_PATH, "find %s -name %s"},
	{FIND_WITH_DEPTH_MAX,"find . -maxdepth %c -name %s"},
	{FIND_WITH_DEPTH_MIN,"find . -mindepth %c -name %s"},
	{FIND_WITH_TYPE,"find . -type %s -name %s"},
	{FIND_FULL,"find %s -type %s -name %s"},
};

struct Find_view *fdview;


void 
GenerateFileInfo_find(char *line)
{
	//assert(line == NULL);
	char *buf;
	int linelen, pathlen;

	linelen = strlen(line);
	
	buf = strrchr(line,'/');
	strcpy(fdview->findinfo[fdview->lineidx].name,buf+1);

	pathlen = linelen - strlen(buf);
	strncpy(fdview->findinfo[fdview->lineidx].path,line,pathlen);

	right_trim(line);
	GatherFileType(line,fdview->findinfo[fdview->lineidx].type);

}


void 
Draw_Find_OutPut(void)
{
	int totallines = fdview->lineidx;
	int i,highlight, j = 0;
	int start =0,end = 0;
	enum line_type  type;
	
	start = 0;
	highlight = g_current + g_change;
	if(highlight < 0){
		mvwaddstr(status_win,0,0,"At the top");
		highlight = 0;
	}else if(highlight < LINES -2){
		if(highlight == totallines) {
			mvwaddstr(status_win,0,0,"At the end");
			highlight = totallines - 1;
		}
	}else{
		start = highlight - LINES + 2;
		if(highlight == totallines){
			mvwaddstr(status_win,0,0,"Hit the bottom");
			highlight = totallines - 1;
			start = highlight - LINES + 1;
		}
	}

	if(totallines < LINES - 1)
		end = totallines;
	else if(totallines >= LINES)
		end = start + LINES;

	werase(stdscr);
	for(i = start; i < end; i++){
		if(i == highlight){
			snprintf(vim_cmd,sizeof(vim_cmd),VIM_CMD_FIND,fdview->findinfo[i].path,fdview->findinfo[i].name);
			type = LINE_CURSOR;
			g_current = i;
			wattrset(stdscr,get_line_attr(type));
			wchgat(stdscr,-1,0,type,NULL);
		}else{
			type = LINE_FILE_LINCON;
			wchgat(stdscr,-1,0,type,NULL);
			wattrset(stdscr,get_line_attr(LINE_FILE_NAME));
		}
		
		if(type != LINE_CURSOR){
			wattrset(stdscr,get_line_attr(LINE_FILE_NAME));
			mvwaddstr(stdscr,j,0,fdview->findinfo[i].path);
		}else{
			mvwaddstr(stdscr,j,0,fdview->findinfo[i].path);
		}

		if(type != LINE_CURSOR){
			wattrset(stdscr,get_line_attr(LINE_FILE_LINUM));
		}
		right_trim(fdview->findinfo[i].name);
		mvwaddstr(stdscr,j,35,fdview->findinfo[i].name);

		if(type != LINE_CURSOR){
			wattrset(stdscr,get_line_attr(LINE_DELIMITER));
		}
		mvwaddstr(stdscr,j,60,fdview->findinfo[i].type);
		mvwaddch(stdscr,j,COLS-1,'\n');
		j++;
	}
	g_change = 0;
}


void 
GatherOutPut_find(FILE *fp)
{
	char *line = NULL;

	line = (char *)malloc(sizeof(char) * FIND_LINELEN);
	if(line == NULL)
		T_ERR("cannot malloc space for line");
	
	fdview = malloc(sizeof(struct Find_view));
	fdview->findinfo = (struct fileinfo_find *)malloc(sizeof(struct fileinfo_find)*128);
	if(fdview->findinfo == NULL)
		T_ERR("cannot malloc space for fdview");
	fdview->lineidx = 0;

	while(fgets(line,FIND_LINELEN,fp) != NULL)
	{
		if(line == NULL)
			continue;

		GenerateFileInfo_find(line);
		fdview->lineidx++;
	}
	
	if(fdview->lineidx == 0){
		endwin();
		printf("can not get any output from find\n");
		quit(1);
	}

	Draw_Find_OutPut();
	refresh();

	free(line);
}



void
RenderFind(void)
{

	FILE *pipe_find;
	char cmd[BUFSIZ];
	
	/**
	 * we will create the commad according 
	 * the find_type
	 * */

	switch(find_type){
		case FIND_DEFAULT:
			sprintf(cmd,fc[FIND_DEFAULT].command,argvs[2]);
			break;
		case FIND_WITH_DEPTH_MAX:
			sprintf(cmd,fc[FIND_WITH_DEPTH_MAX].command,level,argvs[3]);
			break;
		case FIND_WITH_DEPTH_MIN:
			sprintf(cmd,fc[FIND_WITH_DEPTH_MIN].command,level,argvs[3]);
			break;
		case FIND_WITH_TYPE:
			sprintf(cmd,fc[FIND_WITH_TYPE].command,argvs[2],argvs[3]);
			break;
		case FIND_WITH_PATH:
			sprintf(cmd,fc[FIND_WITH_PATH].command,argvs[2],argvs[3]);
			break;
		case FIND_FULL:
			sprintf(cmd,fc[FIND_FULL].command,argvs[2],argvs[3],argvs[4]);
			break;
		default:
			printf("cannot show the result in curses\n may your input error\n");
			break;		
	}

	
	pipe_find = popen(cmd,"r");
	if(pipe_find == NULL)
		T_ERR("can not open the pipe for find");

	GatherOutPut_find(pipe_find);

	pclose(pipe_find);

}
