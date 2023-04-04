/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：opt.c
 *   创 建 者：youlanjie
 *   创建日期：2023年04月01日
 *   描    述：变量相关
 *
 */


#include "./cmd.h"

/*
 * 设置值
 */
int set(Arg format)
{
	char *ch = format.ch;
	char name[CMD_MAX_LEN] = "\0";
	char var[CMD_MAX_LEN] = "\0";
	int  i = 0;

	while (ch != NULL && *ch != '\0') {
		i = 0;
		while (*ch != '\0' && *ch != '=') {
			name[i] = *ch;
			i++;
			ch++;
		}
		name[i] = '\0';
		ch++;
		if (*ch == '\0') {
			break;
		}

		i = 0;
		while (*ch != '\0' && *ch != ';' && *ch != '\n') {
			var[i] = *ch;
			i++;
			ch++;
		}
		if (*ch == ';') {
			ch++;
		}
		var[i] = '\0';

		struct Opt
			*tmp = NULL,
			*p   = NULL;
		for (p = Opt_header; p != NULL && strcmp(p->name, name) != 0 ; p = p->next);
		if (p == NULL) {
			tmp = malloc(sizeof(struct Opt));
			tmp->name = tmp->data = NULL;
			tmp->next = NULL;
		} else {
			tmp = p;
			free(tmp->name);
			free(tmp->data);
			tmp->name = tmp->data = NULL;
		}
		tmp->name = malloc(strlen(name) + 1);
		strcpy(tmp->name, name);
		tmp->data = malloc(strlen(var) + 1);
		strcpy(tmp->data, var);

		p = Opt_header;
		while (p != NULL && p->next != NULL) {
			p = p->next;
		}
		if (p != tmp) {
			p->next = tmp;
		}

		printf("Susseed\n"
		       "Name: %s\n"
		       "Var:  %s\n",
		       tmp->name, tmp->data);
	}
	return 0;
}

/*
 * 打印值
 */
int print(Arg format)
{
	struct Opt *p = Opt_header;
	if (p == NULL) {
		return -1;
	}
	while (p != NULL) {
		if (strcmp(p->name, format.ch) == 0) {
			printf("Name: %s\n"
			       "Var:  %s\n",
			       p->name, p->data);
			return 0;
		}
		p = p->next;
	}
	if (p == NULL) {
		printf("没有这个变量！\n");
	}
	return 0;
}

/*
 * 获取变量
 */
int get(Arg result)
{
	struct Opt *p = Opt_header;
	if (p == NULL) {
		return -1;
	}
	while (p != NULL) {
		if (strcmp(p->name, *result.chp) == 0) {
			*result.chp = p->name;
			return 0;
		}
		p = p->next;
	}
	if (p == NULL) {
		return -1;
	}
	return 0;
}


/*
 * 列出所有的变量列表
 */
int list(Arg type)
{
	struct Opt *p = Opt_header;

	if (p == NULL) {
		return -1;
	}

	while (p != NULL) {
		switch (type.ch[0]) {
		case '1': {
			printf("\033[1;33m--------------------\033[0m\n"
			       "Name: %s\n"
			       "Var:  %s\n"
			       "\033[1;33m--------------------\033[0m\n",
			       p->name, p->data);
			break;
		}
		default:
			printf("Name: %s\n"
			       "Var:  %s\n",
			       p->name, p->data);
			break;
		}

		p = p->next;
	}
	return 0;
}


