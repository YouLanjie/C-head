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
	struct termios new_attr, old_attr;
	/* 设置无缓冲输入 */
	if (tcgetattr(STDIN_FILENO, &old_attr) < 0) return -1;
	new_attr = old_attr;
	new_attr.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &new_attr) < 0) return -1;
	/* 设置无阻塞 */
	int old_fl = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, old_fl | O_NONBLOCK);
	int ch = getchar();
	if (tcsetattr(STDIN_FILENO, TCSANOW, &old_attr) < 0) return -1;
	fcntl(STDIN_FILENO, F_SETFL, old_fl);
	/* 将输入内容“塞”回到输入流中 */
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
	struct termios new_attr, old_attr;
	/* 设置无缓冲输入 */
	if (tcgetattr(STDIN_FILENO, &old_attr) < 0) return -1;
	new_attr = old_attr;
	new_attr.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &new_attr) < 0) return -1;
	/* 设置无阻塞 */
	int old_fl = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, old_fl | O_NONBLOCK);
	int ch = getchar();
	if (tcsetattr(STDIN_FILENO, TCSANOW, &old_attr) < 0) return -1;
	fcntl(STDIN_FILENO, F_SETFL, old_fl);
	/* 将输入内容“塞”回到输入流中 */
	if (ch != EOF) {
		ungetc(ch, stdin);
		return ch;
	}
	return 0;
}

extern int _getch(void)
{
	struct termios new_attr, old_attr;
	// 保存现在的终端设置
	if (tcgetattr(STDIN_FILENO, &old_attr) < 0) return -1;
	new_attr = old_attr;
	// 更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
	// cfmakeraw(&new_attr);
	// 取消行缓冲和回显
	new_attr.c_lflag &= ~(ICANON | ECHO);
	//设置上更改之后的设置
	if (tcsetattr(STDIN_FILENO, TCSANOW, &new_attr) < 0) return -1;
	int ch = getchar();
	// 更改设置为最初的样子
	if (tcsetattr(STDIN_FILENO, TCSANOW, &old_attr) < 0) return -1;
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

/* 读取文件 */
extern char *_fread(FILE *fp)
{
	long size = 0;
	char *str = NULL;

	if (!fp)
		return NULL;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	str = malloc(sizeof(char) * (size + 1));
	if (str == NULL) {
		perror("The file is too big");
		return NULL;
	}
	fseek(fp, 0L, SEEK_SET);
	fread(str, 1, size, fp);

	str[size] = '\0';

	return str;
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
