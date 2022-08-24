#include "menu.h"
#include "include.h"

// ====================================================================== NewMenu =======================================================================
// ======================================================================================================================================================


// 操作函数
// ======================================================================================================================================================

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

extern menuData menuDataInit() {    /* 初始化结构体 */
	menuData data = {
		NULL,
		NULL,
		NULL,
		0,
		_menuAddText,
		_menuAddTextData,
		_menuGetFocus,
		_menu
	};
	return data;
}

static void _menuAddText(menuData * data, ...) {
	struct Text * pNew, * pTmp;
	va_list text;

	va_start(text, data);
	if (data -> text != NULL) {
		free(data -> text);
	}

	pNew = data -> text = malloc(sizeof(struct Text));
	pNew -> text     = va_arg(text, char *);
	pNew -> number   = 1;
	pNew -> describe = NULL;
	pNew -> cfg      = 0;
	pNew -> foot     = 1;
	//  pNew -> max      = 2147483647;    /* 整型的最大值 */
	//  pNew -> min      = -2147483648;    /* 整型的最小值 */
	pNew -> max      = 10000000;
	pNew -> min      = -10000000;
	pNew -> var      = NULL;
	pNew -> function = NULL;

	for (int i = 2; pNew -> text != NULL; i++) {
		pTmp             = pNew;
		pNew -> nextText = malloc(sizeof(struct Text));
		pNew             = pNew -> nextText;
		pNew -> text     = va_arg(text, char *);
		pNew -> describe = NULL;
		pNew -> number   = i;
		pNew -> cfg      = 0;
		pNew -> foot     = 1;
		//  pNew -> max      = 2147483647;    /* 整型的最大值 */
		//  pNew -> min      = -2147483648;    /* 整型的最小值 */
		pNew -> max      = 10000000;
		pNew -> min      = -10000000;
		pNew -> var      = NULL;
		pNew -> function = NULL;
	}
	free(pNew);
	pTmp -> nextText = NULL;
	va_end(text);
	return;
}

static void _menuAddTextData(menuData * data, int type, char * format, ...) {    /* type:0 -> describe, 1 -> function */
	struct Text * pNew;
	va_list text;

	va_start(text, format);
	pNew = data -> text;
	while (*format != '\0') {
		if (*format == '%' && *(format + 1) == 's') {
			if (type == 0) {
				pNew -> describe = va_arg(text, char *);
			}
			else if (type == 1) {
				pNew -> cfg = va_arg(text, int);
			}
			else if (type == 2) {
				pNew -> var = va_arg(text, int *);
			}
			else if (type == 3) {
				pNew -> foot = va_arg(text, int);
			}
			else if (type == 4) {
				pNew -> max = va_arg(text, int);
			}
			else if (type == 5) {
				pNew -> min = va_arg(text, int);
			}
			else {
				pNew -> function = va_arg(text, void *);
			}
			pNew = pNew -> nextText;
			format++;
		}
		else if (*format == '\n' || *format == 'n' || *format == 'N') {
			pNew = pNew -> nextText;
		}
		format++;
	}
	va_end(text);
	return;
}

static void _menuGetFocus(menuData * data, int number) {
	if (data -> focus == NULL) {
		data -> focus = data -> text;
	}
	if (number <= 0) {
		while (data -> focus -> nextText != NULL) {
			data -> focus = data -> focus -> nextText;
		}
		return;
	}
	if (data -> focus -> number > number) {
		data -> focus = data -> text;
	}
	while (data -> focus -> nextText != NULL && data -> focus -> number < number) {
		data -> focus = data -> focus -> nextText;
	}
	return;
}

// NewMenu
// ======================================================================================================================================================

#ifdef __linux
#define winSizeCol size.ws_col    /* x轴 */
#define winSizeRol size.ws_row    /* y轴 */
#endif

#ifdef __linux
/* 定义保存窗口大小的结构体变量 */
static struct winsize size;
#else
static int winSizeCol = 80;
static int winSizeRol = 24;
#endif

/* 交换变量的值 */
#define Swap(a,b) \
	a = a^b;  \
	b=a^b;    \
	a=a^b;

