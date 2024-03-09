#include "include.h"
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm-generic/ioctls.h>

/*
 * 判断有没有输入
 */
extern int kbhit()
{
	struct termios oldt, newt;
	int ch;
	int oldf;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if (ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}

/*
 * 不阻塞输入
 */
extern int kbhitGetchar()
{
	struct termios oldt, newt;
	int ch;
	int oldf;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if (ch != EOF) {
		ungetc(ch, stdin);
		return ch;
	}
	return 0;
}

/* 利用终端特性做的getch */
extern int _getch()
{
	struct termios tm, tm_old;
	int fd = 0, ch;

	if (tcgetattr(fd, &tm) < 0) {	//保存现在的终端设置
		return -1;
	}
	tm_old = tm;
	cfmakeraw(&tm);		//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
	if (tcsetattr(fd, TCSANOW, &tm) < 0) {	//设置上更改之后的设置
		return -1;
	}
	ch = getchar();
	if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {	//更改设置为最初的样子
		return -1;
	}
	return ch;
}

/* Get the size(x) of the window */
extern int get_winsize_col()
{
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	return size.ws_col;
}

/* Get the size(y) of the window */
extern int get_winsize_row()
{
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	return size.ws_row;
}

extern void ctools_ncurses_init()
{
	setlocale(LC_ALL, "zh_CN.UTF8");
	initscr();
	cbreak();		/* 取消行缓冲 */
	noecho();		/* 不回显 */
	curs_set(0);		/* 隐藏光标 */
	if (has_colors() == FALSE) {
		endwin();
		exit(-1);
	}
	start_color();
	/* 初始化颜色对 */
	/*       颜色对           字色（表）   底色（背景） */
	init_pair(C_WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);	/* 蓝底白字 */
	init_pair(C_BLUE_WHITE, COLOR_BLUE, COLOR_WHITE);	/* 白底蓝字 */
	init_pair(C_WHITE_YELLOW, COLOR_WHITE, COLOR_YELLOW);	/* 黄底白字 */
	init_pair(C_BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);	/* 白底黑字 */
	init_pair(C_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);	/* 黑底白字 */
	return;
}
