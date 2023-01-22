#include "menu.h"

// ============================== NewMenu ===============================
// ======================================================================

// 数据操作函数
// ======================================================================

// 定义宏
#define LineH "─"
#define LineV "│"
#define LineLU "┌"
#define LineLD "└"
#define LineRU "┐"
#define LineRD "┘"
#define LineLC "├"
#define LineRC "┤"
#define LineCC "┼"
#define LineUC "┬"
#define LineDC "┴"
#define LineCLU "╭"
#define LineCLD "╰"
#define LineCRU "╮"
#define LineCRD "╯"
#define ArrowOn "↑"
#define ArrowDn "↓"
#define ArrowLf "←"
#define ArrowRi "→"

/* ctools_menu_Init
 * 初始化ncurse
 * 设置语言、颜色对
 */
extern void ctools_menu_Init()
{
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	if (has_colors() == FALSE) {
		endwin();
		exit(-1);
	}
	start_color();
	/* 初始化颜色对 */
	init_pair(1, COLOR_WHITE, COLOR_BLUE);      /* 蓝底白字 */
	init_pair(2, COLOR_BLUE, COLOR_WHITE);      /* 白底蓝字 */
	init_pair(3, COLOR_WHITE, COLOR_YELLOW);    /* 黄底白字 */
	init_pair(4, COLOR_BLACK, COLOR_WHITE);     /* 白底黑字 */
	init_pair(5, COLOR_WHITE, COLOR_BLACK);     /* 黑底白字 */
	return;
}

/* ctools_menu_t_init
 * 初始化变量
 */
extern void ctools_menu_t_init(ctools_menu_t ** tmp)
{
	if (*tmp != NULL) {
		free(*tmp);
		tmp = NULL;
	}
	*tmp = (ctools_menu_t *)malloc(sizeof(ctools_menu_t));
	(*tmp) -> title = NULL;
	(*tmp) -> text  = NULL;
	(*tmp) -> focus = NULL;
	(*tmp) -> cfg   = 0;
	return;
}

extern void ctools_menu_AddText(ctools_menu_t * data, ...)
{
	struct Text *pNew, *pTmp;
	va_list text;

	va_start(text, data);
	if (data->text != NULL) {
		free(data->text);
	}

	pNew = data->text = malloc(sizeof(struct Text));
	pNew->text = va_arg(text, char *);
	pNew->number = 1;
	pNew->describe = NULL;
	pNew->cfg = 0;
	pNew->foot = 1;
	//  pNew -> max      = 2147483647;    /* 整型的最大值 */
	//  pNew -> min      = -2147483648;    /* 整型的最小值 */
	pNew->max = 10000000;
	pNew->min = -10000000;
	pNew->var = NULL;
	pNew->function = NULL;

	for (int i = 2; pNew->text != NULL; i++) {
		pTmp = pNew;
		pNew->nextText = malloc(sizeof(struct Text));
		pNew = pNew->nextText;
		pNew->text = va_arg(text, char *);
		pNew->describe = NULL;
		pNew->number = i;
		pNew->cfg = 0;
		pNew->foot = 1;
		//  pNew -> max      = 2147483647;    /* 整型的最大值 */
		//  pNew -> min      = -2147483648;    /* 整型的最小值 */
		pNew->max = 10000000;
		pNew->min = -10000000;
		pNew->var = NULL;
		pNew->function = NULL;
	}
	free(pNew);
	pTmp->nextText = NULL;
	va_end(text);
	return;
}

extern void ctools_menu_AddTextData(ctools_menu_t * data, int type, char * format, ...)
{				/* type:0 -> describe, 1 -> function */
	struct Text *pNew;
	va_list text;

	va_start(text, format);
	pNew = data->text;
	while (*format != '\0') {
		if (*format == '%' && *(format + 1) == 's') {
			if (type == 0) {
				pNew->describe = va_arg(text, char *);
			} else if (type == 1) {
				pNew->cfg = va_arg(text, int);
			} else if (type == 2) {
				pNew->var = va_arg(text, int *);
			} else if (type == 3) {
				pNew->foot = va_arg(text, int);
			} else if (type == 4) {
				pNew->max = va_arg(text, int);
			} else if (type == 5) {
				pNew->min = va_arg(text, int);
			} else {
				pNew->function = va_arg(text, void *);
			}
			pNew = pNew->nextText;
			format++;
		} else if (*format == '\n' || *format == 'n' || *format == 'N') {
			pNew = pNew->nextText;
		}
		format++;
	}
	va_end(text);
	return;
}

