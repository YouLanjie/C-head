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
	int           cfg;          /* 类型：1数值，2开关 */
	int           foot;         /* 设置的步长 */
	int           max;          /* 设置的最大值 */
	int           min;          /* 设置的最小值 */
	struct Text * nextText;     /* 下一条例（链表） */
};                                  /* 条例结构体 */

typedef struct _menuData{
	char        * title;    /* 标题 */
	struct Text * text;     /* 条例链表头 */
	struct Text * focus;    /* 选中的条例 */
	int           cfg;      /* 菜单状态 */
}menuData;                             /* 菜单类/结构体 */

// 函数声明
// ======================================================================================================================================================

extern void ctools_ncurse_init();
extern void ctools_menu_Init();
extern void ctools_menu_AddText(menuData * data, ...);
extern void ctools_menu_AddTextData(menuData * data, int type, char * format, ...);
extern void ctools_menu_GetFocus(menuData * data, int number);
extern int  ctools_menu_Show(menuData * data);

static void _ctools_menu_ShowScreen(menuData * data);
static void _ctools_menu_ShowText(menuData * data, int focus, int noShowText, int allChose);
static void _ctools_menu_ShowDescribe(menuData * data, int focus, int focus2, int noShowText2, int * allDescribe);
static void _ctools_menu_ShowHelp(menuData * data, int focus, int noShowText, int * allHelp);
static void _ctools_menu_ShowSitting(menuData * data, int focus, int noShowText, int allChose);
static int  _ctools_menu_Input(int * input, int * focus, int * noShowText, int allChose);

#endif
