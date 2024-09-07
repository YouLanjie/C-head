#include "include.h"
#include <fcntl.h>

#ifdef __linux__
#include <termios.h>
#include <sys/ioctl.h>
#include <asm-generic/ioctls.h>
#endif

#ifdef __linux__
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
#endif

/*
 * 不阻塞输入
 */
extern int kbhitGetchar()
{
#ifdef __linux__
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
#endif

#ifdef _WIN32
	if (kbhit() != 0) return getch();
	return 0;
#endif
}

extern int _getch(void)
{
#ifdef __linux__
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
#endif

#ifdef _WIN32
	return getch();
#endif
}

/* Get the size(x) of the window */
extern int get_winsize_col()
{
#ifdef __linux__
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	return size.ws_col;
#endif

#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#endif
}

/* Get the size(y) of the window */
extern int get_winsize_row()
{
#ifdef __linux__
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	return size.ws_row;
#endif

#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#endif
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