static int _menu(menuData * data) {
	int    input       = 1,    /* 保存输入 */
	       focus       = 1,    /* 保存焦点选项的数字 */
	       focus2      = 0,    /* 描述的焦点 */
	       noShowText  = 0,    /* 显示的内容与读取的偏差值（相当于屏幕上方隐藏的条目），用作实现界面滑动 */
	       noShowText2 = 0,    /* 偏差值的备份 */
	       allDescribe = 0,    /* 保存所有的描述字符行总数 */
	       allChose;           /* 保存所有的选择总数 */

	/* 倘若焦点指针不为空，
	 * 则获得焦点指针指向的文本数字编号
	 */
	if (data -> focus != NULL) {
		focus = data -> focus -> number;
	}

	/* 移动焦点指针到最后一条文本 */
	data -> getFocus(data, 0);
	allChose = data -> focus -> number;

	/* 配置选项判断 */
	if (data -> cfg == 1) {
		/* 仅显示屏幕框架 */
		_menuShowScreen(data);
		if (data -> text != NULL) {
			focus = 0;
			/* 打印选项 */
			_menuShowText(data, focus, noShowText, allChose);
			data -> getFocus(data, 1);
		}
		return 0;
	}
	else if (data -> cfg >= 4) {
		/* 仅显示屏幕框架 */
		_menuShowScreen(data);
		if (data -> text != NULL) {
			focus = 0;
			_menuShowHelp(data, focus, noShowText, &allChose);
			data -> getFocus(data, 1);
		}
		return 0;
	}

	while (input != 0x30 && input != 0x1B) {
#ifdef __linux
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		if (winSizeCol == 0 || winSizeRol == 0) {
			winSizeCol = 80;
			winSizeRol = 24;
		}
#endif

		/* 显示屏幕框架 */
		_menuShowScreen(data);

		if (data -> cfg != 2) {
			/* 显示焦点选项的描述 */
			_menuShowDescribe(data, focus, focus2, noShowText2, &allDescribe);
		}
		if (data -> cfg == 0) {
			/* 打印选项 */
			_menuShowText(data, focus, noShowText, allChose);
		}
		else if (data -> cfg == 2) {
			_menuShowHelp(data, focus, noShowText, &allChose);
		}
		else if (data -> cfg == 3) {
			_menuShowSitting(data, focus, noShowText, allChose);
		}

		/* 移动焦点指针到焦点文本 */
		data -> getFocus(data, focus);

		if (data -> cfg != 2) {
			/* 交换变量的值 */
			if (focus2) {    /* 若焦点在描述内容上（focus2 != 0） */
				Swap(focus, focus2);
				Swap(allChose, allDescribe);
				Swap(noShowText, noShowText2);

				/* 打印描述的按键提示 */
				printf("\033[0;1;47;30m\033[8;%dH%sw k%s\033[%d;%dH%ss j%s\033[0m", winSizeCol / 4 * 3 - 3, ArrowOn, ArrowOn, winSizeRol - 1, winSizeCol / 4 * 3 - 3, ArrowDn, ArrowDn);
				printf("\033[0;1;47;30m\033[%d;%dH%02d/%02d\033[0m", winSizeRol - 1, winSizeCol - 6, focus, allChose);
				printf("\033[0;1;44;37m\033[6;%dH%sTAB\033[0m", winSizeCol / 2 + 1, ArrowLf);
			}
			else {
				/* 打印选项的按键提示 */
				printf("\033[0;1;44;37m\033[8;%dH%sw k%s\033[%d;%dH%ss j%s\033[0m", winSizeCol / 4 - 3, ArrowOn, ArrowOn, winSizeRol - 1, winSizeCol / 4 - 3, ArrowDn, ArrowDn);
				printf("\033[0;1;44;37m\033[%d;%dH%02d/%02d\033[0m", winSizeRol - 1, winSizeCol / 2 - 5, focus, allChose);
				if (data -> focus -> describe != NULL) {
					printf("\033[0;1;44;37m\033[6;%dHTAB%s\033[0m", winSizeCol / 2 - 4, ArrowRi);
				}
			}
		}
		else {
			/* 打印帮助的按键提示 */
			printf("\033[0;1;47;30m\033[6;%dH%sw k%s\033[%d;%dH%ss j%s\033[0m", winSizeCol / 2 - 3, ArrowOn, ArrowOn, winSizeRol - 1, winSizeCol / 2 - 3, ArrowDn, ArrowDn);
			printf("\033[0;1;47;30m\033[%d;%dH%02d/%02d\033[0m", winSizeRol - 1, winSizeCol - 6, focus, allChose);
			winSizeRol = winSizeRol + 2;
		}
		kbhitGetchar();
		input = getch();
		/* 输入判断 */
		switch (_menuInput(&input, &focus, &noShowText, allChose)) {
			case '\n':    /* 返回字符 */
				if (data -> cfg == 3 && data -> focus -> cfg == 2 && data -> focus -> var != NULL) {
					if (!*(data -> focus -> var)) {
						*(data -> focus -> var) = 1;
					}
					else {
						*(data -> focus -> var) = 0;
					}
				}
				else if (data -> cfg == 0){
					Clear2
					char output[10];    /* 仅用作字符输出 */
					sprintf(output, "%d", focus);
					return output[0];
				}
				break;
			case '+':
				if (data -> cfg == 3 && data -> focus -> cfg == 1 && data -> focus -> var != NULL) {
					if ((*data -> focus -> var) + data -> focus -> foot > data -> focus -> max) {
						(*data -> focus -> var) = data -> focus -> min;
					}
					else {
						(*data -> focus -> var) += data -> focus -> foot;
					}
				}
				break;
			case '-':
				if (data -> cfg == 3 && data -> focus -> cfg == 1 && data -> focus -> var != NULL) {
					if ((*data -> focus -> var) - data -> focus -> foot < data -> focus -> min) {
						(*data -> focus -> var) = data -> focus -> max;
					}
					else {
						(*data -> focus -> var) -= data -> focus -> foot;
					}
				}
				break;
			case '0':    /* 返回字符0 */
				Clear
				if (data -> cfg == 2) {
					data -> getFocus(data, 1);
				}
				return '0';
				break;
			case 0:    /* 什么都不做 */
				break;
			case '\t':    /* 切换介绍与选项 */
				if (data -> focus -> describe != NULL) {
					if (!focus2) {
						focus2 = focus;
						focus = 1;
						noShowText2 = noShowText;
						noShowText = 0;
						allDescribe = allChose;
					}
					else {
						Swap(focus, focus2);
						Swap(allChose, allDescribe);
						Swap(noShowText, noShowText2);
						focus2 = 0;
						noShowText2 = 0;
					}
				}
				break;
			default:    /* 返回输入的字符 */
				if (data -> cfg == 0) {
					return input;
				}
				break;
		}
		if (data -> cfg != 2) {
			/* 交换变量的值 */
			if (focus2) {
				Swap(focus, focus2);
				Swap(allChose, allDescribe);
				Swap(noShowText, noShowText2);
			}
		}
		else {
			winSizeRol = winSizeRol - 2;
		}
	}
	return 0;
}