extern void ctools_menu_GetFocus(ctools_menu_t * data, int number)
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

// 数据处理部分
// ======================================================================

#ifdef __linux
#define COLS size.ws_col	/* x轴 */
#define LINES size.ws_row	/* y轴 */
#endif

#ifdef __linux
/* 定义保存窗口大小的结构体变量 */
static struct winsize size;
#else
static int COLS = 80;
static int LINES = 24;
#endif

/* 交换变量的值 */
#define Swap(a,b) \
	a = a^b;  \
	b=a^b;    \
	a=a^b;

extern int  ctools_menu_Show(ctools_menu_t * data)
{
	int input = 1,		/* 保存输入 */
	    focus = 1,		/* 保存焦点选项的数字 */
	    focus2 = 0,		/* 描述的焦点 */
	    noShowText = 0,	/* 显示的内容与读取的偏差值（相当于屏幕上方隐藏的条目），用作实现界面滑动 */
	    noShowText2 = 0,	/* 偏差值的备份 */
	    allDescribe = 0,	/* 保存所有的描述字符行总数 */
	    allChose;		/* 保存所有的选择总数 */

	/* 倘若焦点指针不为空，
	 * 则获得焦点指针指向的文本数字编号
	 */
	if (data->focus != NULL) {
		focus = data->focus->number;
	}

	/* 移动焦点指针到最后一条文本 */
	ctools_menu_GetFocus(data, 0);
	allChose = data->focus->number;

	/* 配置选项判断 */
	if (data->cfg == 1) {
		/* 仅显示屏幕框架 */
		_ctools_menu_ShowScreen(data);
		if (data->text != NULL) {
			focus = 0;
			/* 打印选项 */
			_ctools_menu_ShowText(data, focus, noShowText, allChose);
			ctools_menu_GetFocus(data, 1);
		}
		return 0;
	} else if (data->cfg >= 4) {
		/* 仅显示屏幕框架 */
		_ctools_menu_ShowScreen(data);
		if (data->text != NULL) {
			focus = 0;
			_ctools_menu_ShowHelp(data, focus, noShowText, &allChose);
			ctools_menu_GetFocus(data, 1);
		}
		return 0;
	}

	while (input != 0x30 && input != 0x1B) {
#ifdef __linux
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		if (COLS == 0 || LINES == 0) {
			COLS = 80;
			LINES = 24;
		}
#endif

		/* 显示屏幕框架 */
		_ctools_menu_ShowScreen(data);

		if (data->cfg != 2) {
			/* 显示焦点选项的描述 */
			_ctools_menu_ShowDescribe(data, focus, focus2, noShowText2,
					  &allDescribe);
		}
		if (data->cfg == 0) {
			/* 打印选项 */
			_ctools_menu_ShowText(data, focus, noShowText, allChose);
		} else if (data->cfg == 2) {
			_ctools_menu_ShowHelp(data, focus, noShowText, &allChose);
		} else if (data->cfg == 3) {
			_ctools_menu_ShowSitting(data, focus, noShowText, allChose);
		}

		/* 移动焦点指针到焦点文本 */
		ctools_menu_GetFocus(data, focus);

		if (data->cfg != 2) {
			/* 交换变量的值 */
			if (focus2) {	/* 若焦点在描述内容上（focus2 != 0） */
				Swap(focus, focus2);
				Swap(allChose, allDescribe);
				Swap(noShowText, noShowText2);

				/* 打印描述的按键提示 */
				attron(A_DIM);
				attron(COLOR_PAIR(4));
				mvaddstr(7, COLS / 4 * 3 - 4, ArrowOn);
				printw("w k%s", ArrowOn);
				mvaddstr(LINES - 2, COLS / 4 * 3 - 4, ArrowDn);
				printw("s j%s", ArrowDn);

				move(LINES - 2, COLS - 7);
				printw("%02d/%02d", focus, allChose);

				attroff(COLOR_PAIR(4));
				attroff(A_DIM);
				attron(A_BOLD);
				attron(COLOR_PAIR(1));

				mvaddstr(5, COLS / 2, ArrowLf);
				printw("TAB");

				attroff(A_BOLD);
				attroff(COLOR_PAIR(1));
			} else {
				/* 打印选项的按键提示 */
				attron(A_BOLD);
				attron(COLOR_PAIR(1));
				mvaddstr(7, COLS / 4 - 4, ArrowOn);
				printw("w k%s", ArrowOn);
				mvaddstr(LINES - 2, COLS / 4 - 4, ArrowDn);
				printw("s j%s", ArrowDn);

				move(LINES - 2, COLS / 2 - 6);
				printw("%02d/%02d", focus, allChose);

				if (data->focus->describe != NULL) {
					move(5, COLS / 2 - 5);
					printw("TAB%s", ArrowRi);
				}
				attroff(A_BOLD);
				attroff(COLOR_PAIR(1));
			}
		} else {
			/* 打印描述的按键提示 */
			attron(COLOR_PAIR(4));
			mvaddstr(5, COLS / 2 - 4, ArrowOn);
			printw("w k%s", ArrowOn);
			mvaddstr(LINES - 2, COLS / 2 - 4, ArrowDn);
			printw("s j%s", ArrowDn);

			move(LINES - 2, COLS - 7);
			printw("%02d/%02d", focus, allChose);

			attroff(COLOR_PAIR(4));
			LINES = LINES + 2;
		}
		refresh();
		input = getch();
		/* 输入判断 */
		switch (_ctools_menu_Input(&input, &focus, &noShowText, allChose)) {
		case '\n':	/* 返回字符 */
			if (data->cfg == 3 && data->focus->cfg == 2
			    && data->focus->var != NULL) {
				if (!*(data->focus->var)) {
					*(data->focus->var) = 1;
				} else {
					*(data->focus->var) = 0;
				}
			} else if (data->cfg == 0) {
				Clear;
				char output[10];	/* 仅用作字符输出 */
				sprintf(output, "%d", focus);
				return output[0];
			}
			break;
		case '+':
			if (data->cfg == 3 && data->focus->cfg == 1
			    && data->focus->var != NULL) {
				if ((*data->focus->var) + data->focus->foot >
				    data->focus->max) {
					(*data->focus->var) = data->focus->min;
				} else {
					(*data->focus->var) +=
					    data->focus->foot;
				}
			}
			break;
		case '-':
			if (data->cfg == 3 && data->focus->cfg == 1
			    && data->focus->var != NULL) {
				if ((*data->focus->var) - data->focus->foot <
				    data->focus->min) {
					(*data->focus->var) = data->focus->max;
				} else {
					(*data->focus->var) -=
					    data->focus->foot;
				}
			}
			break;
		case '0':	/* 返回字符0 */
			Clear;
			if (data->cfg == 2) {
				ctools_menu_GetFocus(data, 1);
			}
			return '0';
			break;
		case 0:	/* 什么都不做 */
			break;
		case '\t':	/* 切换介绍与选项 */
			if (data->focus->describe != NULL) {
				if (!focus2) {
					focus2 = focus;
					focus = 1;
					noShowText2 = noShowText;
					noShowText = 0;
					allDescribe = allChose;
				} else {
					Swap(focus, focus2);
					Swap(allChose, allDescribe);
					Swap(noShowText, noShowText2);
					focus2 = 0;
					noShowText2 = 0;
				}
			}
			break;
		default:	/* 返回输入的字符 */
			if (data->cfg == 0) {
				return input;
			}
			break;
		}
		if (data->cfg != 2) {
			/* 交换变量的值 */
			if (focus2) {
				Swap(focus, focus2);
				Swap(allChose, allDescribe);
				Swap(noShowText, noShowText2);
			}
		} else {
			LINES = LINES - 2;
		}
	}
	return 0;
}

