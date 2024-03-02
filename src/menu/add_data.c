/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：add_data.c
 *   创 建 者：youlanjie
 *   创建日期：2023年05月27日
 *   描    述：处理菜单数据
 *
 */


#include "./menu.h"

/*
 * 增加数据
 */
static int type_chose(char *type)
{
	if (type == NULL) return 0;
#define S(t) (strcmp(t, type) == 0)
	if (S("number")) return 1;
	else if (S("toggle") || S("button")) return 2;
#undef S
	return 0;
}

/*
 * 添加选项
 *
 * All support type list:
 *  [x] 0 <-> describe,
 *  [x] 1 <-> type(for setting)
 *  [x] 2 <-> var
 *  [x] 3 <-> foot
 *  [x] 4 <-> max
 *  [x] 5 <-> min
 *  [x] o <-> function
 */
extern void add_text(
	Menu * menu,
	int id,
	char *text,
	char *describe,
	void (*func)(),
	int *var,
	char *type,
	int foot,
	int max,
	int min)
{
	Node *pNew = NULL,
	     *pLast = NULL,
	     *pNext = NULL;
	int id_last = 0;

	pNew = menu->text;
	while (pNew != NULL && pNew->next != NULL && (id == 0 || pNew->id < id)) {
		pLast = pNew;
		pNew = pNew->next;
	}
	if (pNew != NULL && (id == 0 || pNew->id < id)) {
		/* Add */
		id_last = pNew->id;
		pLast = pNew;
		pNext = NULL;
		pNew->next = malloc(sizeof(Node));
		pNew = pNew->next;
	} else if (pNew != NULL) {
		/* Insert */
		id_last = pLast->id;
		pNext = pNew;
		pLast->next = malloc(sizeof(Node));
		pNew = pLast->next;
	} else {
		/* New Head */
		pNew = malloc(sizeof(Node));
		menu->text = pNew;
	}
	pLast != NULL && (pLast->next = pNew);

	pNew->id = id_last + 1;
	if (text) {
		pNew->text = malloc(sizeof(char)*(strlen(text) + 1));
		strcpy(pNew->text, text);
	} else {
		pNew->text = NULL;
	}
	if (describe) {
		pNew->describe = malloc(sizeof(char)*(strlen(describe) + 1));
		strcpy(pNew->describe, describe);
	} else {
		pNew->describe = NULL;
	}
	pNew->function = func;
	pNew->var = var;
	pNew->type = type_chose(type);
	pNew->foot = foot == 0 ? 1 : foot;
	// pNew -> max      = 2147483647;    /* 整型的最大值 */
	// pNew -> min      = -2147483648;    /* 整型的最小值 */
	if (max <= min) {
		pNew->max = 10000000;
		pNew->min = -10000000;
	} else {
		pNew->max = max;
		pNew->min = min;
	}
	pNew->next = pNext;

	pNew = pNew->next;
	while (pNew != NULL) pNew->id++, pNew = pNew->next;
	return;
}

extern void del_text(Menu * menu, int id)
{
	Node *pNew = NULL,
	     *pLast = NULL,
	     *pNext = NULL;
	pNew = menu->text;
	if (!pNew)
		return;
	while (pNew != NULL && pNew->next != NULL && (id == 0 || pNew->id < id)) {
		pLast = pNew;
		pNew = pNew->next;
	}
	pNext = pNew->next;
	pLast != NULL && (pLast->next = pNext);
	menu->text == pNew && (menu->text = NULL);
	menu->focus == pNew && (menu->focus = pNext ? pNext : pLast);
	free(pNew);

	pNew = pNext;
	while (pNew != NULL) pNew->id--, pNew = pNew->next;
	return;
}