// MenuScreen
// ======================================================================================================================================================

static void _menuShowScreen(menuData * data) {
#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (winSizeCol == 0 || winSizeRol == 0) {
		winSizeCol = 80;
		winSizeRol = 24;
	}
#endif

	if (data == NULL) {
		return;
	}
	//铺上底色
	printf("\033[0;44;37m");
	Clear
	printf("\033[1;1H");
	printf(LineLU);
	for (int i = 2; i < winSizeCol; i++) {
		printf("\033[1;%dH%s", i, LineH);                 /* 第一横线 */
		printf("\033[5;%dH%s", i, LineH);                 /* 第二横线 */
		printf("\033[%d;%dH%s", winSizeRol, i, LineH);    /* 第三横线 */
		if (data -> cfg == 0 || data -> cfg == 3) {
			printf("\033[7;%dH%s", i, LineH);         /* 第四横线 */
		}
	}
	if (data -> cfg == 0 || data -> cfg == 3) {
		printf("\033[6;%dH选项",winSizeCol / 4 - 2);
		printf("\033[6;%dH描述", winSizeCol /4 * 3 - 2);
	}
	for (int i = 2; i < winSizeRol; i++) {
		printf("\033[%d;1H%s", i, LineV);                 /* 左垂线 */
		printf("\033[%d;%dH%s", i, winSizeCol, LineV);    /* 右垂线 */
		if (i >= 6 && (data -> cfg == 0 || data -> cfg == 3)) {
			printf("\033[%d;%dH%s", i, winSizeCol / 2, LineV);
		}
	}
	for (int i = 2; i < winSizeCol; i++) {
	}
	printf("\033[%d;1H%s", winSizeRol, LineLD);                             /*   左下角   */
	printf("\033[%d;%dH%s", winSizeRol, winSizeCol, LineRD);                /*   右下角   */
	printf("\033[1;%dH%s", winSizeCol, LineRU);                             /*   右上角   */
	printf("\033[5;1H%s", LineLC);                                          /* 左第一连接 */
	printf("\033[5;%dH%s", winSizeCol, LineRC);                             /* 右第一连接 */
	if (data -> cfg == 0 || data -> cfg == 3) {
		printf("\033[5;%dH%s", winSizeCol / 2, LineUC);	                /* 中第一连接!1&2 */
		printf("\033[%d;%dH%s", winSizeRol ,winSizeCol / 2, LineDC);    /* 中第二连接!1&2 */
		printf("\033[7;1H%s", LineLC);	                                /* 左第二连接!1&2 */
		printf("\033[7;%dH%s", winSizeCol, LineRC);                     /* 右第二连接!1&2 */
		printf("\033[7;%dH%s", winSizeCol / 2, LineCC);                 /*  中线交界!1&2  */
	}
	if (data -> title != NULL) {
		printf("\033[3;%dH\033[1;44;37m%s\033[0m", winSizeCol / 2 - (int)strlen(data -> title) / 2, data -> title);
	}
	
	return;
}

