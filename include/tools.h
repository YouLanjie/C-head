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
#include <sys/time.h>
#include <pthread.h>
#include <locale.h>

#ifdef __linux__
#include <sys/ioctl.h>
#include <wait.h>
#include <ncurses.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

/*
 * kbhit getch
 */
#ifdef __linux__
/* 判断有没有输入 */
extern int kbhit();
#endif
/* 利用终端特性做的getch */
extern int _getch(void);
extern int _getch_cond(int *cond);
/* 不阻塞输入 */
extern int kbhitGetchar();
/* Get the size(x) of the window(range:0~) */
extern int get_winsize_col();
/* Get the size(y) of the window(range:0~) */
extern int get_winsize_row();
/* 读取文件 */
extern char *_fread(FILE *fp);
/* 在指定范围内打印 */
extern int print_in_box(char *ch, int x_start, int y_start, int width, int heigh, int hide, int focus, char *color_code, int flag_hl);
/* 初始化Ncurses库(开启ncurses模式) */
extern void ctools_ncurses_init();

#ifdef __linux__
/*
 * 新菜单
 */
typedef void* cmenu;
/* 初始化数据 */
extern cmenu cmenu_create();
/* 设置标题 */
extern void cmenu_set_title(cmenu menu, char *title);
/* 设置菜单类型 
 * Key: "normal", "main_only", "help", "setting", "help_only" */
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
#endif

/*
 * 读取配置文件
 */
typedef void* cconfig;
extern int cconfig_rule_set(cconfig *rule, char *name, void *val);
extern int cconfig_run(cconfig rule, char *data);

#endif

