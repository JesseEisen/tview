#ifndef __DEBUG_H__
#define __DEBUG_H__

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define TVIEW_DBG 1

#if TVIEW_DBG == 1
#define LOG_SHOW(fmt,...) \
	do{printf("File:%s  Line:%d  Msg:fmt",__FILE__, __LINE__,##__VA_ARGS__);}while(0)
#define LOG_FUNC()  printf("Function:%s\n",__func__)
#else
#define LOG_SHOW(fmt,...)
#define LOG_FUNC()
#endif /*TVIEW_DBG*/

#endif /*head define*/
