#include "common.h"
#include "debug.h"
#include "nwin.h"


#define COMMAND_NUMS  32
#define CURSES_MOD  1

static const char  usage[]=
"Usage for tview\n"
"\n"
"tview command [option] [pattern] [file|dir]\n"
"\n"
"tview moves the command's output into ncurses interface\n"
"It just a TUI tool,you can use some keys to operate it.\n"
"\n"
"Option:\n"
"	--help	   show help\n"
"	--list	   list the current configed command\n"
"	--ignore   ignore some dirs or files\n"
"\n"
"Keymap:\n"
"	j\tmove down\n"
"	k\tmove up\n"
"	e\tedit with vim\n"
"	r\treload command's output\n"
"	q\tquit\n"
"\n"
"Example:\n"
"	tview --list\n"
"	tview ls\n"
"	tview grep hello\n"
"\n";


char * command[COMMAND_NUMS]; /*use to save the command get from config*/
int command_plain[COMMAND_NUMS] = {0};
struct CmdConvertTbl cmdtbl[] ={
	{"ls", CMD_LS},
	{"find",CMD_FIND},
	{"grep",CMD_GREP},
};

static struct keymap keymap[] = {
	{'m',  		REQ_VIEW_MAIN},
	{'q', 		REQ_VIEW_CLOSE},
	{'k',  		REQ_MOVE_UP},
	{'j',  		REQ_MOVE_DOWN},
	{'r',		REQ_RELOAD_VIEW},
	{KEY_UP, 	REQ_MOVE_UP},
	{KEY_DOWN, 	REQ_MOVE_DOWN},
	
	{'e',   	REQ_OPEN_VIM},
	{KEY_RIGHT, REQ_OPEN_VIM},
};

char vim_cmd[BUFSIZ];

enum commandType{
	IS_LS,
	IS_FIND,
	IS_GREP,
	IS_MAX,
};

int g_current;
int g_change;

/*parse mode return*/
int command_type;
int ls_type;
extern WINDOW *status_win;

char **argvs;

void
do_nothing(void)
{
	/*do nothing*/
}


static inline void
show_usage(void)
{
	printf("%s\n",usage);
}

static inline void
show_command(void)
{
	int i = 0;
	
	printf("----Current Support command ----\n");
	while(command[i])
	{
		printf("-%s\n", command[i]);
		i++;
	}
	printf("You can add some command in the config\n");
}


static BOOL 
compare_command(const char *input)
{
	int  i = 0;
	/*Here we compare the command with
	 *the list of supported command, which
	 *defined in config file.
	 * */
	while(command[i])
	{
		if(strcmp(command[i], input) == 0)
			return 1;
		i++;
	}

	return 0;
}


/**
 * if the option == --help, return 1;
 * if the option == --list, return 2;
 * because the --ignore need an parameter,
 * so in this way, argc should be 3 at least
 * */

static int
compare_option(const char *option)
{
	if((strcmp(option,"--help") == 0))
		return 1;
	else if((strcmp(option,"--list") == 0))
		return 2;
	else 
		return 0;
}

static int
convert_command_tbl(char *cmd)
{
	int j;
	/*here can use hash table to search*/
	for(j = 0; j<array_size(cmdtbl);j++)
	{
		if(strcmp(cmd,cmdtbl[j].name) == 0)
			return cmdtbl[j].value;
	}

	return -1;

}


/**
 *This function is involed when the argc == 2,
 *We just action the basement of the command
 *may some commands need argument,so here will
 *make a dicision
 */
static int
default_action(char *cmd)
{
	int ret;

	ret = convert_command_tbl(cmd);	
	switch(ret)
	{
			case CMD_LS:
				ls_type = 0;
				return IS_LS;
				break;
			case CMD_FIND:
				CMD_LOG("find","%s\n","argument is needed");
				break;
			case CMD_GREP:
				CMD_LOG("grep","%s\n","argument is needed");
				break;
			default:
				CMD_ERR(cmd,"%s","This command may not show output by ncurses");
	}
	
	return -1;
}


static int 
default_action_third(char *cmd)
{
	int ret;

	ret = convert_command_tbl(cmd);
	switch(ret)
	{
		case CMD_LS:
			ls_type = 1;
			return IS_LS;
			break;
		case CMD_FIND:
			return IS_FIND;
			break;
		case CMD_GREP:
			return IS_GREP;
			break;
		default:
			CMD_ERR(cmd,"%s","This command may not support by ncurses");
	}

	return -1;
}


