#include "common.h"
#include "debug.h"

#define LS_DEFAULT  "ls -lk"
#define LS_WITHARG  "ls -lk %s"
#define LS_LINELEN  64
#define LS_FILENUMS 128

struct fileinfo{
	char name[128]; 
	char type[32];  /*filetype: dir/regular/pipe/...*/
	char size[32];
};

struct LS_view {
	struct fileinfo *fileinfo;
	int  fileno;
};

struct LS_view *lsview;


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

	name = lsview->fileinfo[lsview->fileno].name;
	/*second get the filetype */
	GatherFileType(name,lsview->fileinfo[lsview->fileno].type);
}


void 
Draw_LS_OutPut()
{
		
}


void 
GatherOutPut_ls(FILE *fp)
{
	char *line = NULL;
	int y = 0;
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
		mvwaddstr(stdscr,y,0,lsview->fileinfo[lsview->fileno].type);
		mvwaddstr(stdscr,y,40,lsview->fileinfo[lsview->fileno].size);
		mvwaddstr(stdscr,y,60,lsview->fileinfo[lsview->fileno].name);
		lsview->fileno++;
		y++;
	}
	/*when all the input is saved into*/
	Draw_LS_OutPut();
	refresh();
}



/**
 * this function is to show the ls
 * default order into ncurses
 * */
void
RenderLs_dft(void)
{
	FILE *pipe_ls; 
	char *lines; 

	pipe_ls = popen(LS_DEFAULT,"r");
	if(pipe_ls == NULL)
		T_ERR("can't not open the pipe for ls");
	
	GatherOutPut_ls(pipe_ls);

	pclose(pipe_ls);
}


void 
RenderLs(void)
{
	RenderLs_dft();
}


