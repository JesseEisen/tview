#include "common.h"
#include "debug.h"
#include "nwin.h"

#define LS_DEFAULT  "ls -lk"
#define LS_WITHARG  "ls -lk %s"
#define LS_LINELEN  64
#define LS_FILENUMS 128

#define VIM_CMD  "vim %s"
#define VIM_FULL_PATH "vim ./%s/%s"
#define NAME_PATH "./%s/%s"


struct LS_view *lsview;

extern int g_current;
extern int g_change;
extern char vim_cmd[BUFSIZ];
extern WINDOW *status_win;
extern char **argvs;
extern ls_type;


/*
 * This function is used to split ls output
 * into filename type and size.
 * */
void 
GenerateFileInfo(char *line)
{
	char *buf = line;
	char *outptr = NULL;
	char *res[1];
	char *name = NULL;
	int  i = 0;
	
	//res = malloc(sizeof(char) * 128);
	/*first step is to get the finename*/
	while((res[0] = strtok_r(buf," ",&outptr)) != NULL)
	{
		buf = NULL;
		if(i == 4){
			strcpy(lsview->fileinfo[lsview->fileno].size,res[0]);
			lsview->fileinfo[lsview->fileno].size[strlen(res[0])]='K';
			lsview->fileinfo[lsview->fileno].size[strlen(res[0])+1] = '\0';
		}

		if(i == 8){
			right_trim(res[0]);
			strcpy(lsview->fileinfo[lsview->fileno].name,res[0]);
			lsview->fileinfo[lsview->fileno].name[strlen(res[0])] = '\0';
		}
		i++;
	}

	/*here we need to add the path to name*/
	if(ls_type == 1){
		char fullname[128];	
		sprintf(fullname,NAME_PATH,argvs[2],lsview->fileinfo[lsview->fileno].name);
		name = fullname;
	}else{
		name = lsview->fileinfo[lsview->fileno].name;
	}

		
	/*second get the filetype */
	GatherFileType(name,lsview->fileinfo[lsview->fileno].type);
}

/**
 * This function is used to draw the ls output, 
 * we store all the output of the ls into a global
 * struct lsview. so we render each item one by one
 * and support the scroll.
 * */
void 
Draw_LS_OutPut()
{
	int totallines = lsview->fileno;  
	int i,highlight,j=0;
	int start, end;
	enum line_type  type;
	
	start = 0;
	highlight = g_current + g_change;
	if(highlight <= 0)
	{
		mvwaddstr(status_win,0,0,"At the top");
		highlight = 0;
	}else if(highlight <= LINES - 2){
		if(highlight == totallines) {/*mean at the end */
			mvwaddstr(status_win,0,0,"At the end");
			highlight = totallines - 1;
		}
	}else
	{
		start = highlight - LINES + 2; /*hide one line*/
		//end = highlight+1; /*current line is hightghit - 1*/
		if(highlight == totallines ){ /*really at the end of the array*/
			mvwaddstr(status_win,0,0,"Hit the bottom");
			highlight = totallines - 1;
			start=highlight - LINES + 1;
		}
	}

	
	if(totallines <= LINES-1)
		end = totallines; /*not overlap the screen*/
	else if(totallines >= LINES)
		end = start + LINES;

	werase(stdscr);
	for(i = start; i < end; i++)
	{
		if(i == highlight)
		{
			if(ls_type == 0)
				snprintf(vim_cmd,sizeof(vim_cmd), VIM_CMD,lsview->fileinfo[i].name);
			else
				snprintf(vim_cmd,sizeof(vim_cmd), VIM_FULL_PATH,argvs[2],lsview->fileinfo[i].name);
			type = LINE_CURSOR;
			g_current = i;
			wattrset(stdscr, get_line_attr(type));
			wchgat(stdscr,-1,0,type,NULL);
		}else{
			type = LINE_FILE_LINCON;
			wchgat(stdscr,-1,0,type,NULL);
			wattrset(stdscr,get_line_attr(LINE_FILE_NAME));
		}

		/*set filename, size and type*/
		if(type != LINE_CURSOR){
			wattrset(stdscr,get_line_attr(LINE_FILE_NAME));
			mvwaddstr(stdscr,j,0,lsview->fileinfo[i].name);
		}else{
			mvwaddstr(stdscr,j,0,lsview->fileinfo[i].name);
		}
		
		//wmove(stdscr,i,40);
		if(type != LINE_CURSOR){
			wattrset(stdscr,get_line_attr(LINE_FILE_LINUM));
		}

		mvwaddstr(stdscr,j,40,lsview->fileinfo[i].size);

		//wmove(stdscr,i,60);
		if(type != LINE_CURSOR){
			wattrset(stdscr, get_line_attr(LINE_DELIMITER));
		}
		mvwaddstr(stdscr,j,60,lsview->fileinfo[i].type);
		mvwaddch(stdscr,j,COLS-1,'\n');
		j++; /*j control the screen loop*/
	}
	g_change = 0; /*clear the offset*/
}


