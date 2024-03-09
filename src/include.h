/*
 * File name: include.h
 * Describe : C-head内部使用头文件
 */

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

/* include head file */
#include "../include/tools.h"

// 定义宏
#define C_WHITE_BLUE   252
#define C_BLUE_WHITE   253
#define C_WHITE_YELLOW 254
#define C_BLACK_WHITE  255
#define C_WHITE_BLACK  256

/* kbhit */
extern int ctools_kbhit();
extern int ctools_getch();
extern int ctools_kbhitGetchar();
extern void ctools_ncurses_init();

/*
 * 读取文件配置
 */
struct ctools_CONFIG_NODE {
	char  *name;		/* 名称 */
	int    type;		/* 类型 1:数字 2:字符 */
	char  *ch;		/* 字符 */
	int    num;		/* 数字 */
	struct ctools_CONFIG_NODE *next;
};

/* init使用函数 */
extern const struct ctools_cmd ctools_cmd_init();
extern const struct ctools_config ctools_config_init();

#endif

