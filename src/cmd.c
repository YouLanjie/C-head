/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：cmd.c
 *   创 建 者：youlanjie
 *   创建日期：2023年03月31日
 *   描    述：命令行
 *
 */


#include "./include.h"

#define CMD_MAX_LEN 1024
#define ARG_MAX_LEN 1024

typedef struct Opt {
	char       *name;
	char       *data;
	struct Opt *next;
} Opt;

/* 设置命令列表 */
static int cmd_list_set(ctools_cmd_list*);
/* 提供一种输入方式 */
static int cmd_input(char*);
/* 运行命令（面向客户使用） */
static void * cmd_run(char *command);
/* 运行默认提供的tui交互界面 */
static int cmd_tui(void);

const struct ctools_cmd ctools_cmd_init()
{
	const struct ctools_cmd cmd = {
		.cmd_list_set = cmd_list_set,
		.input        = cmd_input,
		.run          = cmd_run,
		.ui           = cmd_tui,
		.cmd_max_len  = CMD_MAX_LEN,
		.arg_max_len  = ARG_MAX_LEN,
	};
	return cmd;
}

static void *help();
static void *quit();

static void *set(void *format);
static void *print(void *format);
static void *get(void *result);
static void *list(void *type);
static void *csave(void *type);
static void *cread(void *type);

/* 默认命令列表 */
static ctools_cmd_list Cmd_List[] = {
	{"help",  "获得帮助", help,  &Cmd_List[1]},
	{"exit",  "离开程序", quit,  &Cmd_List[2]},
	{"quit",  "离开程序", quit,  &Cmd_List[3]},
	{"set",   "设置值",   set,   &Cmd_List[4]},
	{"print", "打印值",   print, &Cmd_List[5]},
	{"list",  "列出值",   list,  &Cmd_List[6]},
	{"get",   "获取值",   get,   &Cmd_List[7]},
	{"save",  "保存值",   csave, &Cmd_List[8]},
	{"read",  "读取值",   cread, &Cmd_List[9]},
	{"\0",    "空行",     NULL,  NULL}
};
/* 用户命令列表 */
ctools_cmd_list *Cmd_list = NULL;

/* 起个头 */
static Opt *Opt_header;

static int ret_val = 0;

/*
 * 设置额外的命令列表
 */
static int cmd_list_set(ctools_cmd_list *list)
{
	Cmd_list = list;
	return 0;
}

/*
 * 运行默认提供的TUI交互界面
 */
static int cmd_tui(void)
{
	char name[CMD_MAX_LEN] = "\0";

	/* 面向用户应该实现的形式 */
	/* cmd_run("set cmd_line=Please input a command >"); */

	/* 系统内部实现的形式 */
	set("cmd_line=Please input a command >");

	while (1) {
		/* 面向用户应该实现的形式 */
		/* arg = cmd_run("get cmd_line"); */

		/* 系统内部实现的形式 */
		char *tips;
		tips = get("cmd_line");

		if (tips != NULL) {    /* 若有自定义提示 */
			printf("\033[1;32m%s \033[0m", tips);
		} else {    /* 若为空 */
			printf("\033[1;32m%s \033[0m",
			       "Please input a command >");
		}
		/* 获取输入 */
		cmd_input(name);
		printf("\n");
		int *stat = cmd_run(name);
		/* 判断报错 */
		if (stat != NULL && *stat == -1) {
			printf("Command not found: %s\n"
			       "Someting is wrong.  --  \033[1;31m:(\033[0m\n"
			       "Try input `help`\n", name);
		}
	}
	return 0;
}

/*
 * 获取帮助
 */
static void *help(void)
{
	ctools_cmd_list *
		tmp = Cmd_List;

	printf("\033[1;33mCommand list:\033[0m\n");
	while (tmp != NULL) {
		printf("%s \t\t--  %s\n", tmp->name, tmp->describe);
		tmp = tmp->next;
	}
	tmp = Cmd_list;
	if (tmp != NULL) {
		printf("\033[1;33mUser Command list:\033[0m\n");
	}
	while (tmp != NULL) {
		printf("%s \t\t--  %s\n", tmp->name, tmp->describe);
		tmp = tmp->next;
	}
	return NULL;
}

/*
 * 离开程序
 */
static void *quit(void)
{
	exit(0);
	return NULL;
}

/*
 * 解析传入字符串查找并运行命令
 */
