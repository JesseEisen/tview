#ifndef __DEBUG_H__
#define __DEBUG_H__

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ncurses.h>

#define TVIEW_DBG 1

#if TVIEW_DBG == 1
#define LOG_SHOW(fmt,...) \
	do{printf("File:%s  Line:%d  Msg:fmt",__FILE__, __LINE__,##__VA_ARGS__);}while(0)
#define LOG_FUNC()  printf("Function:%s\n",__func__)
#else
#define LOG_SHOW(fmt,...)
#define LOG_FUNC()
#endif /*TVIEW_DBG*/



#define T_ERR(fmt, ...) \
	do{endwin();printf(fmt,##__VA_ARGS__), exit(2);}while(1)

#define CMD_LOG(cmd,fmt,...) \
	do{printf("%s:"fmt,cmd,##__VA_ARGS__);}while(0)

#define CMD_ERR(cmd,fmt,...) \
	do{printf("%s:",fmt,cmd,##__VA_ARGS__); exit(1);}while(0)

#endif /*head define*/
