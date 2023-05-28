/*
 *   Copyright (C) 2023 YouLanjie
 *
 *   文件名称：display.c
 *   创 建 者：youlanjie
 *   创建日期：2023年05月27日
 *   描    述：处理菜单内容显示
 *
 */


#include "./menu.h"

static void Screen(Data*);
static void Text(Data*, int, int, int);
static void Describe(Data*, int, int, int, int* );
static void Help(Data*, int, int, int*);
static void Setting(Data*, int, int, int);

struct display display = {
	.screen   = Screen,
	.text     = Text,
	.describe = Describe,
	.help     = Help,
	.setting  = Setting,
};

/* 填充颜色 */
static void fill(int y1, int x1, int y2, int x2)
{
	for (int i = y1; i < y2; i++) {
		for (int i2 = x1; i2 < x2; i2++) {
			mvaddch(i, i2, ' ');
		}
	}
	return;
}

/* 绘制屏幕 */
static void Screen(Data * data)
{
	if (data == NULL) {
		return;
	}
	/* 铺上底色 */
	attron(COLOR_PAIR(C_WHITE_BLUE));
	fill(0, 0, LINES, COLS);
	/* 绘制边框 */
	box(stdscr, 0, 0);
	for (int i = 1; i < COLS; i++) {
		mvaddstr(4, i, LineH);	/* 第二横线 */
		mvaddstr(LINES - 1, i, LineH);	/* 第三横线 */
		if (data->cfg == 0 || data->cfg == 3)
			mvaddstr(6, i, LineH);	/* 第四横线 */
		
	}
	if (data->cfg == 0 || data->cfg == 3) {
		mvaddstr(5, COLS / 4 - 3, "选项");
		mvaddstr(5, COLS / 4 * 3 - 3, "描述");
	}
	for (int i = 1; i < LINES; i++) {
		mvaddstr(i, 0, LineV);	        /* 左垂线 */
		mvaddstr(i, COLS - 1, LineV);	/* 右垂线 */
		if (i >= 5 && (data->cfg == 0 || data->cfg == 3))
			mvaddstr(i, COLS / 2 - 1, LineV);	/* 中垂线 */
	}
	mvaddstr(LINES - 1, 0       , LineLD);    /*   左下角   */
	mvaddstr(LINES - 1, COLS - 1, LineRD);    /*   右下角   */
	mvaddstr(0        , COLS - 1, LineRU);    /*   右上角   */
	mvaddstr(4        , 0       , LineLC);    /* 左第一连接 */
	mvaddstr(4        , COLS - 1, LineRC);    /* 右第一连接 */
	if (data->cfg == 0 || data->cfg == 3) {
		mvaddstr(4        , COLS / 2 - 1, LineUC);    /* 中第一连接!1&2 */
		mvaddstr(LINES - 1, COLS / 2 - 1, LineDC);    /* 中第二连接!1&2 */
		mvaddstr(6        , 0           , LineLC);    /* 左第二连接!1&2 */
		mvaddstr(6        , COLS - 1    , LineRC);    /* 右第二连接!1&2 */
		mvaddstr(6        , COLS / 2 - 1, LineCC);    /*  中线交界!1&2  */
	}
	if (data->title != NULL) {
		attron(A_BOLD);
		mvaddstr(2, COLS / 2 - (int)strlen(data->title) / 2,
			 data->title);
		attroff(A_BOLD);
	}
	attroff(COLOR_PAIR(C_WHITE_BLUE));
	return;
}

