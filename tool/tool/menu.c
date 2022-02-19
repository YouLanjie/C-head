#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux
	#include <sys/ioctl.h>
	#include <asm-generic/ioctls.h>
	#ifndef fontColorSet
		#define fontColorSet(a,b) printf("\033[%d;%dm",a, b)
	#endif
	#ifndef gotoxy
		#define gotoxy(x,y) printf("\033[%d;%dH",x, y)
	#endif
#endif
#ifdef _WIN32
	#include <windows.h>
	void gotoxy(int x,int y)//光标定位函数
	{
		COORD p;//定义结构体变量p
		HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);//获取当前函数句柄
		p.X=x;p.Y=y;//将光标的目标移动位置传递给结构体
		SetConsoleCursorPosition(handle,p);//移动光标
	}
#endif

/* 菜单#1 */
void Menu(char title[], short pages, short allPages) {
	#ifdef __linux
		struct winsize size;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		int winSizeCol = size.ws_col;

		printf("\033[2;32m\033[6;%dH↑\033[10;%dH↓\033[11;%dH\033[2;32m%d/%d\033[1;33m", winSizeCol / 2 - 1, winSizeCol / 2 - 1, winSizeCol / 2 + 25, pages,allPages);
		printf("\033[2;%dH\033[1;32m%s", winSizeCol / 2 - (int)strlen(title) / 2, title);
		printf("\033[5;%dH\033[34m--------------------------------------------------------\033[34m", winSizeCol / 2 - 27);
		for (int i = 0; i < 6; i++) {
			gotoxy(i + 6, winSizeCol / 2 - 27);
			printf("|\033[54C|");
		}
		printf("\033[13;%dH--------------------------------------------------------\033[0m", winSizeCol / 2 - 27);
		printf("\033[11;%dH\033[1;31m请选择:\033[0m", winSizeCol / 2 - 23);
	#endif
	#ifdef _WIN32
		int winSizeCol = 56;

		gotoxy(6, winSizeCol / 2 - 1); printf("↑"); gotoxy(10, winSizeCol / 2 - 1); printf("↓"); gotoxy(11, winSizeCol / 2 + 25); printf("%d/%d\033[1;33m", pages,allPages);
		gotoxy(2, winSizeCol / 2 - (int)strlen(title) / 2); printf("%s", title);
		gotoxy(5, winSizeCol / 2 - 27); printf("\033[34m--------------------------------------------------------");
		for (int i = 0; i < 6; i++) {
			gotoxy(i + 6, winSizeCol / 2 - 27);
			printf("|\033[54C|");
		}
		gotoxy(13, winSizeCol / 2 - 27); printf("--------------------------------------------------------");
		gotoxy(11, winSizeCol / 2 - 23); printf("请选择:");
	#endif
	return;
}

/* 菜单#2 */
void Menu2(char title[]) {
	#ifdef __linux
		struct winsize size;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		int winSizeCol = size.ws_col;

		printf("\033[2;%dH\033[1;32m%s", winSizeCol / 2 - (int)strlen(title) / 2, title);
		printf("\033[5;%dH\033[34m--------------------------------------------------------", winSizeCol / 2 - 27);
		for (int i = 0; i < 7; i++) {
			gotoxy(i + 6, winSizeCol / 2 - 27);
			printf("|\033[54C|");
		}
		printf("\033[13;%dH--------------------------------------------------------\033[0m", winSizeCol / 2 - 27);
		printf("\033[11;%dH\033[1;31m按任意按键返回：\033[0m", winSizeCol / 2 - 23);
	#endif
	#ifdef _WIN32
		int winSizeCol = 56;

		gotoxy(2, winSizeCol / 2 - (int)strlen(title) / 2); printf("%s", title);
		gotoxy(5, winSizeCol / 2 - 27); printf("\033[34m--------------------------------------------------------");
		for (int i = 0; i < 6; i++) {
			gotoxy(i + 6, winSizeCol / 2 - 27);
			printf("|\033[54C|");
		}
		gotoxy(13, winSizeCol / 2 - 27); printf("--------------------------------------------------------");
		gotoxy(11, winSizeCol / 2 - 23); printf("按任意按键返回：");
	#endif
	return;
}

