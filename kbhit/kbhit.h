#include <stdio.h>
#include <termios.h>

#define Kbhit while (b != 0) if (kbhit()) b = 0;

static struct termios initial_settings, new_settings;
static int peek_character = -1;

extern void init_keyboard();
extern void close_keyboard();
extern int kbhit();
extern int readch(); 

extern void init_keyboard()
{
	tcgetattr(0,&initial_settings);
	new_settings = initial_settings;
	new_settings.c_lflag |= ICANON;
	new_settings.c_lflag |= ECHO;
	new_settings.c_lflag |= ISIG;
	new_settings.c_cc[VMIN] = 1;
	new_settings.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_settings);
}
 
extern void close_keyboard()
{
	tcsetattr(0, TCSANOW, &initial_settings);
}
 
extern int kbhit()
{
	unsigned char ch;
	int nread;
 
	if (peek_character != -1) return 1;
	new_settings.c_cc[VMIN]=0;
	tcsetattr(0, TCSANOW, &new_settings);
	nread = read(0,&ch,1);
	new_settings.c_cc[VMIN]=1;
	tcsetattr(0, TCSANOW, &new_settings);
	if(nread == 1) 
	{
		peek_character = ch;
		return 1;
	}
	return 0;
}
 
extern int readch()
{
	char ch;
 
	if(peek_character != -1) 
	{
		ch = peek_character;
		peek_character = -1;
		return ch;
	}
	read(0,&ch,1);
	return ch;
}
