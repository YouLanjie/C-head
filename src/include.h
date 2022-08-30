/* File name: include.h */
/* This is a head file */

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

/* include head file */
#include <stdio.h>
#include <stdlib.h>
/* system() srand() rand() malloc() free() exit() */
#include <unistd.h>
/* pause() */
#include <string.h>
/* strcat() strcmp() strcpy() */
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
/* signal() */
#include <stdarg.h>

/* 预定义Linux要用到的东西 */
#ifdef __linux
#include <sys/ioctl.h>
#include <wait.h>
#include <pthread.h>
#include <ncurses.h>
#include <locale.h>

#ifndef Clear
	#define Clear clear();
#endif
#ifndef Clear2
	#define Clear2 clear();
#endif
#ifndef fontColorSet
	#define fontColorSet(a,b) printf("\033[%d;%dm",a, b)
#endif
#ifndef gotoxy
	#define gotoxy(x,y) printf("\033[%d;%dH",x, y)
#endif
/* kbhit */
extern int kbhit();
extern int getch_old();
#endif

/* 预定义windows要用到的东西 */
#ifdef _WIN32
#include <windows.h>
#include <conio.h>

#ifndef Clear
	#define Clear printf("\033[2J\033[1;1H");
#endif
#ifndef Clear2
	#define Clear2 system("cls");
#endif
#ifndef fontColorSet
	#define fontColorSet(a,b) printf("\033[%d;%dm",a, b)
#endif
#ifndef gotoxy
	#define gotoxy(x,y) printf("\033[%d;%dH",x, y)
#endif
#endif

/* kbhit */
extern int kbhitGetchar();

#endif

