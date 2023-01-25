/*
 *   Copyright (C) 2022 YouLanjie
 *   
 *   文件名称：char_ext.c
 *   创 建 者：youlanjie
 *   创建日期：2022年12月28日
 *   描    述：
 *
 */

#include "include.h"

int ctools_char_init(ctools_char ** chp)
{
	if (*chp != NULL) {
		return -1;
	} else {
		*chp  = (ctools_char *)malloc(sizeof(ctools_char));
		(*chp)->data= (struct ctools_char_t_lines *)malloc(sizeof(struct ctools_char_t_lines));
		(*chp)->data->pos_y         = 1;
		(*chp)->data->data          = (struct ctools_char_t_char *)malloc(sizeof(struct ctools_char_t_char));
		(*chp)->data->data->pos_x   = 1;
		(*chp)->data->data->ch      = '\0';
		(*chp)->data->data->next_ch = NULL;
		(*chp)->data->data->last_ch = NULL;
		(*chp)->data->next_line     = NULL;
		(*chp)->data->last_line     = NULL;
		(*chp)->focus_line          = (*chp)->data;
		(*chp)->focus_char          = (*chp)->data->data;
	}
	return 0;
}

#define ctools_add_ch\
	chp->focus_char          = (struct ctools_char_t_char *)malloc(sizeof(struct ctools_char_t_char));\
	chp->focus_char->pos_x   = 1;\
	chp->focus_char->ch      = '\0';\
	chp->focus_char->next_ch = NULL;\
	chp->focus_char->last_ch = NULL;

#define ctools_add_line\
	chp->focus_line            = (struct ctools_char_t_lines *)malloc(sizeof(struct ctools_char_t_lines));\
	chp->focus_line->pos_y     = 1;\
	ctools_add_ch;\
	chp->focus_line->data      = chp->focus_char;\
	chp->focus_line->next_line = NULL;\
	chp->focus_line->last_line = NULL;

/* 
 * 添加字符
 */
int ctools_char_add_ch(ctools_char * chp, int pos_y, int pos_x, char * ch)
{
	struct ctools_char_t_char * pLast = NULL;

	if (chp == NULL || pos_y < 0 || pos_y < 0 || ch == NULL || *ch == '\0') {
		return -1;
	}

	ctools_char_move(chp, pos_y, pos_x);
	pLast = chp->focus_char;

	while (ch != NULL && *ch != '\0') {
		ctools_add_ch;
		/* 搭建焦点节点对链表的联系 */
		chp->focus_char->last_ch = pLast;
		chp->focus_char->next_ch = pLast->next_ch;
		/* 搭建链表对焦点节点的联系 */
		chp->focus_char->last_ch->next_ch = chp->focus_char;    /* 上连下 */
		if (chp->focus_char->next_ch != NULL) {
			chp->focus_char->next_ch->last_ch = chp->focus_char;    /* 下连上 */
		}
		/* 存储信息 */
		chp->focus_char->ch = *ch;
		chp->focus_char->pos_x += chp->focus_char->last_ch->pos_x;
		pLast = chp->focus_char;
		ch++;
	}
	/* 重置数字编号 */
	while (chp->focus_char->next_ch != NULL) {
		chp->focus_char = chp->focus_char->next_ch;
		chp->focus_char->pos_x = chp->focus_char->last_ch->pos_x + 1;
	}
	return 0;
}

/* 
 * 插入字符
 */
int ctools_char_insert_ch(ctools_char * chp, int pos_y, int pos_x, char * ch)
{
	struct ctools_char_t_char * pLast = NULL;

	if (chp == NULL || pos_y < 0 || pos_y < 0 || ch == NULL || *ch == '\0') {
		return -1;
	}
	ctools_char_move(chp, pos_y, pos_x);
	if (chp->focus_char->last_ch == NULL) {    /* 如果是第一个字符 */
		pLast = chp->focus_char;
		/* 设置临时node */
		ctools_add_ch;
		/* 搭建焦点节点对链表的联系 */
		chp->focus_char->last_ch = pLast->last_ch;    /* NULL */
		chp->focus_char->next_ch = pLast;
		/* 搭建链表对焦点节点的联系 */
		chp->focus_char->next_ch->last_ch = chp->focus_char;    /* 下连上 */
		chp->focus_char->pos_x = 0;
		/* 重设链表头 */
		chp->focus_line->data = chp->focus_char;
	}
	ctools_char_add_ch(chp, pos_y, pos_x - 1, ch);
	if (chp->focus_line->data->pos_x == 0) {
		chp->focus_line->data = chp->focus_line->data->next_ch;
		free(chp->focus_line->data->last_ch);
		chp->focus_line->data->last_ch = NULL;
	}
	return 0;
}

