/* File name: tools.h */
/* This is a head file */

#ifndef _TOOLS_H_
#define _TOOLS_H_

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

/* 
 * 预定义Linux要用到的东西
 */
#ifdef __linux

#include <sys/ioctl.h>
#include <wait.h>
#include <pthread.h>
#include <ncurses.h>
#include <locale.h>

#ifndef Clear
/* 使用ncurse的 */
#define Clear clear()
#endif

#ifndef Clear2
/* 终端效果 */
#define Clear2 printf("\033[2J")
#endif

#ifndef Clear3
/* 系统 */
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

#endif

/* 
 * 预定义windows要用到的东西
 */
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

/* kbhit */
extern int ctools_kbhitGetchar();

/* menu */
extern int Menu(char *title, char *text[], int tl, int list);
extern void Menu2(char title[], short p, short a);
extern void Menu3(char title[]);


/* 
 * 新菜单
 */

struct ctools_menu_t;

/* struct ctools_menu_t;                /\* 菜单类/结构体 *\/ */
typedef struct {
	void (*const ncurses_init)();
	void (*const data_init)(struct ctools_menu_t**);
	void (*const set_title)(struct ctools_menu_t*, char*);
	void (*const set_type)(struct ctools_menu_t*, char*);
	void (*const add_text)(struct ctools_menu_t*, ...);
	void (*const add_text_data)(struct ctools_menu_t*, int, char*, ...);
	int  (*const show)(struct ctools_menu_t*);
} ctools_menu;
/* use by `CT_MENU` */

#define ctools_menu_TextTypeNumber   1
#define ctools_menu_TextTypeButton   1

#define ctools_menu_TextDataDescribe 0
#define ctools_menu_TextDataSetType  1
#define ctools_menu_TextDataSetVar   2
#define ctools_menu_TextDataSetFoot  3
#define ctools_menu_TextDataSetMax   4
#define ctools_menu_TextDataSetMin   5

/*
 * 命令行操作
 */
#define CMD_MAX_LEN 1024
#define ARG_MAX_LEN 1024

typedef struct Opt {
	char       *name;
	char       *data;
	struct Opt *next;
} Opt;

typedef union Arg {
        char  *ch;
	char **chp;
	int    num;
} Arg;

typedef struct Cmd {
	char const  *const name;
	char const  *const describe;
	Arg        (*const v)(Arg);
	struct Cmd  *const next;
} Cmd;

typedef struct Key {
	char        *name;
	char        *cmd_name;
	/* char         key; */
	/* int        (*v); */
	struct Key  *next;
} Key;

/* 设置命令列表 */
extern int cmd_list_set(Cmd *list);
/* 提供一种输入方式 */
extern int cmd_input(char *cmd);
/* 运行命令（面向客户使用） */
extern Arg cmd_run(char command[CMD_MAX_LEN]);
/* 运行默认提供的tui交互界面 */
extern int cmd_tui(void);

extern Cmd *Cmd_list;

/* 
 * 字符串扩展操作
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

/* 存储访问接口 */
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
/* 读取文本文件到该结构体 */
int ctools_char_fscanf(ctools_char *chp, FILE * fp, int opt, char end_char);

#endif