/* 显示选项 */
static void Text(Data * data, int focus, int noShowText, int allChose)
{
	if (data == NULL) {
		return;
	}
	for (int i = 1;
	     /* Condition begin */
	     data->text != NULL &&
	     data->focus != NULL &&
	     i - noShowText <= allChose &&
	     i - noShowText <= LINES - 10;
	     /* Condition end */
	     i++
	     ) {
		if (i <= noShowText)
			continue;
		get_focus(data, i);
		if (i != focus) {
			attron(COLOR_PAIR(C_WHITE_BLUE));
			attroff(A_BOLD);
		} else {
			attron(COLOR_PAIR(C_WHITE_YELLOW));
			attron(A_BOLD);
		}
		move(i + 7 - noShowText, 3);
		for (int i = 0; i <= COLS / 2 - 6; i++)
			printw(" ");
		mvaddstr(i + 7 - noShowText, 3, data->focus->text);
		attroff(COLOR_PAIR(C_BLUE_WHITE));
		attroff(COLOR_PAIR(C_WHITE_YELLOW));
	}
	return;
}

static int line(int *width, char **ch, int *line_num, int *zhStat, int focus, int *stat, int noShowText, int cfg, int num) {
	if ((*width >= COLS / cfg - 5 && **ch != '\0' && *zhStat == 1)
	    || (*width >= COLS / cfg - 6 && **ch != '\0' && *zhStat == 2)
	    || **ch == '\n' || **ch == '\r') {
		/* 换行时恢复原本的颜色 */
		if (*line_num - num == focus) {
			attroff(COLOR_PAIR(C_WHITE_BLACK));
			attron(COLOR_PAIR(C_BLACK_WHITE));
		}

		/* 行数增加 */
		(*line_num)++;

		/* 移动光标 */
		if (cfg == 2)
			move(*line_num - noShowText - 1, COLS / 2 + 2);
		else
			move(*line_num - noShowText - 1, 3);

		/* 字符清零 */
		*width = 0;
		ctools_kbhitGetchar();

		/* 字符指针下移 */
		if (**ch == '\n' || **ch == '\r') {
			(*ch)++;
		}
	}
	if (*line_num - num == focus && !(*stat)) {	/* 用作高亮选中项 */
		attroff(COLOR_PAIR(C_BLACK_WHITE));
		attron(COLOR_PAIR(C_WHITE_BLACK));
		*stat = 1;
	}
	if (**ch & 0x80)    /* 判断中文字符 */
		*zhStat = 2;    /* 是中文 */
	else    *zhStat = 1;    /* 不是中文 */

	if (**ch == '\0')
		return 1;

	if (*line_num - noShowText <= LINES - 2 &&
	    *line_num - num > noShowText) {    /* 如果超过不显示的行数 */
		printw("%c", **ch);
		if (*zhStat == 2) {
			(*ch)++;
			printw("%c", **ch);
			(*ch)++;
			printw("%c", **ch);
			*width += 2;
		} else
			(*width)++;
	} else {	/* 移动光标到初始位置 */
		if (cfg == 2)
			move(*line_num - noShowText - 1, COLS / 2 + 2);
		else
			move(*line_num - noShowText - 1, 3);
		if (*zhStat == 2) {
			*ch += 2;
			(*width) += 2;
		} else
			(*width)++;
	}
	/* 字符指针下移 */
	(*ch)++;
	return 0;
}

/* 显示描述 */
/* focus:  focus_text     用于获取节点
 * focus2: focus_describe 用于高亮
 */
static void Describe(Data * data, int focus, int focus2, int noShowText, int *allDescribe)
{
	char *ch = NULL; /* 用于打印描述字符时自动折行 */
	int stat = 0,	 /* 高亮标记 */
	    width = 0,	 /* 行内字符总宽度 */
	    line_num = 9,/* 行数 */
	    zhStat = 1;	 /* 是否在打印中文 */

	if (data == NULL)    /* 若数据为空 */
		return;
	/* 打底色 */
	attron(COLOR_PAIR(C_BLACK_WHITE));
	fill(7, COLS / 2 + 1, LINES - 1, COLS - 2);

	get_focus(data, focus);    /* 获取焦点节点的描述数据 */
	if (data->focus->describe == NULL)    /* 若数据为空 */
		return;

	move(8, COLS / 2 + 2);	/* 移动位置 */
	ch = data->focus->describe;
	while (*ch != '\0') {	/* 按字符显示 */
		/* 格式判断：自动换行、换行 */
		int i = line(&width, &ch, &line_num, &zhStat, focus2, &stat, noShowText, 2, 8);
		if (i) break;
	}
	*allDescribe = line_num - 8;
	attroff(COLOR_PAIR(C_BLACK_WHITE));
	attroff(COLOR_PAIR(C_WHITE_BLACK));
	return;
}

