#include <stdio.h>
#define Menu printf("\033[11;11H");

void menu(char a[50]) {
	printf("\033[2;25H\033[1;32m%s",a);
	printf("\033[5;1H\033[34m--------------------------------------------------------");
	printf("\033[6;1H\033[34m|\033[6;56H|");
	printf("\033[7;1H|\033[7;56H|");
	printf("\033[8;1H|\033[8;56H|");
	printf("\033[9;1H|\033[9;56H|");
	printf("\033[10;1H|\033[10;56H|");
	printf("\033[11;1H|\033[11;56H|");
	printf("\033[12;1H|\033[12;56H|");
	printf("\033[13;1H--------------------------------------------------------\033[0m\033[11;11H");
	printf("\033[11;2H\033[1;31m  请选择:\033[0m\033[11;11H");
	return;
}
