/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：cmd.c
 *   创 建 者：youlanjie
 *   创建日期：2023年03月31日
 *   描    述：命令行
 *
 */


#include "./cmd.h"

static int help();
static int quit();

Cmd Cmd_List[] = {
	{"help",  "获得帮助", help,  &Cmd_List[1]},
	{"exit",  "离开程序", quit,  &Cmd_List[2]},
	{"quit",  "离开程序", quit,  &Cmd_List[3]},
	{"set",   "设置值",   set,   &Cmd_List[4]},
	{"print", "打印值",   print, &Cmd_List[5]},
	{"list",  "列出值",   list,  &Cmd_List[6]},
	{"\0",    "空行",     NULL, NULL}
};

Cmd *Cmd_list = NULL;

Opt Opt_header[] = {
	{"rules", "unmove", NULL}
};

#ifdef BUILD_AS_MAIN
int main(void)
{
	char    name[CMD_MAX_LEN] = "\0";

	while (1) {
		printf("\033[1;32m"
		       "Please input a command > "
		       "\033[0m");
		cmd_input(name);
		printf("\n");
		int stat = run_cmd(name);
		if (stat) {
			printf("Someting is wrong.  --  \033[1;31m:(\033[0m\n"
			       "Try input `help`\n");
		}
	}
	return 0;
}
#endif

/*
 * 获取帮助
 */
static int help(void)
{
	struct Cmd *
		tmp = Cmd_List;

	printf("\033[1;33mCommand list:\033[0m\n");
	while (tmp != NULL) {
		printf("%s  --  %s\n", tmp->name, tmp->describe);
		tmp = tmp->next;
	}
	return 0;
}

/*
 * 离开程序
 */
static int quit(void)
{
	exit(0);
	return 0;
}