// 显示函数
// ======================================================================

static void _ctools_menu_ShowScreen(ctools_menu_t * data)
{
#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (COLS == 0 || LINES == 0) {
		COLS = 80;
		LINES = 24;
	}
#endif

	if (data == NULL) {
		return;
	}
	//铺上底色
	attron(COLOR_PAIR(1));
	for (int i = 0; i < LINES; i++) {
		for (int i2 = 0; i2 < COLS; i2++) {
			mvaddch(i, i2, ' ');
		}
	}
	move(0, 0);
	printw(LineLU);
	for (int i = 1; i < COLS; i++) {
		mvaddstr(0, i, LineH);	/* 第一横线 */
		mvaddstr(4, i, LineH);	/* 第二横线 */
		mvaddstr(LINES - 1, i, LineH);	/* 第三横线 */
		if (data->cfg == 0 || data->cfg == 3) {
			mvaddstr(6, i, LineH);	/* 第四横线 */
		}
	}
	if (data->cfg == 0 || data->cfg == 3) {
		mvaddstr(5, COLS / 4 - 3, "选项");
		mvaddstr(5, COLS / 4 * 3 - 3, "描述");
	}
	for (int i = 1; i < LINES; i++) {
		mvaddstr(i, 0, LineV);	        /* 左垂线 */
		mvaddstr(i, COLS - 1, LineV);	/* 右垂线 */
		if (i >= 5 && (data->cfg == 0 || data->cfg == 3)) {
			mvaddstr(i, COLS / 2 - 1, LineV);	/* 中垂线 */
		}
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
	attroff(COLOR_PAIR(1));

	return;
}

