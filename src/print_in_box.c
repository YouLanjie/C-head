/*
 *   Copyright (C) 2024 u0_a99
 *
 *   文件名称：print_in_box.c
 *   创 建 者：u0_a99
 *   创建日期：2024年03月09日
 *   描    述：
 *
 */


#include "include.h"

int print_in_box(char *ch, int x_start, int y_start, int width, int heigh, int hide, int focus, char *color_code, int flag_hl)
{
	int count = 0,
	    line_num = 0;
	char buf[20] = "0";

	x_start = x_start > get_winsize_col() ? 1 : x_start;
	if (x_start < 1) x_start = 1;
	y_start = y_start > get_winsize_row() ? 1 : y_start;
	if (y_start < 1) y_start = 1;

	width = width + x_start > get_winsize_col() ? get_winsize_col() - x_start + 1 : width;
	if (width < 0) width = get_winsize_col() - x_start + 1;
	heigh = heigh + y_start > get_winsize_row() ? get_winsize_row() - y_start + 1 : heigh;
	if (heigh < 0) heigh = get_winsize_row() - y_start + 1;

	focus = focus - 1;
	if (!color_code) color_code = "\033[0m";

	printf("%s", color_code);
	for (int i1 = y_start; i1 < y_start + heigh; i1++) {
		for (int i2 = x_start; i2 < x_start + width; i2++) {
			printf("\033[%d;%dH ", i1, i2);
		}
	}
	printf("\033[%d;%dH", y_start + line_num - (hide > line_num ? 0 : hide), x_start);
	while (ch && *ch != '\0') {
		int cond_out = (count + (*ch & 0x80 ? 2 : 1) > width && ch && *ch != '\0');
		int cond_print = (line_num - hide >= 0 && line_num - hide < heigh);

		if (cond_out || *ch == '\n' || *ch == '\r') {
			/* 行数增加 */
			line_num++;
			/* 字符清零 */
			count = 0;
			/* 移动光标 */
			if (cond_print) printf("\033[%d;%dH", y_start + line_num - (hide > line_num ? 0 : hide), x_start);
			/*kbhitGetchar();*/
			/* 字符指针下移 */
			if (*ch == '\n' || *ch == '\r') {
				ch++;
				continue;
			}
		}

		buf[0] = *ch;
		buf[1] = '\0';
		if (*ch & 0x80) {
			buf[1] = ch[1];
			buf[2] = ch[2];
			buf[3] = '\0';
			ch+=2;
			count += 3;
		} else if (*ch == '\t') {
			count += 8;
			strcpy(buf, "        ");
		} else
			count++;

		int cond_hl = (line_num == focus && flag_hl);
		cond_print = (line_num - hide >= 0 && line_num - hide < heigh);
		if (cond_print) printf("%s%s%s", cond_hl ? "\033[7m" : "", buf, cond_hl ? color_code : "");
		/*kbhitGetchar();*/
		/*usleep(100000/10);*/
		/* 字符指针下移 */
		ch++;
	}
	printf("\033[0m");
	kbhitGetchar();
	return 0;
}

