#include "./menu.h"

/* 初始化ncurse，设置语言、颜色对 */
static void ncurses_init();
/* 初始化变量 */
static void data_init(Data**);
/* 设置标题 */
static void set_title(Data*, char*);
/* 设置类型 */
static void set_type(Data*, char*);
/* 显示菜单 */
static int Show(Data*);
/* 获得输入 */
static int Input(int*, int*, int*, int, int);

extern struct display display;

ctools_menu CT_MENU = {
	.ncurses_init  = ncurses_init,
	.data_init     = data_init,
	.set_title     = set_title,
	.set_type      = set_type,
	.add_text      = add_text,
	.add_text_data = add_text_data,
	.show          = Show,
};

/* ctools_menu_Init
 * 初始化ncurse
 * 设置语言、颜色对
 */
static void ncurses_init()
{
	setlocale(LC_ALL, "zh_CN.UTF8");
	initscr();
	cbreak();    /* 取消行缓冲 */
	noecho();    /* 不回显 */
	curs_set(0);    /* 隐藏光标 */
	if (has_colors() == FALSE) {
		endwin();
		exit(-1);
	}
	start_color();
	/* 初始化颜色对 */
	/*       颜色对           字色（表）   底色（背景） */
	init_pair(C_WHITE_BLUE,   COLOR_WHITE, COLOR_BLUE);      /* 蓝底白字 */
	init_pair(C_BLUE_WHITE,   COLOR_BLUE,  COLOR_WHITE);     /* 白底蓝字 */
	init_pair(C_WHITE_YELLOW, COLOR_WHITE, COLOR_YELLOW);    /* 黄底白字 */
	init_pair(C_BLACK_WHITE,  COLOR_BLACK, COLOR_WHITE);     /* 白底黑字 */
	init_pair(C_WHITE_BLACK,  COLOR_WHITE, COLOR_BLACK);     /* 黑底白字 */
	return;
}

/* ctools_menu_t_init
 * 初始化变量
 */
static void data_init(Data ** tmp)
{
	*tmp = (Data *)malloc(sizeof(Data));
	(*tmp) -> title = NULL;
	(*tmp) -> text  = NULL;
	(*tmp) -> focus = NULL;
	(*tmp) -> cfg   = 0;
	return;
}

/* 移动焦点选项 */
extern void get_focus(Data * data, int number)
{
	if (data->focus == NULL) {
		data->focus = data->text;
	}
	if (number <= 0) {
		while (data->focus->nextText != NULL) {
			data->focus = data->focus->nextText;
		}
		return;
	}
	if (data->focus->number > number) {
		data->focus = data->text;
	}
	while (data->focus->nextText != NULL && data->focus->number < number) {
		data->focus = data->focus->nextText;
	}
	return;
}

static void set_title(Data *data, char *title)
{
	char *t = NULL;
	if (data == NULL || title == NULL || *title == '\0') return;
	t = (char*)malloc(strlen(title));
	strcpy(t, title);
	data->title = t;
	return;
}

static void set_type(Data *data, char *type)
{
	if (data == NULL || type == NULL || *type == '\0') return;
#define S(t) (strcmp(t, type) == 0)
	if (S("normal"))
		data->cfg = 0;
	else if (S("main_only"))
		 data->cfg = 1;
	else if (S("help"))
		 data->cfg = 2;
	else if (S("setting"))
		 data->cfg = 3;
	else if (S("help_only"))
		 data->cfg = 4;
#undef S
	return;
}

