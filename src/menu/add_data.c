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

/* 添加选项 */
extern void add_text(Data * data, ...)
{
	struct Data *pNew = NULL, *pTmp = NULL;
	va_list text;

	va_start(text, data);
	if (data->text != NULL) {
		free(data->text);
	}

	pNew = data->text = malloc(sizeof(struct Data));
	pNew->text = va_arg(text, char *);
	pNew->number = 1;
	pNew->describe = NULL;
	pNew->cfg = 0;
	pNew->foot = 1;
	// pNew -> max      = 2147483647;    /* 整型的最大值 */
	// pNew -> min      = -2147483648;    /* 整型的最小值 */
	pNew->max = 10000000;
	pNew->min = -10000000;
	pNew->var = NULL;
	pNew->function = NULL;

	for (int i = 2; pNew->text != NULL; i++) {
		pTmp = pNew;
		pNew->nextText = malloc(sizeof(struct Data));
		pNew = pNew->nextText;
		pNew->text = va_arg(text, char *);
		pNew->describe = NULL;
		pNew->number = i;
		pNew->cfg = 0;
		pNew->foot = 1;
		// pNew -> max      = 2147483647;    /* 整型的最大值 */
		// pNew -> min      = -2147483648;    /* 整型的最小值 */
		pNew->max = 10000000;
		pNew->min = -10000000;
		pNew->var = NULL;
		pNew->function = NULL;
	}
	free(pNew);
	if (pTmp != NULL)
		pTmp->nextText = NULL;
	va_end(text);
	return;
}

/* 添加描述信息 */
/* type:
 *  0 -> describe,
 *  1 -> type(for setting)
 *  2 -> var
 *  3 -> foot
 *  4 -> max
 *  5 -> min
 *  o -> function
 */
extern void add_text_data(Data * data, char *type, char * format, ...)
{
	struct Data *pNew;
	va_list text;

	va_start(text, format);
	pNew = data->text;
	while (*format != '\0') {
		if (*format == '%' && *(format + 1) == 's') {
#define S(t) (strcmp(t, type) == 0)
			if (S("describe")) {
				pNew->describe = va_arg(text, char *);
			} else if (S("type")) {
				pNew->cfg = va_arg(text, int);
			} else if (S("var")) {
				pNew->var = va_arg(text, int *);
			} else if (S("foot")) {
				pNew->foot = va_arg(text, int);
			} else if (S("max")) {
				pNew->max = va_arg(text, int);
			} else if (S("min")) {
				pNew->min = va_arg(text, int);
			} else {
				pNew->function = va_arg(text, void *);
			}
#undef S
			pNew = pNew->nextText;
			format++;
		} else if (*format == '\n' || *format == 'n' || *format == 'N') {
			pNew = pNew->nextText;
		}
		format++;
	}
	va_end(text);
	return;
}

