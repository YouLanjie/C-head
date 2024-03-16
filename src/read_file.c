/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：read_file.c
 *   创 建 者：youlanjie
 *   创建日期：2023年07月07日
 *   描    述：读取配置文件
 *
 */

#include "include.h"

typedef struct Rules {
	char *str;
	void **var;
	struct Rules *next;
} Rules;

struct VAULE {
	int   num;
	char *begin;
	char *end;
};

struct VAULE value = { 0, NULL, NULL };
enum TK_ctools_CONFIG { TK_TAG = 0, TK_ASSIGNMENT, TK_SEMICOLON, TK_STRING, TK_NUM, TK_FILE_EOF, TK_ERROR };
const static char *TK_list[7] = {"TAG", "ASSIGNMENT", "SEMICLON", "STRING", "NUM", "FILE EOF", "ERROR"};

#define IsDigit(c) (c>='0' && c<='9')
#define IsNonDigit(c) ( (c >= 'a' && c <= 'z') || (c == '_') || (c >= 'A' && c <= 'Z') )
#define IsLetterOrDigit(c) ( IsDigit(c) || IsNonDigit(c) )

static Rules *check(Rules *rules, char *str)
{/*{{{*/
	Rules *tmp = rules;
	while (tmp != NULL) {
		if (strcmp(str, tmp->str) == 0)
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}/*}}}*/

/*
 * 跳过空白字符
 */
static char *SkipWhiteSpace(char *curs, int *line)
{/*{{{*/
	char ch = *curs;
	while (ch == '\t' || ch == '\v' || ch == '\f' || ch == ' ' ||
	       ch == '\r' || ch == '\n' || ch == '#') {
		switch (ch) {
		case '#':
			while (*curs != '\n' && *curs != '\0') {
				++curs;
			}
			break;
		case '\n':
			++curs;
			++(*line);
			break;
		default:
			++curs;
			break;
		}
		ch = *curs;
	}
	return curs;
}/*}}}*/

/*
 * 获取关键字
 */
static int getToken(char **curs, char **ret, int *line)
{/*{{{*/
	int token = 0;

	*curs = SkipWhiteSpace(*curs, line);
	if (IsNonDigit(**curs)) {	/* TAG */
		*ret = *curs;
		while (IsLetterOrDigit(**curs)) ++(*curs);
		/* token = TK_ERROR; */
		token = TK_TAG;
	} else if (**curs == '=') {	/* 等于号（赋值） */
		++(*curs);
		token = TK_ASSIGNMENT;
	} else if (**curs == '"') {	/* 字符 */
		*ret = *curs;
		++(*ret);
		while (1) {
			++(*curs);
			if (**curs == '\n' || **curs == '\0') {
				token = TK_ERROR;
				printf(":%d:expect \"(second),token '%s',str '%s'\n",
				       *line, TK_list[token], *ret);
				break;
			} else if (**curs == '"') {
				**curs = '\0';
				++(*curs);
				token = TK_STRING;
				break;
			}
		}
	} else if (IsDigit(**curs) || **curs == '.' || **curs == '-') {	/* 数字 */
		*ret = *curs;
		if (**curs == '-') {
			++(*curs);
		}
		while (IsDigit(**curs)) {
			++(*curs);
		}

		if (**curs == '.') {
			++(*curs);
			while (IsDigit(**curs)) {
				++(*curs);
			}
		}
		token = TK_NUM;
	} else if (**curs == '\0') {	/* 结束 */
		token = TK_FILE_EOF;
	} else if (**curs == ';') {	/* 分号 */
		++(*curs);
		token = TK_SEMICOLON;
	} else {
		printf(":%d:%c:Unrecognized character\n", *line, **curs);
		++(*curs);
		token = getToken(curs, ret, line);
	}
	return token;
}/*}}}*/

/*
 * 解析内容
 */
static int run(Rules *rules, char *curs)
{/*{{{*/
	int line = 0, type = 0;
	int token = '0';
	char *ret = 0;
	int  value = 0;
	char * str;

	if (curs == NULL)
		return -1;
 EXIT:
	while (token != TK_FILE_EOF) {
		token = getToken(&curs, &ret, &line);
		if (token != TK_TAG)	/* 变量(tag)名称 */
			goto EXIT;

		token = getToken(&curs, &ret, &line);
		if (token != TK_ASSIGNMENT) {	/* 等于号 */
			printf(":%d:expect `=`,token '%s'\n",
			       line, TK_list[token]);
			goto EXIT;
		}
		*(curs - 1) = '\0';
		Rules *rule = check(rules, ret);

		type = token = getToken(&curs, &ret, &line);
		if (token == TK_NUM) {	/* 赋值内容 */
			value = strtod(ret, NULL);
		} else if (token == TK_STRING) {	/* 赋值内容 */
			str = malloc(sizeof(char) * (strlen(ret) + 1));
			strcpy(str, ret);
		} else {
			printf(":%d:expect `num` or `string`,token '%s'\n",
			       line, TK_list[token]);
			goto EXIT;
		}

		token = getToken(&curs, &ret, &line);
		if (token != TK_SEMICOLON) {	/* 分号 */
			printf(":%d:expect `;`,token '%s'\n",
			       line, TK_list[token]);
			goto EXIT;
		}
		if (!rule)
			continue;
		// 运行函数
		if (type == TK_NUM) {
			*rule->var = (void*)(long)value;
		} else if (type == TK_STRING) {
			*rule->var = str;
		}
	}
	return 0;
}/*}}}*/

/*
 * 入口 by char
 */
extern int cconfig_run(cconfig rule, char *data)
{/*{{{*/
	if (!data || !rule)
		return -1;

	Rules *rules = rule;
	char *str = malloc(sizeof(char) * (strlen(data + 1)));
	strcpy(str, data);
	run(rules, str);
	free(str);
	return 0;
}/*}}}*/

/* 设置规则 */
extern int cconfig_rule_set(cconfig *rule, char *name, void *val)
{/*{{{*/
	Rules *rules = check(*rule, name);
	if (rules == NULL && name) {
		rules = malloc(sizeof(Rules));
		rules->str = malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(rules->str, name);
		rules->var = val;
		rules->next = NULL;
	} else {
		rules->var = val;
	}
	Rules *tmp = *rule;
	while(tmp != NULL && tmp->next != NULL) {
		tmp = tmp->next;
	}
	if (!tmp)
		*rule = rules;
	else
		tmp->next = rules;
	return 0;
}/*}}}*/

