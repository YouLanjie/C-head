/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：cmd.c
 *   创 建 者：youlanjie
 *   创建日期：2023年03月31日
 *   描    述：命令行
 *
 */


#include "../include.h"

static Arg help();
static Arg quit();

static Arg set(Arg format);
static Arg print(Arg format);
static Arg get(Arg result);
static Arg list(Arg type);

static Cmd Cmd_List[] = {
	{"help",  "获得帮助", help,  &Cmd_List[1]},
	{"exit",  "离开程序", quit,  &Cmd_List[2]},
	{"quit",  "离开程序", quit,  &Cmd_List[3]},
	{"set",   "设置值",   set,   &Cmd_List[4]},
	{"print", "打印值",   print, &Cmd_List[5]},
	{"list",  "列出值",   list,  &Cmd_List[6]},
	{"get",   "获取值",   get,   &Cmd_List[7]},
	{"\0",    "空行",     NULL,  NULL}
};

Cmd *Cmd_list = NULL;

static Opt Opt_header[] = {
	{"rules", "unmove", NULL}
};

#define BUILD_AS_MAIN
#ifdef BUILD_AS_MAIN

/*
 * test
 */
static Arg This();

int main(void)
{
	char name[CMD_MAX_LEN] = "\0";
	Cmd list[] = {
		{"This is a test.", "Just a test!!!", NULL, &list[1]},
		{"This.", "Just a test!!!", This, NULL}
	};
	Arg arg = {.ch = NULL};

	cmd_list_set(list);

	sprintf(name, "set cmd_line=%s",
		"Please input a command >");
	run_cmd(name);

	while (1) {
		sprintf(name, "get cmd_line");
		arg = run_cmd(name);
		if (arg.ch != NULL) {
			printf("\033[1;32m%s \033[0m",
			       arg.ch);
		} else {
			printf("\033[1;32m%s \033[0m",
			       "Please input a command >");
		}
		cmd_input(name);
		printf("\n");
		int stat = run_cmd(name).num;
		if (stat == -1) {
			printf("Someting is wrong.  --  \033[1;31m:(\033[0m\n"
			       "Try input `help`\n");
		}
	}
	return 0;
}

static Arg This(void)
{
	static char number = 'A';
	static int num = 0;
	char cmd[CMD_MAX_LEN] = "Hello";
	Arg arg = {.num = 0};

	printf("CC是不是没有妈妈呢？\n");
	printf("答案当然是坑定的啦！\n");
	sprintf(cmd, "set %c=%d", number, num);
	run_cmd(cmd);
	num++;
	number++;
	sprintf(cmd, "get rules");
	arg = run_cmd(cmd);
	if (arg.ch != NULL) {
		printf("%s\n", arg.ch);
	}
	return arg;
}
#endif

/*
 * 设置额外的命令列表
 */
int cmd_list_set(Cmd *list)
{
	Cmd_list = list;
	return 0;
}


/*
 * 获取帮助
 */
static Arg help(void)
{
	struct Cmd *
		tmp = Cmd_List;

	printf("\033[1;33mCommand list:\033[0m\n");
	while (tmp != NULL) {
		printf("%s  --  %s\n", tmp->name, tmp->describe);
		tmp = tmp->next;
	}
	tmp = Cmd_list;
	if (tmp != NULL) {
		printf("\033[1;33mUser Command list:\033[0m\n");
	}
	while (tmp != NULL) {
		printf("%s  --  %s\n", tmp->name, tmp->describe);
		tmp = tmp->next;
	}
	Arg arg = {.num = 0};
	return arg;
}

/*
 * 离开程序
 */
static Arg quit(void)
{
	exit(0);
	Arg arg = {.num = 0};
	return arg;
}

/*
 * 解析传入字符串查找并运行命令
 */
Arg run_cmd(char command[CMD_MAX_LEN])
{
	int   stat = 0;
	char  arg[ARG_MAX_LEN]  = "\0";
	Arg   result = {.num = 0};

	/* 分离字符串和参数 */
	for (int i = 0; command[i] != '\0'; ++i) {
		if (stat) {
			arg[i - stat] = command[i];
			command[i - 1] = '\0';
		}
		/* 分隔符判定 */
		if (!stat && command[i] == ' ') {
			stat = i + 1;
		}
	}
	Cmd * tmp = Cmd_List;
	while (tmp != NULL) {
		if (strcmp(command, tmp->name) == 0) {
			/* printf("%s  --  %s\n", tmp->name, tmp->describe); */
			if (tmp->v != NULL) {
				Arg ARG = {.ch = arg};
				result = tmp->v(ARG);
			}
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL && Cmd_list != NULL) {
		tmp = Cmd_list;
		while (tmp != NULL) {
			if (strcmp(command, tmp->name) == 0) {
				/* printf("%s  --  %s\n", tmp->name, tmp->describe); */
				if (tmp->v != NULL) {
					Arg ARG = {.ch = arg};
					tmp->v(ARG);
				}
				break;
			}
			tmp = tmp->next;
		}
	}
	if (tmp == NULL) {
		result.num = -1;
		return result;
	}
	return result;
}

/*
 * 获取输入
 */
int cmd_input(char *cmd)
{
	int     input = 0,
		num   = 0;
	while (input != '\n' && input != '\r') {
		cmd[num] = '\0';
		input = ctools_getch();
		switch (input) {
		case 0x7f: {
			if (num == 0) {
				char const *wrong = "Oh no, something is wrong :(";
				strcpy(cmd, wrong);
				num = strlen(wrong);
				printf("\033[1;31m%s\033[0m", cmd);
			} else {
				num--;
				cmd[num] = '\0';
				printf("\b \b");
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
Arg set(Arg format)
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

		/* printf("Susseed\n" */
		/*        "Name: %s\n" */
		/*        "Var:  %s\n", */
		/*        tmp->name, tmp->data); */
	}
	format.num = 0;
	return format;
}

/*
 * 打印值
 */
Arg print(Arg format)
{
	struct Opt *p = Opt_header;
	if (p == NULL) {
		format.num = -1;
		return format;
	}
	while (p != NULL) {
		if (strcmp(p->name, format.ch) == 0) {
			printf("Name: %s\n"
			       "Var:  %s\n",
			       p->name, p->data);
			format.num = 0;
			return format;
		}
		p = p->next;
	}
	if (p == NULL) {
		printf("没有这个变量！\n");
	}
	format.num = 0;
	return format;
}

/*
 * 获取变量
 */
Arg get(Arg result)
{
	struct Opt *p = Opt_header;
	if (p == NULL) {
		result.num = -1;
		return result;
	}
	while (p != NULL) {
		if (strcmp(p->name, result.ch) == 0) {
			result.ch = p->data;
			return result;
		}
		p = p->next;
	}
	if (p == NULL) {
		result.ch = NULL;
		return result;
	}
	result.ch = NULL;
	return result;
}


/*
 * 列出所有的变量列表
 */
Arg list(Arg type)
{
	struct Opt *p = Opt_header;

	if (p == NULL) {
		type.num = -1;
		return type;
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
	type.num = 0;
	return type;
}