static void _menuShowText(menuData * data, int focus, int noShowText, int allChose) {
#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (winSizeCol == 0 || winSizeRol == 0) {
		winSizeCol = 80;
		winSizeRol = 24;
	}
#endif

	if (data == NULL) {
		return;
	}
	printf("\033[0;30;47m");
	for (int i = 1; data -> text != NULL && data -> focus != NULL && i - noShowText <= allChose && i - noShowText <= winSizeRol - 10; i++) {
		if (i <= noShowText) {
			continue;
		}
		data -> getFocus(data, i);
		if (i != focus) {
			printf("\033[0;44;37m");
		}
		else {
			printf("\033[1;7;47;33m");
		}
		printf("\033[%d;4H", i + 8 - noShowText);
		for (int i = 0; i <= winSizeCol / 2 - 7; i++) {
			printf(" ");
		}
		printf("\033[%d;4H%s\033[0m", i + 8 - noShowText, data -> focus -> text);
		kbhitGetchar();
	}
	return;
}

static void _menuShowDescribe(menuData * data, int focus, int focus2, int noShowText2, int * allDescribe) {
	char * ch = NULL;    /* 用于打印描述字符时自动折行 */

#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (winSizeCol == 0 || winSizeRol == 0) {
		winSizeCol = 80;
		winSizeRol = 24;
	}
