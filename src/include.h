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
 * 通用功能
 */

/* 存储单行的文本、字符串 */
struct ctools_char_t_char {
	int                         pos_x;    /* 列 */
	int                         ch;
	struct ctools_char_t_char * next_ch;
	struct ctools_char_t_char * last_ch;
};

/* 嵌套存储多行的文本、字符串 */
struct ctools_char_t_lines {
	int                          pos_y;    /* LINE */
	struct ctools_char_t_char  * data;
	struct ctools_char_t_lines * next_line;
	struct ctools_char_t_lines * last_line;
};

typedef struct ctools_char_t_box {
	struct ctools_char_t_lines * data;
	struct ctools_char_t_lines * focus_line;
	struct ctools_char_t_char  * focus_char;
}ctools_char;

/* 初始化结构体 */
int ctools_char_init(ctools_char ** chp);
/* 添加字符 */
int ctools_char_add_ch(ctools_char * chp, int pos_y, int pos_x, char * ch);
/* 插入字符 */
int ctools_char_insert_ch(ctools_char * chp, int pos_y, int pos_x, char * ch);
/* 覆盖字符 */
int ctools_char_replace_ch(ctools_char * chp, int pos_y, int pos_x, char * ch);
/* 移除字符 */
int ctools_char_del_ch(ctools_char * chp, int pos_y, int pos_x, int pos_y2, int pos_x2);
/* 移动焦点变量到指定节点 */
int ctools_char_move(ctools_char * chp, int pos_y, int pos_x);

/* 读取文本文件 */
int ctools_char_fscanf(ctools_char *chp, FILE * fp, int opt, char end_char);

#endif