/* 显示 */
static int Show(Data * data)
{
	int input = 1,		        /* 保存输入 */
	    focus_text = 1,             /* 保存焦点选项的数字 */
	    focus_describe = 0,         /* 描述的焦点 */
	    noShowText = 0,             /* 显示的内容与读取的偏差值（相当于屏幕上方隐藏的条目），用作实现界面滑动 */
	    noShowText2 = 0,	        /* 偏差值的备份 */
	    side = 0,                   /* 焦点位置 */
	    height = 0,                 /* 显示区域高度（偏差值） */
	    allDescribe = 0,            /* 保存所有的描述字符总行数（打印进度用） */
	    allChose = 0;               /* 保存所有的选择总数（打印进度用） */

	/* 倘若焦点指针不为空，
	 * 则获得焦点指针指向的文本数字编号
	 */
	if (data->focus != NULL)
		focus_text = data->focus->number;

	/* 移动焦点指针到最后一条文本 */
	get_focus(data, 0);
	allChose = data->focus->number;

	/* 配置选项判断 */
	if (data->cfg == 1) {
		/* 仅显示正常屏幕框架 */
		display.screen(data);
		if (data->text != NULL) {
			focus_text = 0;
			/* 打印选项 */
			display.text(data, focus_text, noShowText, allChose);
			get_focus(data, 1);
		}
		return 0;
	} else if (data->cfg == 4) {
		/* 仅显示帮助屏幕框架 */
		display.screen(data);
		if (data->text != NULL) {
			focus_text = 0;
			display.help(data, focus_text, noShowText, &allChose);
			get_focus(data, 1);
		}
		return 0;
	}

	if (data->cfg == 2) height = 8;
	else height = 10;

	while (input != 0x30 && input != 0x1B) {
		/* 显示屏幕框架 */
		display.screen(data);

		/* 打印选项 */
		if (data->cfg != 2)    /* 非帮助 */
			display.describe(data, focus_text, focus_describe, noShowText2,
					 &allDescribe);    /* 显示焦点选项的描述 */
		if (data->cfg == 0)    /* 默认 */
			display.text(data, focus_text, noShowText, allChose);
		else if (data->cfg == 2)    /* 帮助 */
			display.help(data, focus_text, noShowText, &allChose);
		else if (data->cfg == 3)    /* 设置 */
			display.setting(data, focus_text, noShowText, allChose);

		/* 菜单类型: 0.默认 1.仅显示主界面 2.显示帮助 3.显示设置 4.仅显示帮助，无输入处理 */
		/* 移动焦点指针到焦点文本 */
		get_focus(data, focus_text);

		if (data->cfg != 2) {    /* 非帮助 */
			if (side) {    /* 若焦点在描述内容上（side != 0） */
				/* 打印描述的按键提示 */
				attron(A_DIM);
				attron(COLOR_PAIR(C_BLACK_WHITE));
				move(7,         COLS / 4 * 3 - 4);printw("%sw k%s", ArrowUp, ArrowUp);
				move(LINES - 2, COLS / 4 * 3 - 4);printw("%ss j%s", ArrowDn, ArrowDn);
				move(LINES - 2, COLS - 7);printw("%02d/%02d", focus_describe, allDescribe);
				attroff(COLOR_PAIR(C_BLACK_WHITE));
				attroff(A_DIM);

				attron(A_BOLD);
				attron(COLOR_PAIR(C_WHITE_BLUE));
				move(5, COLS / 2);printw("%sTAB", ArrowLf);
				attroff(A_BOLD);
				attroff(COLOR_PAIR(C_WHITE_BLUE));
			} else {    /* 焦点在选项上 */
				/* 打印选项的按键提示 */
				attron(A_BOLD);
				attron(COLOR_PAIR(C_WHITE_BLUE));
				move(7,         COLS / 4 - 4);printw("%sw k%s", ArrowUp, ArrowUp);
				move(LINES - 2, COLS / 4 - 4);printw("%ss j%s", ArrowDn, ArrowDn);
				move(LINES - 2, COLS / 2 - 6);printw("%02d/%02d", focus_text, allChose);
				if (data->focus->describe != NULL) {    /* 如果有描述 */
					move(5, COLS / 2 - 5);printw("TAB%s", ArrowRi);
				}
				attroff(A_BOLD);
				attroff(COLOR_PAIR(C_WHITE_BLUE));
			}
		} else {    /* 帮助类型限定 */
			/* 打印描述的按键提示 */
			attron(COLOR_PAIR(C_BLACK_WHITE));
			move(5,         COLS / 2 - 4);printw("%sw k%s", ArrowUp, ArrowUp);
			move(LINES - 2, COLS / 2 - 4);printw("%ss j%s", ArrowDn, ArrowDn);
			move(LINES - 2, COLS - 7);printw("%02d/%02d", focus_text, allChose);
			attroff(COLOR_PAIR(C_BLACK_WHITE));
		}
		refresh();
		input = getch();
		/* 输入判断 */
		if (!side)
			input = Input(&input, &focus_text, &noShowText, allChose, height);
		else {
			input = Input(&input, &focus_describe, &noShowText2, allDescribe, height);
			if (input == '0') {
				input = '\t';
			}
		}
		switch (input) {
		case '\n':	/* 返回字符 */
			if (data->cfg == 3 && data->focus->cfg == 2
			    && data->focus->var != NULL) {
				if (!*(data->focus->var))
					*(data->focus->var) = 1;
				else
					*(data->focus->var) = 0;
			} else if (data->cfg == 0) {
				Clear;
				char output[10];	/* 仅用作字符输出 */
				sprintf(output, "%d", focus_text);
				return output[0];
			}
			break;
		case '+':
			if (data->cfg == 3 && data->focus->cfg == 1
			    && data->focus->var != NULL) {
				if ((*data->focus->var) + data->focus->foot >
				    data->focus->max)
					(*data->focus->var) = data->focus->min;
				else
					(*data->focus->var) += data->focus->foot;
			}
			break;
		case '-':
			if (data->cfg == 3 && data->focus->cfg == 1
			    && data->focus->var != NULL) {
				if ((*data->focus->var) - data->focus->foot <
				    data->focus->min)
					(*data->focus->var) = data->focus->max;
				else
					(*data->focus->var) -= data->focus->foot;
			}
			break;
		case '0':	/* 返回字符0 */
			Clear;
			if (data->cfg == 2)
				get_focus(data, 1);
			return '0';
			break;
		case 0:	/* 什么都不做 */
			break;
		case '\t':	/* 切换介绍与选项 */
			if (data->focus->describe != NULL) {
				if (!side) {    /* 在选项 */
					side = 1;
					focus_describe = 1;
				} else {    /* 在描述 */
					side = 0;
					focus_describe = 0;
					noShowText2 = 0;
				}
			}
			break;
		default:	/* 返回输入的字符 */
			if (data->cfg == 0)
				return input;
			break;
		}
	}
	return 0;
}