#endif

	if (data == NULL) {
		return;
	}
	/* 打底色 */
	printf("\033[0;30;47m");
	for (int i = 8; i <= winSizeRol - 1; i++) {
		printf("\033[%d;%dH", i, winSizeCol / 2 + 2);
		for (int i2 = winSizeCol / 2 + 2; i2 <= winSizeCol - 2; i2++) {
			printf(" ");
		}
	}

	data -> getFocus(data, focus);
	if (data -> focus -> describe != NULL) {    /* 倘若描述不为空 */
		int stat   = 0,    /* 仅作临时变量 */
		    i      = 0,    /* 行内字符数/宽度 */
		    i2     = 9,    /* 行数 */
		    zh     = 0,    /* 行内中文字数 */
		    zhStat = 1;    /* 是否在打印中文 */

		printf("\033[9;%dH\033[0;30;47m", winSizeCol / 2 + 3);    /* 设置颜色 */
		ch = data -> focus -> describe;
		while (*ch != '\0') {    /* 按字符显示 */
			/* 格式判断：自动换行、换行 */
			if (((i >= winSizeCol / 2 - 4 && *ch != '\0' && zhStat == 1) || (i >= winSizeCol / 2 - 6 && *ch != '\0' && zhStat == 2 )) || *ch == '\n' || *ch == '\r') {
				/* 换行时恢复原本的颜色 */
				if (i2 - 8 == focus2) {
					printf("\033[0;30;47m");
				}

				/* 行数增加 */
				i2++;

				/* 移动光标 */
				printf("\033[%d;%dH", i2 - noShowText2 , winSizeCol / 2 + 3);

				/* 字符清零 */
				i = 0;
				zh = 0;
				kbhitGetchar();

				/* 字符指针下移 */
				if (*ch == '\n' || *ch == '\r') {
					ch++;
				}
			}

			if (i2 - 8 == focus2 && !stat) {    /* 用作高亮选中项 */
				printf("\033[0;7;30;47m");
				stat = 1;
			}

			if (*ch == '%' && *(ch + 1) == 'z') {    /* 开启或关闭中文打印 */
				zhStat = 3 - zhStat;
				ch += 2;
			}

			if (*ch == '\0') {
				break;
			}

			if (i2 - 9 >= noShowText2 && i2 - 8 - noShowText2 <= winSizeRol - 10) {    /* 如果超过不显示的行数 */
				printf("%c", *ch);
				if (zhStat == 2) {
					ch++;
					printf("%c", *ch);
					ch++;
					printf("%c", *ch);
					zh++;
					i += 2;
				}
				else {
					i++;
				}
			}
			else {                             /* 移动光标到初始位置 */
				printf("\033[%d%dH", i2 - noShowText2, winSizeCol / 2 + 3);
				if (zhStat == 2) {
					ch += 2;
					zh++;
					i += 2;
				}
				else {
					i++;
				}
			}

			/* 字符指针下移 */
			ch++;

		}
		*allDescribe = i2 - 8;
	}
	return;
}

static void _menuShowHelp(menuData * data, int focus, int noShowText, int * allHelp) {
	char * ch       = NULL;    /* 用于打印描述字符时自动折行 */
	int    i        = 0,       /* 行内字符总量 */
	       i2       = 7,       /* 多少行 */
	       i3       = 1,       /* 多少条文本 */
	       zh       = 0,       /* 行内中文字数 */
	       zhStat   = 1;       /* 是否在打印中文 */

#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (winSizeCol == 0 || winSizeRol == 0) {
		winSizeCol = 80;
		winSizeRol = 24;
	}
#endif

	if (data == NULL || data -> text == NULL) {
		return;
	}
	/* 打底色 */
	printf("\033[0;30;47m");
	for (int i = 6; i <= winSizeRol - 1; i++) {
		printf("\033[%d;3H", i);
		for (int i2 = 2; i2 <= winSizeCol - 3; i2++) {
			printf(" ");
		}
	}
	kbhitGetchar();

	printf("\033[7;%dH\033[0;30;47m", 4);
	do {
		data -> getFocus(data, i3);
		if (data -> focus == NULL) {
			return;
		}
		ch = data -> focus -> text;
		while (*ch != '\0') {
			/* 格式判断：自动换行、换行 */
			if (((i >= winSizeCol - 4 && *(ch + 1) != '\0' && zhStat == 1) || (i >= winSizeCol - 6 && *ch != '\0' && zhStat == 2)) || *ch == '\n' || *ch == '\r') {
				/* 换行时恢复原本的颜色 */
				if (i2 - 6 == focus) {
					printf("\033[0;30;47m");
				}

				/* 行数增加 */
				i2++;

				/* 移动光标 */
				printf("\033[%d;%dH", i2 - noShowText , 4);

				/* 字符清零 */
				i = 0;
				zh = 0;
				kbhitGetchar();

				/* 字符指针下移 */
				if (*ch == '\n' || *ch == '\r') {
					ch++;
				}
			}

			/* 用作高亮选中项 */
			if (i2 - 6 == focus) {
				printf("\033[0;7;30;47m");
			}

			if (*ch == '%' && *(ch + 1) == 'z') {    /* 开启或关闭中文打印 */
				zhStat = 3 - zhStat;
				ch += 2;
				continue;
			}

			if (*ch == '\0') {
				break;
			}

			if (i2 - 6 - noShowText <= winSizeRol - 8 && i2 - 6 > noShowText) {
				printf("%c", *ch);
				if (zhStat == 2) {
					ch++;
					printf("%c", *ch);
					ch++;
					printf("%c", *ch);
					zh++;
					i += 2;
				}
				else {
					i++;
				}
			}
			else {                             /* 移动光标到初始位置 */
				printf("\033[%d;4H", i2 - noShowText);
				if (zhStat == 2) {
					ch += 2;
					zh++;
					i += 2;
				}
				else {
					i++;
				}
			}

			/* 字符指针下移 */
			ch++;
		}
		i3++;
		/* 换行时恢复原本的颜色 */
		if (i2 - 6 == focus) {
			printf("\033[0;30;47m");
		}

		/* 行数增加 */
		i2++;

		/* 移动光标 */
		printf("\033[%d;%dH", i2 - noShowText , 4);

		/* 字符清零 */
		i = 0;
		kbhitGetchar();
	}while (data -> focus -> nextText != NULL);
	*allHelp = i2 - 7;
	return;
}

