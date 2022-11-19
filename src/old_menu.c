#include "include.h"

int ctools_old_menu(char *title, char *text[], int tl, int list) { /* 菜单程序 */
#ifdef __linux
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	int winSizeCol = size.ws_col;
#else
	int winSizeCol = 56;
#endif
	int input = 1, currentPage = 1, count = 1, allPages = (tl - 1) / (4 * list) + 1;

	if (list <= 0 || list > 2) {
		list = 2;
	}

	while (input != 0x30 && input != 0x1B) {
		Clear2;
#ifdef __linux
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		winSizeCol = size.ws_col;
#endif

#ifdef __linux
		printf("\033[5;%dH\033[0;2;34m--------------------------------------------------------", winSizeCol / 2 - 27);
		for (int i = 0; i < 7; i++) {
			gotoxy(i + 6, winSizeCol / 2 - 27);
			printf("\033[0;2;34m|\033[54C|");
		}
		printf("\033[13;%dH--------------------------------------------------------", winSizeCol / 2 - 27);
		printf("\033[0;2;32m\033[6;%dH↑\033[11;%dH↓\033[11;%dH\033[0;2;32m%d/%d\033[0;1;33m", winSizeCol / 2 - 1, winSizeCol / 2 - 1, winSizeCol / 2 + 25, currentPage,allPages);
		printf("\033[2;%dH\033[1;32m%s\033[0m", winSizeCol / 2 - (int)strlen(title) / 2, title);
#else
		gotoxy(6, winSizeCol / 2 - 1);
		printf("↑");
		gotoxy(11, winSizeCol / 2 - 1);
		printf("↓");
		gotoxy(11, winSizeCol / 2 + 24);
		printf("%2d/%2d", currentPage,allPages);
		gotoxy(2, winSizeCol / 2 - (int)strlen(title) / 2);
		printf("%s", title);
		gotoxy(5, winSizeCol / 2 - 27);
		printf("--------------------------------------------------------");
		for (int i = 0; i < 7; i++) {
			gotoxy(i + 6, winSizeCol / 2 - 27);
			printf("|");
			gotoxy(i + 6, winSizeCol / 2 + 27);
			printf("|");
		}
		gotoxy(13, winSizeCol / 2 - 27);
		printf("--------------------------------------------------------");
#endif
		for (int i = 1; i <= tl - list * 4 * (currentPage - 1) && i <= list * 4; i++) {
			if (i != count) {
#ifdef __linux
				printf("\033[33m\033[%d;%dH%s\033[0m", (i + 1) / list + 6 - list % 2, winSizeCol / 2 - 20 + ((i + 1) % list) * 32, text[i - 1 +  list * 4 * (currentPage - 1)]);
#else
				gotoxy((i + 1) / list + 6 - list % 2, winSizeCol / 2 - 20 + ((i + 1) % list) * 32);
				printf("%s", text[i - 1 +  list * 4 * (currentPage - 1)]);
#endif
			}
			else {
#ifdef __linux
				printf("\033[1;7;33m\033[%d;%dH%s\033[0m", (i + 1) / list + 6 - list % 2, winSizeCol / 2 - 20 + ((i + 1) % list) * 32, text[i - 1 +  list * 4 * (currentPage - 1)]);
#else
				gotoxy((i + 1) / list + 6 - list % 2, winSizeCol / 2 - 20 + ((i + 1) % list) * 32 - list);
				printf("> %s", text[i - 1 +  list * 4 * (currentPage - 1)]);
#endif
			}
			ctools_kbhitGetchar();
		}
		input = getch();
		if (input == 0x1B) {
			if (ctools_kbhit()) {
				getchar();
				input = getchar();
				if (input == 'A') {
					if (count > list) {
						count -= list;
					}
					else if (currentPage > 1){
						count += list * 3;
						currentPage--;
					}
				}
				else if (input == 'B') {
					if (count < list * 4 - (list + 1) % 2 && count + list * 4 * (currentPage - 1) <= tl - list) {
						count += list;
					}
					else if (currentPage < allPages){
						count -= list * 3;
						currentPage++;
						if (count + (4 * list) * (currentPage - 1) > tl) {
							count -= 1;
						}
					}
				}
				else if (input == 'C') {
					if (count < list * 4 && count + list * 4 * (currentPage - 1) < tl) {
						count++;
					}
					else if (currentPage < allPages){
						count = 1;
						currentPage++;
					}
				}
				else if (input == 'D') {
					if (count > 1) {
						count--;
					}
					else if (currentPage > 1){
						count = list * 4;
						currentPage--;
					}
				}
			}
			else {
				Clear2;
				return 0;
			}
		}
		else if (input == 'w' || input == 'W' || input == 'k' || input == 'K') {
			if (count > list) {
				count -= list;
			}
			else if (currentPage > 1){
				count += list * 3;
				currentPage--;
			}
		}
		else if (input == 's' || input == 'S' || input == 'j' || input == 'J') {
			if (count < list * 4 - (list + 1) % 2 && count + list * 4 * (currentPage - 1) <= tl - list) {
				count += list;
			}
			else if (currentPage < allPages){
				count -= list * 3;
				currentPage++;
				if (count + (4 * list) * (currentPage - 1) > tl) {
					count -= 1;
				}
			}
		}
		else if (input == 'd' || input == 'D' || input == 'l' || input == 'L') {
			if (count < list * 4 && count + list * 4 * (currentPage - 1) < tl) {
				count++;
			}
			else if (currentPage < allPages){
				count = 1;
				currentPage++;
			}
		}
		else if (input == 'a' || input == 'A' || input == 'h' || input == 'H') {
			if (count > 1) {
				count--;
			}
			else if (currentPage > 1){
				count = list * 4;
				currentPage--;
			}
		}
		else if (input == 'q' || input == 'Q') {
			Clear;
			return '0';
		}
		else if (input == ' ' || input == '\r' || input == '\n') {
			Clear2;
			char output[10];
			sprintf(output, "%d", count + 8 * (currentPage - 1));
			return output[0];
		}
		else {
			Clear2;
			return input;
		}
	}
	return 0;
}

