/*
 *   Copyright (C) 2024 u0_a221
 *   
 *   文件名称：init.c
 *   创 建 者：u0_a221
 *   创建日期：2024年02月05日
 *   描    述：
 *
 */

#include "include.h"

extern const struct ctools_menu menu;
extern const struct ctools_cmd cmd;
extern const struct ctools_config config;

const struct ctools ctools_init()
{
	const struct ctools ctools = {
		.menu = ctools_menu_init(),
		.cmd = ctools_cmd_init(),
		.config = ctools_config_init(),
		.ncurses_init = ctools_ncurses_init,
		.kbhit = ctools_kbhit,
		.getch = ctools_getch,
		.getcha = ctools_getch,
		.kbhitGetchar = ctools_kbhitGetchar,
	};
	return ctools;
}
