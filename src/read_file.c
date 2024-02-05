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

struct ctools_CONFIG {
	char *filename;		/* 文件名称 */
	FILE *file;		/* 文件 */
	char *base;		/* 全部数据 */
	char *curs;		/* 光标 */
	char *lineHead;		/* 行头 */
	long  size;		/* 文件大小 */
	int   line;		/* 报错用行数 */

	struct ctools_CONFIG_NODE *node;
	struct ctools_CONFIG_NODE *curs_node;
};

struct VAULE {
	int   num;
	char *begin;
	char *end;
};

struct ctools_CONFIG Config = { NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, NULL };
struct VAULE value = { 0, NULL, NULL };
enum TK_ctools_CONFIG { TK_TAG = 0, TK_ASSIGNMENT, TK_SEMICOLON, TK_STRING, TK_NUM, TK_FILE_EOF, TK_ERROR };
const static char TK_list[7][12] = {"TAG", "ASSIGNMENT", "SEMICLON", "STRING", "NUM", "FILE EOF", "ERROR"};

#define CURS (Config.curs)

#define IsDigit(c) (c>='0' && c<='9')
#define IsNonDigit(c) ( (c >= 'a' && c <= 'z') || (c == '_') || (c >= 'A' && c <= 'Z') )
#define IsLetterOrDigit(c) ( IsDigit(c) || IsNonDigit(c) )

/*
 * 读取文件
 */
static char *read_file(char *filename)
{
	Config.file = fopen(filename, "r");
	if (!Config.file)
		return NULL;

	fseek(Config.file, 0L, SEEK_END);
	Config.size = ftell(Config.file);
	Config.base = malloc(Config.size + 1);
	if (Config.base == NULL) {
		perror("The file is too big");
		goto RUN_EXIT;
	}
	fseek(Config.file, 0L, SEEK_SET);
	fread(Config.base, 1, Config.size, Config.file);

	Config.base[Config.size] = '\0';
	Config.curs = Config.lineHead = Config.base;
	Config.filename = filename;

 RUN_EXIT:
	fclose(Config.file);
	return Config.base;
}

/*
 * 跳过空白字符
 */
static void SkipWhiteSpace(void)
{
	char ch = *CURS;
	while (ch == '\t' || ch == '\v' || ch == '\f' || ch == ' ' ||
	       ch == '\r' || ch == '\n' || ch == '#') {
		switch (ch) {
		case '#':
			while (*CURS != '\n' && *CURS != '\0') {
				++CURS;
			}
			break;
		case '\n':
			++CURS;
			++(Config.line);
			Config.lineHead = CURS;
			break;
		default:
			++CURS;
			break;
		}
		ch = *CURS;
	}
}

/*
 * 获取关键字
 */
static int getToken(void)
{
	int token = 0;
	char *start = CURS;
	SkipWhiteSpace();
	if (IsNonDigit(*CURS)) {	/* TAG */
		start = CURS;
		while (IsLetterOrDigit(*CURS)) {
			++CURS;
		}
		struct ctools_CONFIG_NODE *curs = Config.node;
		if (curs == NULL) {
			Config.node = curs = malloc(sizeof(struct ctools_CONFIG_NODE));
			curs->name = NULL;
			curs->type = 0;
			curs->ch   = NULL;
			curs->num  = 0;
			curs->next = NULL;
		}
		while (curs != NULL && curs->next != NULL) {
			if (curs->name != NULL
			    && (CURS - start) == (long)strlen(curs->name)
			    && strncmp(start, curs->name, CURS - start) == 0) {
				Config.curs_node = curs;
				token = TK_TAG;
				goto GET_TOKEN_EXIT;
			}
			curs = curs->next;
		}
		if (curs->name != NULL && curs->next == NULL) {
			curs->next = malloc(sizeof(struct ctools_CONFIG_NODE));
			curs = curs->next;
			if (!curs) {
				token = TK_ERROR;
				goto GET_TOKEN_EXIT;
			}
		}
		curs->name = malloc(sizeof(char) * (CURS - start + 1));
		int i = 0;
		for (i = 0; i < (CURS - start); ++i) {
			curs->name[i] = *(start + i);
		}
		curs->name[i] = '\0';
		curs->type = 0;
		curs->ch   = NULL;
		curs->num  = 0;
		curs->next = NULL;
		Config.curs_node = curs;
		/* token = TK_ERROR; */
		token = TK_TAG;
	} else if (*CURS == '=') {	/* 等于号（赋值） */
		++CURS;
		token = TK_ASSIGNMENT;
	} else if (*CURS == '"') {	/* 字符 */
		value.begin = CURS;
		while (1) {
			++CURS;
			if (*CURS == '\n' || *CURS == '\0') {
				value.end = value.begin;
				token = TK_ERROR;
				printf("%s:%d:expect \"(second),token '%s',str '%s'\n",
				       Config.filename, Config.line, TK_list[token],
				       value.begin);
				break;
			} else if (*CURS == '"') {
				value.end = CURS;
				++CURS;
				token = TK_STRING;
				break;
			}
		}
	} else if (IsDigit(*CURS) || *CURS == '.' || *CURS == '-') {	/* 数字 */
		if (*CURS == '-') {
			++CURS;
		}
		while (IsDigit(*CURS)) {
			++CURS;
		}

		if (*CURS == '.') {
			++CURS;
			while (IsDigit(*CURS)) {
				++CURS;
			}
		}
		value.num = strtod((char *)start, NULL);
		token = TK_NUM;
	} else if (*CURS == '\0') {	/* 结束 */
		token = TK_FILE_EOF;
	} else if (*CURS == ';') {	/* 分号 */
		++CURS;
		token = TK_SEMICOLON;
	} else {
		printf("%s:%d:%c:%s", Config.filename, Config.line, *CURS,
		       "Unrecognized character\n");
		++CURS;
		token = getToken();
	}
 GET_TOKEN_EXIT:
	return token;
}