static void *cmd_run(char *command)
{
	char *cmd = NULL;
	char *arg = NULL;
	void *result = NULL;

	if (command == NULL) return NULL;

	/* 分离字符串和参数 */
	for (cmd = command; command != NULL && *command != '\0' && *command != ' '; command++);
	if (command != NULL && *command != '\0') {
		*command = '\0';
		arg = command + 1;
	}
	ctools_cmd_list * tmp = Cmd_List;
	while (tmp != NULL) {
		if (strcmp(cmd, tmp->name) == 0) {
			/* printf("%s  --  %s\n", tmp->name, tmp->describe); */
			if (tmp->v != NULL) {
				result = tmp->v(arg);
			}
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL && Cmd_list != NULL) {
		tmp = Cmd_list;
		while (tmp != NULL) {
			if (strcmp(cmd, tmp->name) == 0) {
				/* printf("%s  --  %s\n", tmp->name, tmp->describe); */
				if (tmp->v != NULL) {
					result = tmp->v(arg);
				}
				break;
			}
			tmp = tmp->next;
		}
	}
	if (tmp == NULL) {
		ret_val = -1;
		return &ret_val;
	}
	return result;
}

/*
 * 获取输入
 */
static int cmd_input(char *cmd)
{
	int     input = 0,
		num   = 0;
	while (input != '\n' && input != '\r') {
		cmd[num] = '\0';
		input = ctools_getch();
		switch (input) {
		case 0x7f: {
			if (num != 0) {
				num--;
				if(cmd[num] & 0x80) {
					cmd[num] = '\0';
					num--;
					cmd[num] = '\0';
					num--;
					cmd[num] = '\0';
					printf("\b\b  \b\b");
				} else {
					cmd[num] = '\0';
					printf("\b \b");
				}
			}
			break;
		}
		case 0x1B: {
			if (ctools_kbhit() == 1) {
				break;
			} else {
				input = '\n';
			}
			break;
		}
		case '\n':
		case '\r': {
			break;
		}
		case '\t': {
			cmd[num] = input;
			cmd[num + 1] = '\0';
			num++;
			printf(" ");
			break;
		}
		default:
			cmd[num] = input;
			cmd[num + 1] = '\0';
			num++;
			printf("%c", input);
			break;
		}
	}
	return 0;
}

/*
 * 设置值
 */
static void *set(void *format)
{
	char *ch = format;
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

		for (p = Opt_header; p != NULL && p->name != NULL && strcmp(p->name, name) != 0 ; p = p->next);
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
		while (p != NULL && p != tmp && p->next != NULL) {
			p = p->next;
		}
		if (p != NULL && p != tmp) p->next = tmp;
		if (Opt_header == NULL) {
			Opt_header = tmp;
		}

		/* printf("Susseed\n" */
		/*        "Name: %s\n" */
		/*        "Var:  %s\n", */
		/*        tmp->name, tmp->data); */
	}
	return NULL;
}

/*
 * 打印值
 */
static void *print(void *format)
{
	struct Opt *p = Opt_header;
	if (p == NULL) {
		printf("No var\n");
		ret_val = -1;
		return &ret_val;
	}
	while (p != NULL) {
		if (strcmp(p->name, format) == 0) {
			printf("Name: %s\n"
			       "Var:  %s\n",
			       p->name, p->data);
			ret_val = 0;
			return &ret_val;
		}
		p = p->next;
	}
	if (p == NULL) {
		printf("没有这个变量！\n");
	}
	ret_val = 0;
	return &ret_val;
}

/*
 * 获取变量
 */
static void *get(void *result)
{
	struct Opt *p = Opt_header;
	if (p == NULL) {
		printf("No var\n");
		ret_val = -1;
		return &ret_val;
	}
	while (p != NULL) {
		if (strcmp(p->name, result) == 0) {
			result = p->data;
			return result;
		}
		p = p->next;
	}
	if (p == NULL) {
		return NULL;
	}
	return NULL;
}


/*
 * 列出所有的变量列表
 */
static void *list(void *type)
{
	struct Opt *p = Opt_header;

	if (p == NULL) {
		printf("No var\n");
		ret_val = -1;
		return &ret_val;
	}

	while (p != NULL) {
		switch (type ? ((char*)type)[0] : '0') {
		case '1':
			printf("\033[1;33m--------------------\033[0m\n"
			       "Name: %s\n"
			       "Var:  %s\n"
			       "\033[1;33m--------------------\033[0m\n",
			       p->name, p->data);
			break;
		case 'h':
			printf("usage: list [1] [h]\n"
			       "options:\n"
			       "     1 使用分割线\n"
			       "     h 显示此内容\n");
			return type;
			break;
		default:
			printf("Name: %s\n"
			       "Var:  %s\n",
			       p->name, p->data);
			break;
		}
		p = p->next;
	}
	return type;
}


/*
 * 保存变量
 */
static void *csave(void *type)
{
	struct Opt *p = Opt_header;
	FILE *fp = NULL;

	if (p == NULL || type == NULL) {
		printf("请指定文件名！\n");
		ret_val = -2;
		return &ret_val;
	}

	fp = fopen(type, "w");
	if (!fp) {
		printf("打开文件失败！\n");
		ret_val = -3;
		return &ret_val;
	}

	while (p != NULL) {
		fprintf(fp, "%s=%s\n",
		       p->name, p->data);
		p = p->next;
	}
	fclose(fp);
	ret_val = 0;
	return &ret_val;
}

/*
 * 读取变量
 */
static void *cread(void *type)
{
	FILE *fp = NULL;

	if (type == NULL) {
		printf("请指定文件名！\n");
		ret_val = -2;
		return &ret_val;
	}

	fp = fopen(type, "r");
	if (!fp) {
		printf("打开文件失败！\n");
		ret_val = -3;
		return &ret_val;
	}

	char cmd[CMD_MAX_LEN] = "\0";
	char cmd2[CMD_MAX_LEN + 4] = "\0";
	while (fgets(cmd, CMD_MAX_LEN, fp) != NULL) {
		cmd[strlen(cmd) - 1] = '\0';
		sprintf(cmd2, "set %s", cmd);
		cmd_run(cmd2);
	}
	fclose(fp);
	ret_val = 0;
	return &ret_val;
}