static void Help(Data * data, int focus, int noShowText, int *allHelp)
{
	char *ch = NULL;	/* 用于打印描述字符时自动折行 */
	int width = 0,		/* 行内字符总宽度 */
	    line_num = 7,		/* 行数 */
	    i3 = 1,		/* 多少条文本 */
	    zhStat = 1,		/* 是否在打印中文 */
	    stat = 0;

	if (data == NULL || data->text == NULL)
		return;
	/* 打底色 */
	attron(COLOR_PAIR(C_BLACK_WHITE));
	fill(5, 2, LINES - 1, COLS - 2);
	move(6, 3);
	do {    /* 遍历多项 */
		get_focus(data, i3);
		if (data->focus == NULL)
			return;
		ch = data->focus->text;
		while (*ch != '\0') {    /* 遍历单项 */
			/* 格式判断：自动换行、换行 */
			int i = line(&width, &ch, &line_num, &zhStat, focus, &stat, noShowText, 1, 6);
			if (i) break;
		}
		i3++;
		/* 换行时恢复原本的颜色 */
		if (line_num - 6 == focus) {
			attroff(COLOR_PAIR(C_WHITE_BLACK));
			attron(COLOR_PAIR(C_BLACK_WHITE));
		}

		/* 行数增加 */
		line_num++;

		/* 移动光标 */
		move(line_num - noShowText - 1, 3);

		/* 字符清零 */
		width = 0;
	} while (data->focus->nextText != NULL);
	*allHelp = line_num - 7;
	attroff(COLOR_PAIR(C_BLACK_WHITE));
	attroff(COLOR_PAIR(C_WHITE_BLACK));
	return;
}

static void Setting(Data * data, int focus, int noShowText, int allChose)
{
	if (data == NULL || data->text == NULL)
		return;
	for (int i = 1;
	     i - noShowText <= allChose && i - noShowText <= LINES - 10; i++) {
		if (i <= noShowText)
			continue;
		get_focus(data, i);
		if (i != focus) {
			attroff(A_BOLD);
			attroff(COLOR_PAIR(C_WHITE_YELLOW));
			attron(COLOR_PAIR(C_WHITE_BLUE));
		} else {
			attroff(COLOR_PAIR(C_WHITE_BLUE));
			attron(COLOR_PAIR(C_WHITE_YELLOW));
			attron(A_BOLD);
		}
		move(i + 7 - noShowText, 3);
		for (int i = 0; i <= COLS / 2 - 6; i++)
			printw(" ");
		mvaddstr(i + 7 - noShowText, 3, data->focus->text);
		if (data->focus->cfg == 1 && data->focus->var != NULL) {
			mvaddch(i + 7 - noShowText, COLS / 2 - 11, '[');
			attron(A_UNDERLINE);
			printw("%7d", *(data->focus->var));
			attroff(A_UNDERLINE);
			printw("]");
		} else if (data->focus->cfg == 2 && data->focus->var != NULL) {
			if (*(data->focus->var) == 0)
				mvaddstr(i + 7 - noShowText, COLS / 2 - 5,
					 "( )");
			else
				mvaddstr(i + 7 - noShowText, COLS / 2 - 5,
					 "(*)");
		}
	}
	attroff(COLOR_PAIR(C_WHITE_BLUE));
	attroff(COLOR_PAIR(C_WHITE_YELLOW));
	return;
}

