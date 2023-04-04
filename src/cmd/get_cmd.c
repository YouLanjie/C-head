/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：get_cmd.c
 *   创 建 者：youlanjie
 *   创建日期：2023年03月31日
 *   描    述：获取输入
 *
 */


#include "./cmd.h"

/*
 * 解析传入字符串查找并运行命令
 */
int run_cmd(char command[CMD_MAX_LEN])
{
	int   stat = 0;
	char  arg[ARG_MAX_LEN]  = "\0";

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
				Arg   ARG = {.ch = arg};
				tmp->v(ARG);
			}
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		return -1;
	}
	return 0;
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

