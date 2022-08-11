#ifndef _MENU_H_

#define _MENU_H_

#include "include.h"

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
	int           cfg;          /* 类型 */
	int           foot;         /* 调整宽度 */
	struct Text * nextText;     /* 下一条例（链表） */
};                                  /* 条例结构体 */

typedef struct _menuData{
	char        * title;                                                                      /* 标题 */
	struct Text * text;                                                                       /* 条例链表头 */
	struct Text * focus;                                                                      /* 选中的条例 */
	int           cfg;                                                                        /* 菜单状态 */
	void       (* addText)    (struct _menuData * data, ...);                                 /* 添加条例 */
	void       (* addTextData)(struct _menuData * data, int type, char * format, ...);        /* 添加条例信息 */
	void       (* getFocus)   (struct _menuData * data, int number);                          /* 更改焦点指针 */
	int        (* menuShow)   (struct _menuData * data);                                      /* 更改焦点指针 */
}menuData;                                                                                        /* 菜单类/结构体 */

// 函数声明
// ======================================================================================================================================================

extern void  menuDataInit(menuData * data);
static void _menuAddText(menuData * data, ...);
static void _menuAddTextData(menuData * data, int type, char * format, ...);
static void _menuGetFocus(menuData * data, int number);

static int  _menu(menuData * data);
static void _menuShowScreen(menuData * data);
static void _menuShowText(menuData * data, int focus, int noShowText, int allChose);
static void _menuShowDescribe(menuData * data, int focus, int focus2, int noShowText2, int * allDescribe);
static void _menuShowHelp(menuData * data, int focus, int noShowText);
static void _menuShowSitting(menuData * data, int focus, int noShowText, int allChose);
static int  _menuInput(int * input, int * focus, int * noShowText, int allChose);

#endif
