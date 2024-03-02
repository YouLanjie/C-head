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

typedef struct Node {
	char        * text;		/* 条例内容 */
	char        * describe;		/* 描述/帮助信息 */
	void       (* function)();	/* 调用的函数 */
	int         * var;		/* 调整的变量值 */
	int           id;		/* 编号 */
	int           type;		/* 类型：1数值，2开关 */
	int           foot;		/* 设置的步长 */
	int           max;		/* 设置的最大值 */
	int           min;		/* 设置的最小值 */
	struct Node * next;		/* 下一条例（链表） */
} Node;					/* 条例结构体 */

typedef struct ctools_menu_t {
	char * title;	/* 标题 */
	Node * text;	/* 条例链表头 */
	Node * focus;	/* 选中的条例 */
	int    type;	/* 菜单类型: 0.默认 1.仅显示主界面 2.显示帮助 3.显示设置 4.仅显示帮助，无输入处理 */
} Menu;			/* 菜单类/结构体 */

struct display {
	void (*const screen)(Menu*);
	void (*const text)(Menu*, int, int, int);
	void (*const describe)(Menu*, int, int, int, int*);
	void (*const help)(Menu*, int, int, int*);
	void (*const setting)(Menu*, int, int, int);
};
extern struct display display;

extern void add_text(Menu * menu, int id, char *text, char *describe, void (*func)(), int *var, char *type, int foot, int max, int min);
extern void del_text(Menu * menu, int id);
extern void set_focus(Menu *, int);

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

