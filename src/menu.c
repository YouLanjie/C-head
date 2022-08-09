#include "menu.h"

// ====================================================================== NewMenu =======================================================================
// ======================================================================================================================================================

// 定义数据类型
// ======================================================================================================================================================

//  struct Text {
//          char        * text;         /* 条例内容 */
//          char        * describe;     /* 描述/帮助信息 */
//          void       (* function);    /* 调用的函数 */
//          int           number;       /* 编号 */
//          int           cfg;          /* 类型 */
//          struct Text * nextText;     /* 下一条例（链表） */
//  };                                  /* 条例结构体 */
//
//  typedef struct _menuData{
//          char        * title;                                                                      /* 标题 */
//          struct Text * text;                                                                       /* 条例链表头 */
//          struct Text * focus;                                                                      /* 选中的条例 */
//          int           cfg;                                                                        /* 菜单状态 */
//          void       (* addText)    (struct _menuData * data, ...);                                 /* 添加条例 */
//          void       (* addTextData)(struct _menuData * data, int type, char * format, ...);       /* 添加条例信息 */
//          void       (* getFocus)   (struct _menuData * data, int number);                          /* 更改焦点指针 */
//  }menuData;                                                                                        /* 菜单类/结构体 */

// 函数声明
// ======================================================================================================================================================

//  void menuDataInit(menuData * data);
//  void _menuAddText(menuData * data, ...);
//  void _menuAddTextData(menuData * data, int type, char * format, ...);
//  void _menuGetFocus(menuData * data, int number);
//  static void MenuScreen();

//
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

