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
	#define Clear clear()
#endif
#ifndef Clear2
	#define Clear2 printf("\033[2J")
#endif
#ifndef Clear3
	#define Clear3 system("clear")
#endif
#ifndef fontColorSet
	#define fontColorSet(a,b) printf("\033[%d;%dm",a, b)
#endif
#ifndef gotoxy
	#define gotoxy(x,y) printf("\033[%d;%dH",x, y)
#endif

/* kbhit */
extern int ctools_kbhit();
extern int ctools_getch();
extern int ctools_kbhitGetchar();
#endif

/* 预定义windows要用到的东西 */
#ifdef _WIN32
#include <windows.h>
#include <conio.h>

#ifndef Clear
	#define Clear printf("\033[2J\033[1;1H")
#endif
#ifndef Clear2
	#define Clear2 system("cls")
#endif
#ifndef fontColorSet
	#define fontColorSet(a,b) printf("\033[%d;%dm",a, b)
#endif
#ifndef gotoxy
	#define gotoxy(x,y) printf("\033[%d;%dH",x, y)
#endif
#endif

/*
 * 新菜单
 */
struct ctools_menu_t;    /* 菜单类/结构体 */
typedef struct {
	void (*const ncurses_init)();
	void (*const data_init)(struct ctools_menu_t**);
	void (*const set_title)(struct ctools_menu_t*, char*);
	void (*const set_type)(struct ctools_menu_t*, char*);
	void (*const add_text)(struct ctools_menu_t*, ...);
	void (*const add_text_data)(struct ctools_menu_t*, char*, char*, ...);
	int  (*const show)(struct ctools_menu_t*);
} ctools_menu;
/* use by `CT_MENU` */

/*
 * 命令行操作
 */
union ctools_cmd_arg {
        char  *ch;
	char **chp;
	int    num;
};

typedef struct ctools_cmd {
	char const            *const name;
	char const            *const describe;
	union ctools_cmd_arg (*const v)(union ctools_cmd_arg);
	struct ctools_cmd     *const next;
} ctools_cmd_list;
extern ctools_cmd_list *Cmd_list;

typedef struct {
	int                  (*const cmd_list_set)(ctools_cmd_list*);
	int                  (*const input)(char*);
	union ctools_cmd_arg (*const run)(char[]);
	int                  (*const ui)(void);
	int                    const cmd_max_len;
	int                    const arg_max_len;
} ctools_cmd;

#endif