/* 
 * 覆盖字符
 */
int ctools_char_replace_ch(ctools_char * chp, int pos_y, int pos_x, char * ch)
{
	struct ctools_char_t_char * pLast = NULL;

	if (chp == NULL || pos_y < 0 || pos_y < 0 || ch == NULL || *ch == '\0') {
		return -1;
	}
	ctools_char_move(chp, pos_y, pos_x);
	while (ch != NULL && *ch != '\0') {
		if (chp->focus_char == NULL) {
			return -1;
		}
		chp->focus_char->ch = *ch;
		if (chp->focus_char->next_ch == NULL) {    /* 新建 */
			pLast = chp->focus_char;
			ctools_add_ch;
			/* 搭建焦点节点对链表的联系 */
			chp->focus_char->last_ch = pLast;
			chp->focus_char->next_ch = pLast->next_ch;    /* NULL */
			/* 搭建链表对焦点节点的联系 */
			chp->focus_char->last_ch->next_ch = chp->focus_char;    /* 上连下 */
			chp->focus_char->pos_x += chp->focus_char->last_ch->pos_x;
		} else {
			chp->focus_char = chp->focus_char->next_ch;
		}
		ch++;
	}
	return 0;
}

/* 
 * 移除字符
 */
int ctools_char_del_ch(ctools_char * chp, int pos_y, int pos_x, int pos_y2, int pos_x2)
{
	if (chp == NULL || pos_y < 0 || pos_y < 0 || pos_y2 < 0 || pos_x2 < 0) {
		return -1;
	}

	if (pos_y2 == 0) {    /* in default */
		pos_y2 = pos_y;
	} else if (pos_y2 < pos_y) {    /* swap */
		pos_y  ^= pos_y2;
		pos_y2 ^= pos_y;
		pos_y  ^= pos_y2;
	}
	/* for pos x */
	if (pos_x2 < pos_x) {    /* swap */
		pos_x  ^= pos_x2;
		pos_x2 ^= pos_x;
		pos_x  ^= pos_x2;
	}

	ctools_char_move(chp, pos_y, pos_x);
	/* 移除单行到行末的字符 */
	while (chp->focus_line->pos_y   <  pos_y2 &&
	       chp->focus_char->next_ch != NULL   ) {
		/* 分割条件与语句注释 */
		chp->focus_char->next_ch->last_ch = chp->focus_char->last_ch;
		chp->focus_char = chp->focus_char->next_ch;
		free(chp->focus_char->last_ch->next_ch);
		chp->focus_char->last_ch->next_ch = chp->focus_char;
	}
	if (chp->focus_line->pos_y     <  pos_y2 &&
	    chp->focus_line->next_line != NULL   &&
	    chp->focus_char->next_ch   == NULL   ) {
		chp->focus_char->last_ch->next_ch = NULL;
		free(chp->focus_char);
		chp->focus_line = chp->focus_line->next_line;
		chp->focus_char = chp->focus_line->data;
	}
	/* 移除整行字符 */
	while (chp->focus_line->pos_y     < pos_y2 &&
	       chp->focus_line->next_line != NULL  ) {
		/* 从链表中移除该指针指向的空间 */
		chp->focus_line->next_line->last_line = chp->focus_line->last_line;
		chp->focus_line = chp->focus_line->next_line;
		free(chp->focus_line->last_line->next_line);
		chp->focus_line->last_line->next_line = chp->focus_line;
		chp->focus_char = chp->focus_line->data;
	}
	/* 移除非整行字符 */
	while (chp->focus_line->pos_y   == pos_y2 &&
	       chp->focus_char->next_ch != NULL   &&
	       chp->focus_char->pos_x   <  pos_x2 ) {
		/* 从行首到指定位置 */
		chp->focus_char->next_ch->last_ch = chp->focus_char->last_ch;
		chp->focus_char = chp->focus_char->next_ch;
		free(chp->focus_char->last_ch->next_ch);
		chp->focus_char->last_ch->next_ch = chp->focus_char;
	}
	if (chp->focus_line->pos_y     == pos_y2 &&
	    (chp->focus_char->next_ch  == NULL   ||
	     chp->focus_char->pos_x    == pos_x2 )) {
		/* 分割条件与语句注释 */
		if (chp->focus_char->last_ch != NULL) {
			chp->focus_char->last_ch->next_ch = NULL;
		}
		free(chp->focus_char);
		if (chp->focus_line->next_line == NULL) {
			chp->focus_line = chp->focus_line->last_line;
		} else {
			chp->focus_line = chp->focus_line->next_line;
		}
		if (chp->focus_line != NULL) {
			chp->focus_char = chp->focus_line->data;
		}
	}
	return 0;
}