static void _ctools_menu_ShowText(ctools_menu_t * data, int focus, int noShowText, int allChose)
{
#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (COLS == 0 || LINES == 0) {
		COLS = 80;
		LINES = 24;
	}
#endif

	if (data == NULL) {
		return;
	}
	for (int i = 1;
	     data->text != NULL && data->focus != NULL
	     && i - noShowText <= allChose && i - noShowText <= LINES - 10;
	     i++) {
		if (i <= noShowText) {
			continue;
		}
		ctools_menu_GetFocus(data, i);
		if (i != focus) {
			attron(COLOR_PAIR(1));
			attroff(A_BOLD);
		} else {
			attron(COLOR_PAIR(3));
			attron(A_BOLD);
		}
		move(i + 7 - noShowText, 3);
		for (int i = 0; i <= COLS / 2 - 6; i++) {
			printw(" ");
		}
		mvaddstr(i + 7 - noShowText, 3, data->focus->text);
		attroff(COLOR_PAIR(2));
		attroff(COLOR_PAIR(3));
	}
	return;
}

static void _ctools_menu_ShowDescribe(ctools_menu_t * data, int focus, int focus2, int noShowText2, int *allDescribe)
{
	char *ch = NULL;	/* 用于打印描述字符时自动折行 */

#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (COLS == 0 || LINES == 0) {
		COLS = 80;
		LINES = 24;
	}
#endif

	if (data == NULL) {
		return;
	}
	/* 打底色 */
	attron(COLOR_PAIR(4));
	for (int i = 8; i <= LINES - 1; i++) {
		move(i - 1, COLS / 2 + 1);
		for (int i2 = COLS / 2 + 2; i2 <= COLS - 2; i2++) {
			printw(" ");
		}
	}
	attroff(COLOR_PAIR(4));
	attron(COLOR_PAIR(4));

	ctools_menu_GetFocus(data, focus);
	if (data->focus->describe != NULL) {	/* 倘若描述不为空 */
		int stat = 0,	/* 仅作临时变量 */
		    i = 0,	/* 行内字符数/宽度 */
		    i2 = 9,	/* 行数 */
		    zhStat = 1;	/* 是否在打印中文 */

		move(8, COLS / 2 + 2);	/* 设置颜色 */
		ch = data->focus->describe;
		while (*ch != '\0') {	/* 按字符显示 */
			/* 格式判断：自动换行、换行 */
			if (((i >= COLS / 2 - 4 && *ch != '\0' && zhStat == 1)
			     ||
			     (i >= COLS / 2 - 6 && *ch != '\0' && zhStat == 2))
			    || *ch == '\n'
			    || *ch == '\r') {
				/* 换行时恢复原本的颜色 */
				if (i2 - 8 == focus2) {
					attroff(COLOR_PAIR(5));
					attron(COLOR_PAIR(4));
				}

				/* 行数增加 */
				i2++;

				/* 移动光标 */
				move(i2 - noShowText2 - 1, COLS / 2 + 2);

				/* 字符清零 */
				i = 0;
				ctools_kbhitGetchar();

				/* 字符指针下移 */
				if (*ch == '\n' || *ch == '\r') {
					ch++;
				}
			}

			if (i2 - 8 == focus2 && !stat) {	/* 用作高亮选中项 */
				attroff(COLOR_PAIR(4));
				attron(COLOR_PAIR(5));
				stat = 1;
			}

			if (*ch & 0x80) {    /* 判断中文字符 */
				zhStat = 2;
			} else {
				zhStat = 1;
			}

			if (*ch == '\0') {
				break;
			}

			if (i2 - 9 >= noShowText2 && i2 - 8 - noShowText2 <= LINES - 10) {	/* 如果超过不显示的行数 */
				printw("%c", *ch);
				if (zhStat == 2) {
					ch++;
					printw("%c", *ch);
					ch++;
					printw("%c", *ch);
					i += 2;
				} else {
					i++;
				}
			} else {	/* 移动光标到初始位置 */
				move(i2 - noShowText2 - 1, COLS / 2 + 3);
				if (zhStat == 2) {
					ch += 2;
					i += 2;
				} else {
					i++;
				}
			}
			/* 字符指针下移 */
			ch++;

		}
		*allDescribe = i2 - 8;
	}
	attroff(COLOR_PAIR(4));
	attroff(COLOR_PAIR(5));
	return;
}

