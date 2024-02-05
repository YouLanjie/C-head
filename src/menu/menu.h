/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：menu.h
 *   创 建 者：youlanjie
 *   创建日期：2023年05月27日
 *   描    述：菜单程序
 *
 */

#ifndef MENU_H
#define MENU_H

#include "../include.h"

struct Data {
	char        * text;         /* 条例内容 */
	char        * describe;     /* 描述/帮助信息 */
	void       (* function)();  /* 调用的函数 */
	int         * var;          /* 调整的变量值 */
	int           number;       /* 编号 */
	int           cfg;          /* 类型：1数值，2开关 */
	int           foot;         /* 设置的步长 */
	int           max;          /* 设置的最大值 */
	int           min;          /* 设置的最小值 */
	struct Data * nextText;     /* 下一条例（链表） */
};                                  /* 条例结构体 */

typedef struct ctools_menu_t {
	char        * title;    /* 标题 */
	struct Data * text;     /* 条例链表头 */
	struct Data * focus;    /* 选中的条例 */
	int           cfg;      /* 菜单类型: 0.默认 1.仅显示主界面 2.显示帮助 3.显示设置 4.仅显示帮助，无输入处理 */
} Data;                         /* 菜单类/结构体 */

struct display {
	void (*const screen)(Data*);
	void (*const text)(Data*, int, int, int);
	void (*const describe)(Data*, int, int, int, int*);
	void (*const help)(Data*, int, int, int*);
	void (*const setting)(Data*, int, int, int);
	/* void (*screen)(); */
};

extern void set_text(Data *, ...);
extern void add_text(Data *, char*);
extern void set_text_data(Data*, char*, char*, ...);
extern void add_text_data(Data*, char*, char*);
extern void get_focus(Data*, int);

// 定义宏
#define LineH "─"
#define LineV "│"
#define LineLU "┌"
#define LineLD "└"
#define LineRU "┐"
#define LineRD "┘"
#define LineLC "├"
#define LineRC "┤"
#define LineCC "┼"
#define LineUC "┬"
#define LineDC "┴"
#define LineCLU "╭"
#define LineCLD "╰"
#define LineCRU "╮"
#define LineCRD "╯"
#define ArrowUp "↑"
#define ArrowDn "↓"
#define ArrowLf "←"
#define ArrowRi "→"

#endif

