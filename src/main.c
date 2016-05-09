#include "common.h"
#include "log.h"


static const char  usage[]=
"Usage for tview\n"
"tview command [option] [pattern] [file|dir]\n"
"\n"
"tview just make the command's output into ncurses view\n"
"you can use the j/k to move to the items, and you can \n"
"use the e to open the item hightlighted\n"
"\n"
"option:\n"
"		--help	   show help\n"
"		--list	   list the current configed command\n"
"		--ignore   ignore some dirs or files\n"
"\n"
"Example:\n"
"	tview --list\n"
"	tview ls\n"
"	tview grep hello\n"


int 
parser_option(int argc, char **argv)
{
	if(argc < 2)
}


int 
main(int argc,char **argv)
{

	parser_option(argc, argv);
}
