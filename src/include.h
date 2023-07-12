/* File name: include.h */
/* This is a head file */

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

/* include head file */
#include "../include/tools.h"

/*
 * 读取文件配置
 */
struct ctools_CONFIG_NODE {
	char   *name;     /* 名称 */
	int     type;     /* 类型 1:数字 2:字符 */
	char   *ch;       /* 字符 */
	int     num;      /* 数字 */
	struct ctools_CONFIG_NODE *next;
};

#endif