#ifdef __linux
extern void ctools_old_menu2(char title[], short pages, short allPages)
{   /* 菜单#1带翻页提示的菜单 */
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	int winSizeCol = size.ws_col;

	printf("\033[0;2;32m\033[6;%dH↑\033[10;%dH↓\033[11;%dH\033[2;32m%d/%d\033[1;33m", winSizeCol / 2 - 1, winSizeCol / 2 - 1, winSizeCol / 2 + 25, pages,allPages);
	printf("\033[2;%dH\033[0;1;32m%s", winSizeCol / 2 - (int)strlen(title) / 2, title);
	printf("\033[5;%dH\033[34m--------------------------------------------------------", winSizeCol / 2 - 27);
	for (int i = 0; i < 7; i++) {
		gotoxy(i + 6, winSizeCol / 2 - 27);
		printf("|\033[54C|");
	}
	printf("\033[13;%dH--------------------------------------------------------", winSizeCol / 2 - 27);
	printf("\033[11;%dH\033[1;31m请选择:\033[0m", winSizeCol / 2 - 23);
	return;
}

extern void ctools_old_menu3(char title[])
{  /* 菜单#2纯洁 */
		struct winsize size;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		int winSizeCol = size.ws_col;

		printf("\033[2;%dH\033[0;1;32m%s", winSizeCol / 2 - (int)strlen(title) / 2, title);
		printf("\033[5;%dH\033[0;2;34m--------------------------------------------------------", winSizeCol / 2 - 27);
		for (int i = 0; i < 7; i++) {
			gotoxy(i + 6, winSizeCol / 2 - 27);
			printf("|\033[54C|");
		}
		printf("\033[13;%dH--------------------------------------------------------", winSizeCol / 2 - 27);
		printf("\033[11;%dH\033[1;31m按任意按键返回：\033[0m", winSizeCol / 2 - 23);
	return;
}
#endif

#ifdef _WIN32
extern void ctools_old_menu2(char title[], short pages, short allPages)
{
	int winSizeCol = 56;

	gotoxy(6, winSizeCol / 2 - 1);
	printf("↑");
	gotoxy(10, winSizeCol / 2 - 1);
	printf("↓");
	gotoxy(11, winSizeCol / 2 + 24);
	printf("%d/%d", pages,allPages);
	gotoxy(2, winSizeCol / 2 - (int)strlen(title) / 2);
	printf("%s", title);
	gotoxy(5, winSizeCol / 2 - 27);
	printf("--------------------------------------------------------");
	for (int i = 0; i < 7; i++) {
		gotoxy(i + 6, winSizeCol / 2 - 27);
		printf("|");
		gotoxy(i + 6, winSizeCol / 2 + 27);
		printf("|");
	}
	gotoxy(13, winSizeCol / 2 - 27);
	printf("--------------------------------------------------------");
	gotoxy(11, winSizeCol / 2 - 23);
	printf("请选择:");
}
extern void ctools_old_menu3(char title[])
{
	int winSizeCol = 56;

	gotoxy(2, winSizeCol / 2 - (int)strlen(title) / 2);
	printf("%s", title);
	gotoxy(5, winSizeCol / 2 - 27);
	printf("--------------------------------------------------------");
	for (int i = 0; i < 7; i++) {
		gotoxy(0, 0);
		gotoxy(i + 6, winSizeCol / 2 - 27);
		printf("|");
		gotoxy(i + 6, winSizeCol / 2 + 27);
		printf("|");
	}
	gotoxy(13, winSizeCol / 2 - 27);
	printf("--------------------------------------------------------");
	gotoxy(11, winSizeCol / 2 - 23);
	printf("按任意按键返回：");
}
#endif

