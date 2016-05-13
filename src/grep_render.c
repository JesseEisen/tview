#include "common.h"
#include "debug.h"
#include "nwin.h"

#define GREP_CMD "grep -arn %s *"
#define VIM_CMD_R "vim +%s %s"

struct Grep_view gview;


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
	int start, end;
	enum line_type type;

	start = 0; /*first set the start frome the line buffer*/
	highlight = g_current + g_change;
	if(highlight < 0)
	{
		mvwaddstr(status_win,0,0,"At the top");
		highlight = 0;
	}else if(highlight < LINE - 2){ /*full of the screen*/
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
	for(i = start, i < end; i++)
	{
		if(i == highlight)
		{
			snprintf(vim_cmd,sizeof(vim_cmd),VIM_CMD_R,gview->grepinfo[lineidx].location,gview->grepinfo[lineidx].name);
		}
	}
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
	
	string_cpy(gview->grepinfo[lineidx].name,strsplit(line,':'));

	end = strchr(line,':');
	end += 1; /*overlap colon*/
	string_cpy(gview->grepinfo[lineidx].location,strsplit(end,':'));

	end = strchr(line,':');
	end += 1;
	trim(end); /*clear the space and newline character*/
	string_cpy(gview->grepinfo[lineidx].content,end);
	
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

	while(fgets(line,COLS,fp) != NULL)
	{
		GeneratelineInfo(line);
		gview->lineidx++;
	}

	Draw_Grep_OutPut();
	refresh();

}




void
RenderGrep(void)
{
	FILE *pipe_grep;
	char cmd[BUFSIZ];
	
	pipe_grep = popen(GREP_CMD,"r");
	if(pipe_grep == NULL)
		T_ERR("can't not open the pipe for grep");

	GatherOutPut_grep(pipe_grep);

	pclose(pipe_ls);
	
}