/* 处理输入 */
static int Input(int *input, int *focus, int *noShowText, int allChose, int height)
{
	switch (*input) {
	case 0x1B:
		if (ctools_kbhit() != 0) {
			getchar();
			*input = getchar();
			switch (*input) {
			case 'A':
			case 'D':
				goto LABEL_LAST;
				break;
			case 'B':
			case 'C':
				goto LABEL_NEXT;
				break;
			}
		} else {
			Clear;
			return '0';
		}
		break;
	case 'd':
	case 'D':
	case 'l':
	case 'L':
	case 's':
	case 'S':
	case 'j':
	case 'J':
	LABEL_NEXT:
		if (*focus < allChose) {
			(*focus)++;
		} else {
			*focus = 1;
			while (*focus - *noShowText < 1)    /* noShowText > 0 */
				(*noShowText)--;
			break;
		}
		while (*focus - *noShowText > LINES - height)
			(*noShowText)++;
		break;
	case 'g':
		*focus = 1;
		while (*focus - *noShowText < 1)
			(*noShowText)--;
		break;
	case 'G':
		while (*focus < allChose)
			(*focus)++;
		while (*focus - *noShowText > LINES - 10)
			(*noShowText)++;
		break;
	case 'a':
	case 'A':
	case 'h':
	case 'H':
	case 'w':
	case 'W':
	case 'k':
	case 'K':
	LABEL_LAST:
		if (*focus > 1) {
			(*focus)--;
		} else {
			while (*focus < allChose)
				(*focus)++;
			while (*focus - *noShowText > LINES - 10)
				(*noShowText)++;
		}
		while (*focus - *noShowText < 1)
			(*noShowText)--;
		break;
	case 'q':
	case 'Q':
	case '0':
		Clear;
		return '0';
		break;
	case ' ':
	case '\n':
	case '\r':
		return '\n';
		break;
	case '=':
		return '+';
		break;
	case '\t':
		return '\t';
		break;
	default:
		return *input;
		break;
	}
	Clear;
	return 0;
}

