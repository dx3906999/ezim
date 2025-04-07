#ifndef DISPLAY_H
#define DISPLAY_H


void get_terminal_size(int* width, int* height);
void get_cursor_position(int *x, int *y);
int kbhit();
int getch();
#endif
