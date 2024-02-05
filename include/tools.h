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
#include <sys/ioctl.h>
#include <wait.h>
#include <pthread.h>
#include <ncurses.h>
#include <locale.h>

/* Old menu */
extern int ctools_old_menu(char *title, char *text[], int tl, int list);
extern void ctools_old_menu2(char title[], short p, short a);
extern void ctools_old_menu3(char title[]);

/* 
 * 新菜单
 */
struct ctools_menu_t;		/* 结构体数据 */
struct ctools_menu {
	/* 初始化Ncurses库(开启ncurses模式) */
	void (*const ncurses_init)();
	/* 初始化数据 */
	void (*const data_init)(struct ctools_menu_t ** data);
	/* 设置标题 */
	void (*const set_title)(struct ctools_menu_t * data, char *title);
	/* 设置菜单类型 */
	void (*const set_type)(struct ctools_menu_t * data, char *type_str);
	/* 设置文本 */
	void (*const set_text)(struct ctools_menu_t * data, ...);
	/* 添加文本 */
	void (*const add_text)(struct ctools_menu_t * data, char *new_text);
	/* 设置文本数据 */
	void (*const set_text_data)(struct ctools_menu_t * data, char *type_str, char *format, ...);
	/* 添加文本数据 */
	void (*const add_text_data)(struct ctools_menu_t * data, char *type_str, char *obj);
	/* 显示菜单 */
	int (*const show)(struct ctools_menu_t * data);
};

/*
 * 命令行操作
 */
typedef struct ctools_cmd_list {
	char const             *const name;
	char const             *const describe;
	void *                (*const v)(void *);
	struct ctools_cmd_list *const next;
} ctools_cmd_list;

struct ctools_cmd {
	int    (*const cmd_list_set)(ctools_cmd_list*);
	int    (*const input)(char*);
	void * (*const run)(char[]);
	int    (*const ui)(void);
	int      const cmd_max_len;
	int      const arg_max_len;
};

/*
 * 读取配置文件
 */
struct ctools_CONFIG_NODE;
struct ctools_config {
	char *(*readfile)(char *filename);
	struct ctools_CONFIG_NODE *(*run_file)(char *filename);
	struct ctools_CONFIG_NODE *(*run_char)(char *data);
	char *(*get_name)(struct ctools_CONFIG_NODE* node);
	int   (*get_type)(struct ctools_CONFIG_NODE* node);
	char *(*get_str)(struct ctools_CONFIG_NODE* node);
	int   (*get_num)(struct ctools_CONFIG_NODE* node);
	struct ctools_CONFIG_NODE *(*get_next_node)(struct ctools_CONFIG_NODE* node);
};

struct ctools {
	/* 菜单 */
	const struct ctools_menu menu;
	/* 命令行 */
	const struct ctools_cmd cmd;
	/* 配置文件读取 */
	const struct ctools_config config;
	/* 初始化Ncurses库(开启ncurses模式) */
	void (*const ncurses_init)();
	/* 判断有没有输入 */
	int (*const kbhit)();
	/* 不阻塞输入 */
	int (*const kbhitGetchar)();
	/* 利用终端特性做的getch */
	int (*const getch)();
	/* 用于对抗ncurses的宏定义干扰 */
	int (*const getcha)();
};
extern const struct ctools ctools_init();

#endif

