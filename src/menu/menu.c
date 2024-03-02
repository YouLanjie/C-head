#include "./menu.h"

/* 初始化ncurse，设置语言、颜色对 */
static void ncurses_init();
/* 初始化变量 */
static void data_init(Menu**);
/* 设置标题 */
static void set_title(Menu*, char*);
/* 设置类型 */
static void set_type(Menu*, char*);
/* 显示菜单 */
static int show(Menu*);
/* 获得输入 */
static int Input(int, int*, int*, int, int);

const struct ctools_menu ctools_menu_init()
{
	const struct ctools_menu menu = {
		.ncurses_init  = ncurses_init,
		.data_init     = data_init,
		.set_title     = set_title,
		.set_type      = set_type,
		.add_text      = add_text,
		.del_text      = del_text,
		.show          = show,
	};
	return menu;
}

/* ctools_menu_Init
 * 初始化ncurse
 * 设置语言、颜色对
 */
static void ncurses_init()
{
	ctools_ncurses_init();
	return;
}

/* ctools_menu_t_init
 * 初始化变量
 */
static void data_init(Menu ** tmp)
{
	*tmp = (Menu *)malloc(sizeof(Menu));
	(*tmp) -> title = NULL;
	(*tmp) -> text  = NULL;
	(*tmp) -> focus = NULL;
	(*tmp) -> type  = 0;
	return;
}

/* 移动焦点选项 */
extern void set_focus(Menu * menu, int id)
{
	if (menu->focus == NULL) {
		menu->focus = menu->text;
		if (menu->focus == NULL)
			return;
	}
	if (id <= 0) {
		while (menu->focus->next != NULL) {
			menu->focus = menu->focus->next;
		}
		return;
	}
	if (menu->focus->id > id) {
		menu->focus = menu->text;
	}
	while (menu->focus->next != NULL && menu->focus->id < id) {
		menu->focus = menu->focus->next;
	}
	return;
}

static void set_title(Menu *menu, char *title)
{
	char *t = NULL;
	if (menu == NULL || title == NULL || *title == '\0') return;
	t = (char*)malloc(strlen(title) + 1);
	strcpy(t, title);
	menu->title = t;
	return;
}

static void set_type(Menu *menu, char *type)
{
	if (menu == NULL || type == NULL || *type == '\0') return;
#define S(t) (strcmp(t, type) == 0)
	if (S("normal"))
		menu->type = 0;
	else if (S("main_only"))
		 menu->type = 1;
	else if (S("help"))
		 menu->type = 2;
	else if (S("setting"))
		 menu->type = 3;
	else if (S("help_only"))
		 menu->type = 4;
#undef S
	return;
}