/*
 * 重新初始化变量
 */
static int reInitVar(void)
{
	Config.filename = NULL;
	Config.curs = NULL;
	Config.line = 0;
	Config.node = NULL;
	Config.curs_node = NULL;
	return 0;
}

/*
 * 解析内容
 */
static struct ctools_CONFIG_NODE *runner()
{
	int token = '0';
	int type = 0;

	if (CURS == NULL)
		return NULL;
 EXIT:
	while (token != TK_FILE_EOF) {
		token = getToken();
		if (token != TK_TAG)	/* 变量(tag)名称 */
			goto EXIT;

		token = getToken();
		if (token != TK_ASSIGNMENT) {	/* 等于号 */
			printf("%s:%d:expect `=`,token '%s'\n",
			       Config.filename, Config.line, TK_list[token]);
			goto EXIT;
		}

		type = token = getToken();
		if (token != TK_NUM && token != TK_STRING) {	/* 赋值内容 */
			printf("%s:%d:expect `num` or `string`,token '%s'\n",
			       Config.filename, Config.line, TK_list[token]);
			goto EXIT;
		}

		token = getToken();
		if (token != TK_SEMICOLON) {	/* 分号 */
			printf("%s:%d:expect `;`,token '%s'\n",
			       Config.filename, Config.line, TK_list[token]);
			goto EXIT;
		}
		// 运行函数
		if (type == TK_NUM) {
			Config.curs_node->type = 1;
			Config.curs_node->num = value.num;
		} else if (type == TK_STRING) {
			Config.curs_node->type = 2;
			Config.curs_node->ch = malloc(sizeof(char) * (value.end - value.begin));
			strncpy(Config.curs_node->ch, (value.begin + 1), value.end - (value.begin + 1));
			Config.curs_node->ch[value.end - (value.begin + 1)] = '\0';
		}
	}
	return Config.node;
}

/*
 * 入口 by filename
 */
static struct ctools_CONFIG_NODE *run_by_filename(char *filename)
{
	reInitVar();
	read_file(filename);
	runner();
	return Config.node;
}

/*
 * 入口 by filename
 */
static struct ctools_CONFIG_NODE *run_by_char(char *data)
{
	reInitVar();
	Config.curs = Config.lineHead = Config.base = data;
	Config.filename = "<INPUT>";
	runner();
	return Config.node;
}

static char *rt_name(struct ctools_CONFIG_NODE *data)
{
	if (data != NULL)
		return data->name;
	else
		return NULL;
}

static char *rt_str(struct ctools_CONFIG_NODE *data)
{
	if (data != NULL)
		return data->ch;
	else
		return NULL;
}

static int rt_num(struct ctools_CONFIG_NODE *data)
{
	if (data != NULL)
		return data->num;
	else
		return 0;
}

static int rt_type(struct ctools_CONFIG_NODE *data)
{
	if (data != NULL)
		return data->type;
	else
		return 0;
}

static struct ctools_CONFIG_NODE *rt_next(struct ctools_CONFIG_NODE *data)
{
	if (data != NULL)
		return data->next;
	else
		return NULL;
}


const struct ctools_config ctools_config_init()
{
	const struct ctools_config config = {
		.readfile = read_file,
		.run_file = run_by_filename,
		.run_char = run_by_char,
		.get_name = rt_name,
		.get_type = rt_type,
		.get_str = rt_str,
		.get_num = rt_num,
		.get_next_node = rt_next,
	};
	return config;
}
