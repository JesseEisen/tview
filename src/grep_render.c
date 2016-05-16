#include "common.h"
#include "debug.h"
#include "nwin.h"

#define GREP_CMD "grep -arn %s *"
#define VIM_CMD_R "vim +%s %s"

struct Grep_view *gview;


extern WINDOW *status_win;
extern char **argvs;
extern int g_change;
extern int g_current;
extern char vim_cmd[BUFSIZ];

#define string_cpy(dst, src) \
	string_ncopy(dst,src,sizeof(dst))


/**
 * Draw the grep output by ncurses
 * */
void 
Draw_Grep_OutPut(void)
{
	int totallines = gview->lineidx;
	int i, highlight,j=0;
	int start, end, len;
	enum line_type type;
	char *fnumber, *fname;

	start = 0; /*first set the start frome the line buffer*/
	highlight = g_current + g_change;
	if(highlight < 0)
	{
		mvwaddstr(status_win,0,0,"At the top");
		highlight = 0;
	}else if(highlight < LINES - 2){ /*full of the screen*/
		if(highlight == totallines){ /*at the end of the screen*/
			mvwaddstr(status_win,0,0,"At the end");
			highlight = totallines - 1;
		}
	}else{ /*here need to scroll*/
		start = highlight - LINES + 2; /*hidden some line*/
		if(highlight == totallines){ /*at the end of the buffer*/
			mvwaddstr(status_win,0,0,"Hit the bottom");
			highlight = totallines -1;
			start =highlight - LINES + 1;
		}
	}

	if(totallines < LINES -1)
		end = totallines; /*not more than a screen*/
	else 
		end = start + LINES;

	/*when we do this render we need to consider the length of content*/
	werase(stdscr);
	for(i = start; i < end; i++)
	{
		if(i == highlight)
		{
			fnumber = gview->grepinfo[i].location;
			fname = gview->grepinfo[i].name;
			right_trim(fnumber);
			right_trim(fname);
			snprintf(vim_cmd,sizeof(vim_cmd),VIM_CMD_R,fnumber,fname);
			type = LINE_CURSOR;
			g_current = i;
			wattrset(stdscr,get_line_attr(type));
			wchgat(stdscr,-1,0,type,NULL);
		}else{
			type = LINE_FILE_LINCON;
			wchgat(stdscr,-1,0,type,NULL);
			wattrset(stdscr,get_line_attr(LINE_FILE_NAME));
		}

		/*first set filename, in grep the filename may contain t
		 * the path, so the length of it should be controled.
		 * */
		len = strlen(gview->grepinfo[i].name);
		if(len > 25) {/*the lenght is out of name len range*/
		   if(type != LINE_CURSOR) /*normal line */
				wattrset(stdscr,get_line_attr(LINE_DELIMITER));
			mvwaddch(stdscr,j,0,'~'); /*hide some character*/
			if(type != LINE_CURSOR)
				wattrset(stdscr,get_line_attr(LINE_FILE_NAME));
			mvwaddnstr(stdscr,j,1,gview->grepinfo[i].name+(len-25),24);				
		}else{
			mvwaddstr(stdscr,j,0,gview->grepinfo[i].name);
		}

		/*add the location*/
		if(type != LINE_CURSOR){
			wattrset(stdscr,get_line_attr(LINE_FILE_LINUM));
		}
		mvwaddstr(stdscr,j,30,gview->grepinfo[i].location);

		/*add content, the content may out of the length,so we still
		 * need to hide some text*/
		len = strlen(gview->grepinfo[i].content);
		if(len > 50){
			if(type != LINE_CURSOR)
				wattrset(stdscr,get_line_attr(LINE_DELIMITER));
			mvwaddnstr(stdscr,j,45,gview->grepinfo[i].content,COLS-47);
			if(type != LINE_CURSOR)
				wattrset(stdscr,get_line_attr(LINE_FILE_NAME));
			mvwaddch(stdscr,j,COLS-2,'~');
		}else{
			mvwaddstr(stdscr,j,45,gview->grepinfo[i].content);
		}
		
		mvwaddch(stdscr,j,COLS-1,'\n');
		j++;
	}
	g_change = 0; /*clear the offset*/
}


/**
 *Because the grep -an output delimeter by
 * colon, so we just use the raw way to parse
 * it
 * */
void
GeneratelineInfo(char *line)
{
	char *end;
	
	string_cpy(gview->grepinfo[gview->lineidx].name,strsplit(line,':'));

	end = strchr(line,':');
	end += 1; /*overlap colon*/
	string_cpy(gview->grepinfo[gview->lineidx].location,strsplit(end,':'));

	end = strchr(end,':');
	end += 1;
	trim(end); /*clear the space and newline character*/
	string_cpy(gview->grepinfo[gview->lineidx].content,end);
	
}


void
GatherOutPut_grep(FILE *fp)
{
	char *line = NULL;
	
	line = (char *)malloc(sizeof(char)*COLS);
	if(line == NULL)
		T_ERR("cannot malloc space for line");
	
	gview = malloc(sizeof(struct Grep_view));
	if(gview == NULL)
		T_ERR("cannot malloc space for gview");
	gview->grepinfo = (struct fileinfo_grep *)malloc(sizeof(struct fileinfo) * 128);
	if(gview->grepinfo == NULL)
		T_ERR("cannot malloc space for grepinfo");
	gview->lineidx = 0;
#if 1
	while(fgets(line,COLS,fp) != NULL)
	{
		if(line != NULL){
			GeneratelineInfo(line);
			gview->lineidx++;
		}
	}
	if(gview->lineidx == 0){
		endwin();
		printf("can not find anything\n");
		free(gview->grepinfo);
		free(gview);
		exit(1);
	}else{
		Draw_Grep_OutPut();
		refresh();
	}
#endif 

	free(line);
}




void
RenderGrep(void)
{
	FILE *pipe_grep;
	char cmd[BUFSIZ];
	
	snprintf(cmd,sizeof(cmd),GREP_CMD,argvs[2]);
	right_trim(cmd);
	pipe_grep = popen(cmd,"r");
	if(pipe_grep == NULL)
		T_ERR("can't not open the pipe for grep");

	GatherOutPut_grep(pipe_grep);

	pclose(pipe_grep);
	
}

void 
RenderGrep1(void)
{
	FILE *pipe_grep;
	char *line;
	char cmd[BUFSIZ];

	snprintf(cmd,sizeof(cmd),GREP_CMD,argvs[2]);
	right_trim(cmd);

	pipe_grep = popen(cmd,"r");
	if(pipe_grep == NULL)
		T_ERR("cannot open pipe");

	line = (char *)malloc(sizeof(char) * 80);
	if(fgets(line,80,pipe_grep) != NULL);
		mvwaddstr(stdscr,4,0,line);

	GatherOutPut_grep(pipe_grep);
	GeneratelineInfo(line);

	getch();
	endwin();
	exit(1);
}