/* 显示 */
static int show(Menu * menu)
{
	int input = 1,			/* 保存输入 */
	    focus_id = 1,		/* 保存焦点选项的数字 */
	    focus_id2 = 0,		/* 描述的焦点 */
	    noShowText = 0,		/* 显示的内容与读取的偏差值（相当于屏幕上方隐藏的条目），用作实现界面滑动 */
	    noShowText2 = 0,		/* 偏差值的备份 */
	    side = 0,			/* 焦点位置 */
	    height = 0,			/* 显示区域高度（偏差值） */
	    line_node = 0,		/* 保存所有的选择总数（打印进度用） */
	    line_desc = 0;		/* 保存所有的描述字符总行数（打印进度用） */

	/* 倘若焦点指针不为空，
	 * 则获得焦点指针指向的文本数字编号
	 */
	if (menu->focus != NULL)
		focus_id = menu->focus->id;

	/* 移动焦点指针到最后一条文本 */
	set_focus(menu, 0);
	line_node = menu->focus->id;

	/* 配置选项判断 */
	if (menu->type == 1 || menu->type == 4) {
		display.screen(menu);
		if (menu->text == NULL)
			return 0;
		focus_id = 0;
		/* 打印选项 */
		if (menu->type == 1)
			display.text(menu, focus_id, noShowText, line_node); /* 仅显示正常屏幕框架 */
		else
			display.help(menu, focus_id, noShowText, &line_node); /* 仅显示帮助屏幕框架 */
		set_focus(menu, 1);
	}

	if (menu->type == 2) height = 8;
	else height = 10;

	clear();
	while (input != 0x30 && input != 0x1B) {
		/* 显示屏幕框架 */
		display.screen(menu);

		/* 打印选项 */
		if (menu->type != 2)    /* 非帮助 */
			display.describe(menu, focus_id, focus_id2, noShowText2, &line_desc);    /* 显示焦点选项的描述 */
		if (menu->type == 0)    /* 默认 */
			display.text(menu, focus_id, noShowText, line_node);
		else if (menu->type == 2)    /* 帮助 */
			display.help(menu, focus_id, noShowText, &line_node);
		else if (menu->type == 3)    /* 设置 */
			display.setting(menu, focus_id, noShowText, line_node);

		/* 菜单类型: 0.默认 1.仅显示主界面 2.显示帮助 3.显示设置 4.仅显示帮助，无输入处理 */
		/* 移动焦点指针到焦点文本 */
		set_focus(menu, focus_id);

		if (menu->type != 2) {    /* 非帮助 */
			if (side) {    /* 若焦点在描述内容上（side != 0） */
				/* 打印描述的按键提示 */
				attron(A_DIM);
				attron(COLOR_PAIR(C_BLACK_WHITE));
				move(7,         COLS / 4 * 3 - 4);printw("%sw k%s", ArrowUp, ArrowUp);
				move(LINES - 2, COLS / 4 * 3 - 4);printw("%ss j%s", ArrowDn, ArrowDn);
				move(LINES - 2, COLS - 7);printw("%02d/%02d", focus_id2, line_desc);
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
				move(LINES - 2, COLS / 2 - 6);printw("%02d/%02d", focus_id, line_node);
				if (menu->focus->describe != NULL) {    /* 如果有描述 */
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
			move(LINES - 2, COLS - 7);printw("%02d/%02d", focus_id, line_node);
			attroff(COLOR_PAIR(C_BLACK_WHITE));
		}
		refresh();
		input = getch();
		/* 输入判断 */
		if (!side)
			input = Input(input, &focus_id, &noShowText, line_node, height);
		else {
			input = Input(input, &focus_id2, &noShowText2, line_desc, height);
			if (input == '0') {
				input = '\t';
			}
		}
		switch (input) {
		case '\n':	/* 返回字符 */
			if (menu->type == 3 && menu->focus->type == 2
			    && menu->focus->var != NULL) {
				if (!*(menu->focus->var))
					*(menu->focus->var) = 1;
				else
					*(menu->focus->var) = 0;
			} else if (menu->type == 0) {
				/* clear(); */
				/* char output[10];	/\* 仅用作字符输出 *\/ */
				/* sprintf(output, "%d", focus_text); */
				if (menu->focus->function != NULL)
					menu->focus->function();
				return focus_id;    /* 输出所选数字 */
			}
			break;
		case '+':
			if (menu->type == 3 && menu->focus->type == 1
			    && menu->focus->var != NULL) {
				if ((*menu->focus->var) + menu->focus->foot >
				    menu->focus->max)
					(*menu->focus->var) = menu->focus->min;
				else
					(*menu->focus->var) += menu->focus->foot;
			}
			break;
		case '-':
			if (menu->type == 3 && menu->focus->type == 1
			    && menu->focus->var != NULL) {
				if ((*menu->focus->var) - menu->focus->foot <
				    menu->focus->min)
					(*menu->focus->var) = menu->focus->max;
				else
					(*menu->focus->var) -= menu->focus->foot;
			}
			break;
		case 0:	/* 什么都不做 */
			break;
		case '\t':	/* 切换介绍与选项 */
			if (menu->focus->describe != NULL) {
				if (!side) {    /* 在选项 */
					side = 1;
					focus_id2 = 1;
				} else {    /* 在描述 */
					side = 0;
					focus_id2 = 0;
					noShowText2 = 0;
				}
			}
			break;
		default:	/* 返回输入的字符 */
			if (menu->type == 0) {
				if (input >= '0' && input <= '9') {
					input-=48;
				}
				return input;
			}
			break;
		}
	}
	return 0;
}

/* 处理输入 */
static int Input(int input, int *focus, int *noShowText, int allChose, int height)
{
	input = (input > 'a' && input < 'z' && input != 'g') ? input - 32 : input;
	switch (input) {
	case 0x1B:
		if (ctools_kbhit() != 0) {
			getchar();
			input = getchar();
			switch (input) {
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
			clear();
			return '0';
		}
		break;
	case 'D':
	case 'L':
	case 'S':
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
	case 'A':
	case 'H':
	case 'W':
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
	case 'Q':
	case '0':
		clear();
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
		return input;
		break;
	}
	clear();
	return 0;
}