static void _menuShowSitting(menuData * data, int focus, int noShowText, int allChose) {
#ifdef __linux
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (winSizeCol == 0 || winSizeRol == 0) {
		winSizeCol = 80;
		winSizeRol = 24;
	}
#endif

	if (data == NULL || data -> text == NULL) {
		return;
	}
	printf("\033[0;30;47m");
	for (int i = 1; i - noShowText <= allChose && i - noShowText <= winSizeRol - 10; i++) {
		if (i <= noShowText) {
			continue;
		}
		data -> getFocus(data, i);
		if (i != focus) {
			printf("\033[0;44;37m");
		}
		else {
			printf("\033[1;7;47;33m");
		}
		printf("\033[%d;4H", i + 8 - noShowText);
		for (int i = 0; i <= winSizeCol / 2 - 7; i++) {
			printf(" ");
		}
		printf("\033[%d;4H%s", i + 8 - noShowText, data -> focus -> text);
		if (data -> focus -> cfg == 1 && data -> focus -> var != NULL) {
			printf("\033[%d;%dH[\033[4m%7d]\033[0m", i + 8 - noShowText, winSizeCol / 2 - 10, *(data -> focus -> var));
		}
		else if (data -> focus -> cfg == 2 && data -> focus -> var != NULL) {
			if (*(data -> focus -> var)  == 0) {
				printf("\033[%d;%dH( )\033[0m", i + 8 - noShowText, winSizeCol / 2 - 4);
			}
			else {
				printf("\033[%d;%dH(*)\033[0m", i + 8 - noShowText, winSizeCol / 2 - 4);
			}
		}
		kbhitGetchar();
	}
	return;
}

static int _menuInput(int * input, int * focus, int * noShowText, int allChose) {
	switch (*input) {
		case 0x1B:
			if (kbhit() != 0) {
				getchar();
				*input = getch();
				switch (*input) {
					case 'A':
					case 'D':
						if (*focus > 1) {
							(*focus)--;
						}
						else {
							while (*focus < allChose) {
								(*focus)++;
							}
							while (*focus - *noShowText > winSizeRol - 10) {
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
						}
						else {
							*focus = 1;
							while (*focus - *noShowText < 1) {
								(*noShowText)--;
							}
							break;
						}
						while (*focus - *noShowText > winSizeRol - 10) {
							(*noShowText)++;
						}
						break;
				}
			}
			else {
				Clear
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
			}
			else {
				*focus = 1;
				while (*focus - *noShowText < 1) {
					(*noShowText)--;
				}
				break;
			}
			while (*focus - *noShowText > winSizeRol - 10) {
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
			while (*focus - *noShowText > winSizeRol - 10) {
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
			}
			else {
				while (*focus < allChose) {
					(*focus)++;
				}
				while (*focus - *noShowText > winSizeRol - 10) {
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
			Clear
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
	Clear
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