int 
parser_option(int argc, char **argv)
{
	if(argc < 2){
		show_usage();
		return -1;
	}

	int ret;
	
	/**
	 * Here may be two suitations:
	 * 1. tview command
	 * 2. tview option
	 * */
	if(argc == 2)
	{
		if(compare_command(argv[1]))
		{
			ret = default_action(argv[1]);
			return ret;	
		}else if(ret = compare_option(argv[1]))
		{
			if(ret == 1)
				show_usage();
			else if(ret == 2)
				show_command();
		}else
		{
			/*error, just show the usage*/
			show_usage();
		}
	}
	
	/**
	 * Here may be some suitations
	 * 1. tview command directory
	 * 2. tview command pattern
	 * 
	 * * **/
	if(argc == 3)
	{
		if(compare_command(argv[1])) /*command with parameter*/
		{
			ret = default_action_third(argv[1]);
			return ret;
		}
	}

	return -1;
}


static void 
load_command(FILE *fp)
{
	int i = 0;
	size_t bufsize = 20;
	ssize_t nbytes;
	char *buffer = NULL;

	while((nbytes = getline(&buffer,&bufsize,fp)) != -1)
	{
		command[i] = (char *)malloc(sizeof(char) * 20);
		right_trim(buffer);
		strcpy(command[i], buffer);	
		i++;
	}


}

static void
convert_command(void)
{
	int i = 0;

	while(command[i])
	{
	    command_plain[i] = convert_command_tbl(command[i]);
		i++;
	}
}

void 
open_view(void)
{
	switch(command_type){
		case IS_LS:
			RenderLs(ls_type);
			break;
		case IS_GREP:
			RenderGrep();
			break;
		case IS_FIND:
			RenderFind();
			break;
		default:
			do_nothing();
			break;
	}
}


/*
 *This function is use to update the line
 * which will show in highlight.
 * The render function is depend on the 
 * command_type
 * */
void 
Redraw_view(void)
{
	switch(command_type){
		case IS_LS:
			Draw_LS_OutPut();
			redrawwin(stdscr);
			wrefresh(stdscr);
			break;
		case IS_GREP:
			Draw_Grep_OutPut();
			redrawwin(stdscr);
			wrefresh(stdscr);
			break;
		default:
			do_nothing();
			break;
	}
}

void
Reload_info(void)
{
	switch(command_type){
		case IS_LS:
			RenderLs(ls_type);
			redrawwin(stdscr);
			wrefresh(stdscr);
			break;
		case IS_GREP:
			RenderGrep();
			redrawwin(stdscr);
			wrefresh(stdscr);
			break;
		default:
			do_nothing();
			break;
	}
}


void
fresh_view(int key)
{
	switch(key){
		case REQ_MOVE_DOWN:
			g_change = 1;
			break;
		case REQ_MOVE_UP:
			g_change = -1;
			break;
	}
	
	Redraw_view();
}


int 
view_control(int key)
{
	switch(key){
		case REQ_MOVE_DOWN:
		case REQ_MOVE_UP:
			fresh_view(key);
			break;
		case REQ_VIEW_CLOSE:
			quit(0);
			break;
		case REQ_OPEN_VIM:
			def_prog_mode();    /*save current tty modes*/
			endwin();			/*temporarily leave curses*/
			system(vim_cmd);	/*run shell*/
			reset_prog_mode();	/*return to the previous tty mode*/
			refresh();
			break;
		case REQ_VIEW_MAIN:
			open_view(); 
			break;
		case REQ_RELOAD_VIEW:
			Reload_info();
			break;
		default:
			return 1;
	}	

	return 1;
}


static enum request
get_request(int key)
{
	int i;

	for(i = 0; i < array_size(keymap);i++){
		if(keymap[i].shortcut == key)
			return keymap[i].request;
	}

	return (enum request) key;
}



int 
main(int argc,char **argv)
{
	FILE *fp;
	char ch;

	enum request request;
	request = REQ_VIEW_MAIN;
	
	
	fp = fopen("config","r+");
	if(fp == NULL)
		T_ERR("cannot read the file config!\nplease checkout the file is exist\n");
	
	argvs = argv;

	load_command(fp);
	fclose(fp);
	convert_command();
	
	command_type = parser_option(argc, argv);
	if(command_type == -1)
		exit(1);
	
	g_current = 0;
	g_change = 0;
#if CURSES_MOD == 1
	Init_Screen();
#endif 
	
#if CURSES_MOD == 1
	while(view_control(request))
	{
		 ch = wgetch(status_win);
		 request = get_request(ch);
	}
#endif

#if CURSES_MOD == 1
	getch();
#endif

#if CURSES_MOD == 1
	quit(0);
#endif 

	return 0;
}
