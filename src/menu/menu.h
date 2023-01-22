#ifndef _MENU_H_

#define _MENU_H_

#include "../include.h"

// The new menu function.
// ======================================================================================================================================================

// 定义数据类型
// ======================================================================================================================================================

struct Text {
	char        * text;         /* 条例内容 */
	char        * describe;     /* 描述/帮助信息 */
	void       (* function);    /* 调用的函数 */
	int         * var;          /* 调整的变量值 */
	int           number;       /* 编号 */
	int           cfg;          /* 类型：1数值，2开关 */
	int           foot;         /* 设置的步长 */
	int           max;          /* 设置的最大值 */
	int           min;          /* 设置的最小值 */
	struct Text * nextText;     /* 下一条例（链表） */
};                                  /* 条例结构体 */

typedef struct{
	char        * title;    /* 标题 */
	struct Text * text;     /* 条例链表头 */
	struct Text * focus;    /* 选中的条例 */
	int           cfg;      /* 菜单类型: 0.默认 1.仅显示主界面 2.显示帮助 3.显示设置 4.仅显示帮助，无输入处理 */
} ctools_menu_t;                /* 菜单类/结构体 */

// 函数声明
// ======================================================================================================================================================

/* 初始化ncurse，设置语言、颜色对 */
extern void ctools_menu_Init();
/* 初始化变量 */
extern void ctools_menu_t_init(ctools_menu_t ** tmp);
/* 添加选项 */
extern void ctools_menu_AddText(ctools_menu_t * data, ...);
/* 添加描述信息 */
extern void ctools_menu_AddTextData(ctools_menu_t * data, int type, char * format, ...);
/* 移动焦点变量到指定节点 */
extern void ctools_menu_GetFocus(ctools_menu_t * data, int number);
/* 显示菜单 */
extern int  ctools_menu_Show(ctools_menu_t * data);

static void _ctools_menu_ShowScreen(ctools_menu_t * data);
static void _ctools_menu_ShowText(ctools_menu_t * data, int focus, int noShowText, int allChose);
static void _ctools_menu_ShowDescribe(ctools_menu_t * data, int focus, int focus2, int noShowText2, int * allDescribe);
static void _ctools_menu_ShowHelp(ctools_menu_t * data, int focus, int noShowText, int * allHelp);
static void _ctools_menu_ShowSitting(ctools_menu_t * data, int focus, int noShowText, int allChose);
static int  _ctools_menu_Input(int * input, int * focus, int * noShowText, int allChose);

#endif