void 
GatherOutPut_ls(FILE *fp)
{
	char *line = NULL;
	void *new_space;
	int y = 0, is_expand = 0;
	char type[32];
	char size[32];

	line = (char *)malloc(sizeof(char) * LS_LINELEN);
	if(line == NULL)
		T_ERR("cannot malloc space for line");
	
	lsview = malloc(sizeof(struct LS_view));
	lsview->fileinfo =(struct fileinfo *)malloc(sizeof(struct fileinfo) * 128);
	if(lsview->fileinfo == NULL)
		T_ERR("cannot malloc space for lsview");
	lsview->fileno = 0;
	
	while(fgets(line,LS_LINELEN,fp) != NULL)
	{
		if(y == 0){  /*ignore the total line*/
			y++;
			continue;
		}
		GenerateFileInfo(line);
		lsview->fileno++;
		if(is_expand == 0 && lsview->fileno > 128)
		{	
			new_space = realloc(lsview->fileinfo, sizeof(struct fileinfo) * 128);	
			if(new_space == NULL)
				T_ERR("cannot realloc more space for lsview");
			lsview->fileinfo =(struct fileinfo *) new_space;
			is_expand = 1;
		}
	}
	/*when all the input is saved into*/
	Draw_LS_OutPut();
	refresh();
}


/**
 * para: type   judge default or expand
 * type = 0  default
 * type = 1  expand
 * */
void 
RenderLs(int type)
{
	FILE *pipe_ls; 
	char *lines;
	char cmd[BUFSIZ];
	

	if(type == 0)
		pipe_ls = popen(LS_DEFAULT,"r");
	else{
		sprintf(cmd,LS_WITHARG,argvs[2]);
		right_trim(cmd);
		pipe_ls = popen(cmd,"r");
	}

	if(pipe_ls == NULL)
		T_ERR("can't not open the pipe for ls");
	
	GatherOutPut_ls(pipe_ls);

	pclose(pipe_ls);
}


void 
RenderLs1(int type)
{
	FILE *pipe_ls;
	char *line;
	char cmd[BUFSIZ];

	sprintf(cmd,LS_WITHARG,argvs[2]);
	pipe_ls = popen(cmd,"r");
	
	line = (char *)malloc(sizeof(char) * LS_LINELEN);
	if(line == NULL)
		T_ERR("cannot malloc space for line");
	if(fgets(line,LS_LINELEN,pipe_ls) != NULL)
		mvwaddstr(stdscr,4,0,line);
	

	waddstr(stdscr,argvs[2]);
	mvwaddstr(stdscr,2,0,cmd);
	right_trim(cmd);
	mvwaddstr(stdscr,3,0,cmd);
	
	getch();
	endwin();
	exit(1);
}

