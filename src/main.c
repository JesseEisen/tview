#include "common.h"
#include "debug.h"

#define COMMAND_NUMS  32


static const char  usage[]=
"Usage for tview\n"
"\n"
"tview command [option] [pattern] [file|dir]\n"
"\n"
"tview just make the command's output into ncurses view\n"
"you can use the j/k to move to the items, and you can \n"
"use the e to open the item hightlighted\n"
"\n"
"Option:\n"
"	--help	   show help\n"
"	--list	   list the current configed command\n"
"	--ignore   ignore some dirs or files\n"
"\n"
"Example:\n"
"	tview --list\n"
"	tview ls\n"
"	tview grep hello\n";

char * command[COMMAND_NUMS]; /*use to save the command get from config*/
int command_plain[COMMAND_NUMS] = {0};
struct CmdConvertTbl cmdtbl[] ={
	{"ls", CMD_LS},
	{"find",CMD_FIND},
	{"grep",CMD_GREP},
};




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
	printf("You can add some command in the config\n");
	while(command[i])
	{
		printf("-%s", command[i]);
		i++;
	}
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
static void
default_action(char *cmd)
{
	int ret;

	ret = convert_command_tbl(cmd);	
	switch(ret)
	{
			case CMD_LS:
				RenderLs_dft();
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

}



int 
parser_option(int argc, char **argv)
{
	if(argc < 2)
		show_usage();

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
			default_action(argv[1]);	
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


int 
main(int argc,char **argv)
{
	FILE *fp;

	fp = fopen("config","r+");
	if(fp == NULL)
		T_ERR("cannot read the file config!\nplease checkout the file is exist\n");

	load_command(fp);
	fclose(fp);
	convert_command();
	
	parser_option(argc, argv);

	return 0;
}
