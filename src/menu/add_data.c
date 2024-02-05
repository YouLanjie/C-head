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
 * 设置选项
 */
extern void set_text(Data * data, ...)
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

/*
 * 添加选项
 */
extern void add_text(Data * data, char *text)
{
	struct Data *pNew = NULL, *pTmp = NULL;

	pNew = data->text;
	while (pNew != NULL && pNew->nextText != NULL) pNew = pNew->nextText;
	if (pNew != NULL) {
		pTmp = pNew;
		pNew->nextText = malloc(sizeof(struct Data));
		pNew = pNew->nextText;
	} else {
		pNew = malloc(sizeof(struct Data));
		data->text = pNew;
	}
	pNew->text = malloc(sizeof(char)*(strlen(text) + 1));
	strcpy(pNew->text, text);
	pNew->describe = NULL;
	if (pTmp != NULL) pNew->number = pTmp->number + 1;
	else pNew->number = 1;
	pNew->cfg = 0;
	pNew->foot = 1;
	// pNew -> max      = 2147483647;    /* 整型的最大值 */
	// pNew -> min      = -2147483648;    /* 整型的最小值 */
	pNew->max = 10000000;
	pNew->min = -10000000;
	pNew->var = NULL;
	pNew->function = NULL;
	pNew->nextText = NULL;
	return;
}

/*
 * 设定描述信息
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
extern void set_text_data(Data * data, char *type, char * format, ...)
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
				char *ch = va_arg(text, char *);
				if (strcmp(ch, "number"))
					pNew->cfg = 2;
				else if (strcmp(ch, "button"))
					pNew->cfg = 1;
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

/*
 * 增加数据
 *
 * All support type list:
 *  [x] 0 <-> describe,
 *  [x] 1 <-> type(for setting)
 *  [ ] 2 <-> var
 *  [x] 3 <-> foot
 *  [x] 4 <-> max
 *  [x] 5 <-> min
 *  [ ] o <-> function
 */
extern void add_text_data(Data * data, char *type, char * text)
{
	struct Data *pNew = NULL;

	pNew = data->text;
	while (pNew != NULL && pNew->describe != NULL) pNew = pNew->nextText;
	if (pNew == NULL) return;

#define S(t) (strcmp(t, type) == 0)
	if (S("describe")) {
		pNew->describe = malloc(sizeof(char)*(strlen(text) + 1));
		strcpy(pNew->describe, text);
	} else if (S("type")) pNew->cfg  = strtod(text, NULL);
	else if (S("foot"))   pNew->foot = strtod(text, NULL);
	else if (S("max"))    pNew->max  = strtod(text, NULL);
	else if (S("min"))    pNew->min  = strtod(text, NULL);
#undef S
	return;
}

