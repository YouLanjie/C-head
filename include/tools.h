/*
 * File name: tools.h
 * Describe : C-head公开使用头文件
 */

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

/*
 * kbhit getch
 */
extern int kbhit();
extern int _getch();
extern int kbhitGetchar();
/* Get the size(x) of the window(range:0~) */
extern int get_winsize_col();
/* Get the size(y) of the window(range:0~) */
extern int get_winsize_row();

/*
 * 新菜单
 */
typedef void* cmenu;
/* 初始化数据 */
extern cmenu cmenu_create();
/* 设置标题 */
extern void cmenu_set_title(cmenu menu, char *title);
/* 设置菜单类型 */
extern void cmenu_set_type(cmenu menu, char *type_str);
/* 添加选项 */
extern void cmenu_add_text(cmenu menu, int id, char *text, char *describe, void (*func)(), int *var, char *type, int foot, int max, int min);
/* 设置选项
 * Key: "text", "describe", "func", "var", "type", "foot", "max", "min" */
extern void cmenu_set_text(cmenu menu, int id, char *tag, void *value);
/* 移除选项 */
extern void cmenu_del_text(cmenu menu, int id);
/* 显示菜单 */
extern int cmenu_show(cmenu menu);

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