static void _ctools_menu_ShowHelp(ctools_menu_t * data, int focus, int noShowText, int *allHelp)
{
	char *ch = NULL;	/* 用于打印描述字符时自动折行 */
	int i = 0,		/* 行内字符总量 */
	    i2 = 7,		/* 多少行 */
	    i3 = 1,		/* 多少条文本 */
	    zhStat = 1,		/* 是否在打印中文 */
	    stat = 1;

#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (COLS == 0 || LINES == 0) {
		COLS = 80;
		LINES = 24;
	}
#endif

	if (data == NULL || data->text == NULL) {
		return;
	}
	/* 打底色 */
	attron(COLOR_PAIR(4));
	for (int i = 6; i <= LINES - 1; i++) {
		move(i - 1, 2);
		for (int i2 = 2; i2 <= COLS - 3; i2++) {
			printw(" ");
		}
	}
	move(6, 3);
	do {
		ctools_menu_GetFocus(data, i3);
		if (data->focus == NULL) {
			return;
		}
		ch = data->focus->text;
		while (*ch != '\0') {
			/* 格式判断：自动换行、换行 */
			if (((i >= COLS - 4 && *(ch + 1) != '\0' && zhStat == 1)
			     || (i >= COLS - 6 && *ch != '\0' && zhStat == 2))
			    || *ch == '\n' || *ch == '\r') {
				/* 换行时恢复原本的颜色 */
				if (i2 - 6 == focus) {
					attroff(COLOR_PAIR(5));
					attron(COLOR_PAIR(4));
				}

				/* 行数增加 */
				i2++;

				/* 移动光标 */
				move(i2 - noShowText - 1, 3);

				/* 字符清零 */
				i = 0;

				/* 字符指针下移 */
				if (*ch == '\n' || *ch == '\r') {
					ch++;
				}
				continue;
			}

			/* 用作高亮选中项 */
			if (i2 - 6 == focus && stat == 1) {
				attroff(COLOR_PAIR(4));
				attron(COLOR_PAIR(5));
				stat = 0;
			}

			if (*ch & 0x80) {    /* 判断中文字符 */
				zhStat = 2;
			} else {
				zhStat = 1;
			}

			if (*ch == '\0') {
				break;
			}

			if (i2 - 6 - noShowText <= LINES - 8
			    && i2 - 6 > noShowText) {
				printw("%c", *ch);
				if (zhStat == 2) {
					ch++;
					printw("%c", *ch);
					ch++;
					printw("%c", *ch);
					i += 2;
				} else {
					i++;
				}
			} else {	/* 移动光标到初始位置 */
				move(i2 - noShowText - 1, 3);
				if (zhStat == 2) {
					ch += 2;
					i += 2;
				} else {
					i++;
				}
			}

			/* 字符指针下移 */
			ch++;
		}
		i3++;
		/* 换行时恢复原本的颜色 */
		if (i2 - 6 == focus) {
			attroff(COLOR_PAIR(5));
			attron(COLOR_PAIR(4));
		}

		/* 行数增加 */
		i2++;

		/* 移动光标 */
		move(i2 - noShowText - 1, 3);

		/* 字符清零 */
		i = 0;
	} while (data->focus->nextText != NULL);
	*allHelp = i2 - 7;
	attroff(COLOR_PAIR(4));
	attroff(COLOR_PAIR(5));
	return;
}