/* 
 * 移动指针到指定，自动创建新空间
 * 坐标指定为0时不移动对应轴
 */
int ctools_char_move(ctools_char * chp, int pos_y, int pos_x)
{
	struct ctools_char_t_lines * pLast  = chp->focus_line;
	struct ctools_char_t_char  * pLast2 = NULL;

	if (chp == NULL || pos_y < 0 || pos_y < 0) {
		return -1;
	} else if (pos_y == 0 && pos_x == 0) {
		return 0;
	}

	if (pos_y == 0) {
		pos_y = chp->focus_line->pos_y;
	}

	if (pos_x == 0) {
		pos_x = chp->focus_char->pos_x;
	}

	/* 换行 */
	while (pos_y > chp->focus_line->pos_y) {    /* 目标行数比目前行数更大 */
		if (chp->focus_line->next_line == NULL) {    /* 下一行为空 */
			ctools_add_line;
			/* 搭建焦点节点对链表的联系 */
			chp->focus_line->last_line = pLast;
			chp->focus_line->next_line = pLast->next_line;
			/* 搭建链表对焦点节点的联系 */
			chp->focus_line->last_line->next_line = chp->focus_line;    /* 上连下 */
			/* 设置坐标 */
			chp->focus_line->pos_y += chp->focus_line->last_line->pos_y;
		} else {
			chp->focus_line = chp->focus_line->next_line;
		}
		pLast = chp->focus_line;
	}

	while (pos_y < chp->focus_line->pos_y && pos_y >= 1) {
		chp->focus_line = chp->focus_line->last_line;
	}

	pLast  = NULL;
	pLast2 = chp->focus_char = chp->focus_line->data;

	/* 换列 */
	while (pos_x > chp->focus_char->pos_x) {    /* 目标列数比目前行数更大 */
		if (chp->focus_char->next_ch == NULL) {    /* 下一列为空 */
			ctools_add_ch;
			/* 搭建焦点节点对链表的联系 */
			chp->focus_char->last_ch = pLast2;
			chp->focus_char->next_ch = pLast2->next_ch;
			/* 搭建链表对焦点节点的联系 */
			chp->focus_char->last_ch->next_ch = chp->focus_char;    /* 上连下 */
			/* 设置坐标 */
			chp->focus_char->pos_x += chp->focus_char->last_ch->pos_x;
		} else {
			chp->focus_char = chp->focus_char->next_ch;
		}
		pLast2 = chp->focus_char;
	}
	return 0;
}

int ctools_char_fscanf(ctools_char *chp, FILE * fp, int opt, char end_char)
{
	char ch = '\0';

	if (!fp || chp == NULL) {
		return -1;
	} else {
		while (ch != EOF) {
			ch = getc(fp);
			if (ch == EOF) {
				return 0;
			} else if (ch == end_char) {
				if (opt == 1) {
					ctools_char_add_ch(chp,
							   chp->focus_line->pos_y,
							   chp->focus_char->pos_x,
							   &ch);
				}
				return 0;
			} else {
				ctools_char_add_ch(chp,
						   chp->focus_line->pos_y,
						   chp->focus_char->pos_x,
						   &ch);
			}
		}    /* while ch != EOF */
	}    /* fp == TRUE */
	return 0;
}