extern void menuDataInit(menuData * data) {    /* 初始化结构体 */
	data -> title       = NULL;
	data -> text        = NULL;
	data -> focus       = NULL;
	data -> cfg         = 0;
	data -> addText     = _menuAddText;
	data -> addTextData = _menuAddTextData;
	data -> getFocus    = _menuGetFocus;
	data -> menuShow    = _menu;
	return;
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

	for (int i = 2; pNew -> text != NULL; i++) {
		pTmp             = pNew;
		pNew -> nextText = malloc(sizeof(struct Text));
		pNew             = pNew -> nextText;
		pNew -> text     = va_arg(text, char *);
		pNew -> describe = NULL;
		pNew -> number   = i;
		pNew -> cfg      = 0;
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
			if (!type) {
				pNew -> describe = va_arg(text, char *);
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
	while (data -> focus -> number < number) {
		data -> focus = data -> focus -> nextText;
	}
	return;
}

// NewMenu
// ======================================================================================================================================================

#define winSizeCol size.ws_col    /* x轴 */
#define winSizeRol size.ws_row    /* y轴 */

/* 交换变量的值 */
#define Swap(a,b) \
	a = a^b;  \
	b=a^b;    \
	a=a^b;

static int _menu(menuData * data) {
#ifdef __linux
	/* 定义保存窗口大小的结构体变量 */
	struct winsize size;

	/* 获取最后一条文本的信息 */
	data -> getFocus(data, 0);

	int    input       = 1,                          /* 保存输入 */
	       focus       = 1,                          /* 保存焦点选项的数字 */
	       focus2      = 0,                          /* 描述的焦点 */
	       noShowText  = 0,                          /* 显示的内容与读取的偏差值（相当于屏幕上方隐藏的条目），用作实现界面滑动 */
	       noShowText2 = 0,                          /* 偏差值的备份 */
	       allChose    = data -> focus -> number,    /* 保存所有的选择总数 */
	       allDescribe = 0;                          /* 保存所有的描述字符行总数 */

	while (input != 0x30 && input != 0x1B) {
		//铺上底色
		printf("\033[0;44;37m");
		Clear
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		winSizeCol = size.ws_col;

		/* 显示屏幕框架 */
		_menuShowScreen(data);

		/* 打印选项 */
		_menuShowText(data, focus, noShowText, allChose);

		/* 显示焦点选项的描述 */
		_menuShowDescribe(data, focus, focus2, noShowText2, &allDescribe);

		if (focus2) {
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
		input = getch();
		switch (_menuInput(&input, &focus, &noShowText, allChose)) {
			case '\n':
				Clear2
				char output[10];    /* 仅用作字符输出 */
				sprintf(output, "%d", focus);
				return output[0];
				break;
			case '0':
				Clear
				return '0';
				break;
			case 0:
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
			default:
				return input;
				break;
		}
		if (focus2) {
			Swap(focus, focus2);
			Swap(allChose, allDescribe);
			Swap(noShowText, noShowText2);
		}
	}
#endif
	return 0;
}

// MenuScreen
// ======================================================================================================================================================

static void _menuShowScreen(menuData * data) {
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

	//铺上底色
	printf("\033[0;44;37m");
	Clear
	printf("\033[1;1H");
	printf(LineLU);
	for (int i = 2; i < winSizeCol; i++) {
		printf("\033[1;%dH%s\033[5;%dH%s\033[%d;%dH%s\033[7;%dH%s", i, LineH, i, LineH, winSizeRol, i, LineH, i, LineH);
	}
	printf("\033[6;%dH选项\033[6;%dH描述",winSizeCol / 4 - 2, winSizeCol /4 * 3 - 2);
	for (int i = 2; i < winSizeRol; i++) {
		printf("\033[%d;1H%s\033[%d;%dH%s", i, LineV, i, winSizeCol, LineV);
		if (i >= 6) {
			printf("\033[%d;%dH%s", i, winSizeCol / 2, LineV);
		}
	}
	for (int i = 2; i < winSizeCol; i++) {
	}
	printf("\033[%d;1H%s\033[%d;%dH%s\033[1;%dH%s\033[5;1H%s\033[5;%dH%s\033[5;%dH%s\033[%d;%dH%s\033[7;%dH%s\033[7;1H%s\033[7;%dH%s", winSizeRol, LineLD, winSizeRol, winSizeCol, LineRD, winSizeCol, LineRU, LineLC, winSizeCol, LineRC, winSizeCol / 2, LineUC, winSizeRol ,winSizeCol / 2, LineDC, winSizeCol / 2, LineCC, LineLC, winSizeCol, LineRC);
	//  printf("\033[0;2;44;32m\033[6;%dH↑\033[11;%dH↓\033[11;%dH\033[0;2;44;32m%d/%d\033[0;1;44;33m", winSizeCol / 2 - 1, winSizeCol / 2 - 1, winSizeCol / 2 + 25, currentPage,allPages);
	printf("\033[3;%dH\033[1;44;37m%s\033[0m", winSizeCol / 2 - (int)strlen(data -> title) / 2, data -> title);
	
	return;
}

static void _menuShowText(menuData * data, int focus, int noShowText, int allChose) {
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

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
		printf("\033[%d;4H%s\033[0m", i + 8 - noShowText, data -> focus -> text);
		kbhitGetchar();
	}
	return;
}

static void _menuShowDescribe(menuData * data, int focus, int focus2, int noShowText2, int * allDescribe) {
	struct winsize size;
	char * ch = NULL;    /* 用于打印描述字符时自动折行 */

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

	/* 打底色 */
	printf("\033[0;30;47m");
	for (int i = 8; i <= winSizeRol - 1; i++) {
		printf("\033[%d;%dH", i, winSizeCol / 2 + 2);
		for (int i2 = winSizeCol / 2 + 2; i2 <= winSizeCol - 2; i2++) {
			printf(" ");
		}
	}

	data -> getFocus(data, focus);
	if (data -> focus -> describe != NULL) {
		int stat = 1,    /* 仅作临时变量 */
		    i    = 0,
		    i2   = 9;

		printf("\033[9;%dH\033[0;30;47m", winSizeCol / 2 + 3);
		ch = data -> focus -> describe;
		while (*ch != '\0' && i2 - 8 - noShowText2 <= winSizeRol - 10) {
			if (i2 - 8 == focus2 && stat) {    /* 用作高亮选中项 */
				printf("\033[0;7;30;47m");
				stat = 0;
			}
			if (i2 - 9 >= noShowText2) {       /* 限定跳过不显示的字符 */
				printf("%c", *ch);
				i++;
			}
			else {                             /* 移动光标到初始位置 */
				printf("\033[%d%dH", i2 - noShowText2, winSizeCol / 2 + 3);
			}

			/* 字符指针下移 */
			ch++;

			/* 格式判断：自动换行、换行 */
			if ((i >= winSizeCol / 2 - 4 && *ch != '\0') || *ch == '\n' || *ch == '\r') {
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
				kbhitGetchar();

				/* 字符指针下移 */
				if (*ch == '\n' || *ch == '\r') {
					ch++;
				}
			}
		}
		ch = data -> focus -> describe;
		*allDescribe = 1;
		i = 0;
		while (*ch != '\0') {
			i++;
			if ((i >= winSizeCol / 2 - 4 && *ch != '\0') || *ch == '\n' || *ch == '\r') {
				(*allDescribe)++;
				i = 0;
				ch++;
			}
			ch++;
		}
	}
	return;
}

static int _menuInput(int * input, int * focus, int * noShowText, int allChose) {
	struct winsize size;

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

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
						while (*focus - *noShowText < 1) {
							(*noShowText)--;
						}
						break;
					case 'B':
					case 'C':
						if (*focus < allChose) {
							(*focus)++;
						}
						while (*focus - *noShowText > winSizeRol - 10) {
							(*noShowText)++;
						}
						break;
				}
			}
			else {
				Clear2
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
		case '\t':
			return '\t';
			break;
	}
	Clear2
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

