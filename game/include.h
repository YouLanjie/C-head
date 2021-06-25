/* File name: include.h */
/* This is a head file */

/* include head file */
#include <stdio.h>
#include <stdlib.h>
/* system() srand() rand() malloc() free() exit() */
#include <unistd.h>
/* pause() */
#include <sys/stat.h>
/* pass */
#include <string.h>
/* strcat() strcmp() strcpy() */

/* kbhit */
#include "game/kbhit.c"
int kbhit();
int input();
int kbhit_if();
int kbhit2();

/* menu */
#include "game/menu.c"
#ifndef Menu
	#define Menu printf("\033[0m\033[11;11H");
#endif
#ifndef Menu2
	#define Menu2 printf("\033[0m\033[11;19H");
#endif
void menu(char a[50]);
void menu2(char a[50]);

/* pid */
#include <sys/types.h>
/* pid_t */
#include <signal.h>
/* signal() */
#include <wait.h>