static void _ctools_menu_ShowSitting(ctools_menu_t * data, int focus, int noShowText, int allChose)
{
#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (COLS == 0 || LINES == 0) {
		COLS = 80;
		LINES = 24;
	}
#endif

	if (data == NULL || data->text == NULL) {
		return;
	}
	for (int i = 1;
	     i - noShowText <= allChose && i - noShowText <= LINES - 10; i++) {
		if (i <= noShowText) {
			continue;
		}
		ctools_menu_GetFocus(data, i);
		if (i != focus) {
			attroff(A_BOLD);
			attroff(COLOR_PAIR(3));
			attron(COLOR_PAIR(1));
		} else {
			attroff(COLOR_PAIR(1));
			attron(COLOR_PAIR(3));
			attron(A_BOLD);
		}
		move(i + 7 - noShowText, 3);
		for (int i = 0; i <= COLS / 2 - 6; i++) {
			printw(" ");
		}
		mvaddstr(i + 7 - noShowText, 3, data->focus->text);
		if (data->focus->cfg == 1 && data->focus->var != NULL) {
			mvaddch(i + 7 - noShowText, COLS / 2 - 11, '[');
			attron(A_UNDERLINE);
			printw("%7d", *(data->focus->var));
			attroff(A_UNDERLINE);
			printw("]");
		} else if (data->focus->cfg == 2 && data->focus->var != NULL) {
			if (*(data->focus->var) == 0) {
				mvaddstr(i + 7 - noShowText, COLS / 2 - 5,
					 "( )");
			} else {
				mvaddstr(i + 7 - noShowText, COLS / 2 - 5,
					 "(*)");
			}
		}
	}
	attroff(COLOR_PAIR(1));
	attroff(COLOR_PAIR(3));
	return;
}

static int _ctools_menu_Input(int *input, int *focus, int *noShowText, int allChose)
{
	switch (*input) {
	case 0x1B:
		if (ctools_kbhit() != 0) {
			getchar();
			*input = getchar();
			switch (*input) {
			case 'A':
			case 'D':
				if (*focus > 1) {
					(*focus)--;
				} else {
					while (*focus < allChose) {
						(*focus)++;
					}
					while (*focus - *noShowText >
					       LINES - 10) {
						(*noShowText)++;
					}
				}
				while (*focus - *noShowText < 1) {
					(*noShowText)--;
				}
				break;
			case 'B':
			case 'C':
				if (*focus < allChose) {
					(*focus)++;
				} else {
					*focus = 1;
					while (*focus - *noShowText < 1) {
						(*noShowText)--;
					}
					break;
				}
				while (*focus - *noShowText > LINES - 10) {
					(*noShowText)++;
				}
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
		if (*focus < allChose) {
			(*focus)++;
		} else {
			*focus = 1;
			while (*focus - *noShowText < 1) {
				(*noShowText)--;
			}
			break;
		}
		while (*focus - *noShowText > LINES - 10) {
			(*noShowText)++;
		}
		break;
	case 'g':
		*focus = 1;
		while (*focus - *noShowText < 1) {
			(*noShowText)--;
		}
		break;
	case 'G':
		while (*focus < allChose) {
			(*focus)++;
		}
		while (*focus - *noShowText > LINES - 10) {
			(*noShowText)++;
		}
		break;
	case 'a':
	case 'A':
	case 'h':
	case 'H':
	case 'w':
	case 'W':
	case 'k':
	case 'K':
		if (*focus > 1) {
			(*focus)--;
		} else {
			while (*focus < allChose) {
				(*focus)++;
			}
			while (*focus - *noShowText > LINES - 10) {
				(*noShowText)++;
			}
		}
		while (*focus - *noShowText < 1) {
			(*noShowText)--;
		}
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

#undef Swap

#undef winSizeCol
#undef winSizeRol

#undef LineH
#undef LineV
#undef LineLU
#undef LineLD
#undef LineRU
#undef LineRD
#undef LineLC
#undef LineRC
#undef LineCC
#undef LineUC
#undef LineDC
#undef LineCLU
#undef LineCLD
#undef LineCRU
#undef LineCRD
#undef ArrowOn
#undef ArrowDn
