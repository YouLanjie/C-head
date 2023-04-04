/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：cmd.h
 *   创 建 者：youlanjie
 *   创建日期：2023年03月31日
 *   描    述：命令行的头文件
 *
 */

#ifndef CMD_H
#define CMD_H

#include "../include.h"

#define CMD_MAX_LEN 1024
#define ARG_MAX_LEN 1024

typedef union Arg {
        char  *ch;
	char **chp;
	int    num;
} Arg;

typedef struct Opt {
	char       *name;
	char       *data;
	struct Opt *next;
} Opt;

typedef struct Cmd {
	char const  *const name;
	char const  *const describe;
	int        (*const v)(union Arg);
	struct Cmd  *const next;
} Cmd;

typedef struct Key {
	char        *name;
	char        *cmd_name;
	/* char         key; */
	/* int        (*v); */
	struct Key  *next;
} Key;

int run_cmd(char command[CMD_MAX_LEN]);
int cmd_input(char *cmd);

int set(Arg format);
int print(Arg format);
int get(Arg result);
int list(Arg type);

extern Cmd *Cmd_list;

extern Opt Opt_header[];
extern Cmd Cmd_List[];

/* #define BUILD_AS_MAIN */

#endif

