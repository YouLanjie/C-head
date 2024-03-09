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

const struct ctools ctools_init()
{
	const struct ctools ctools = {
		.cmd = ctools_cmd_init(),
		.config = ctools_config_init(),
	};
	return ctools;
}
