#ifndef MAIN_H
#define MAIN_H
#include "terminal.h"

extern struct winsize wd_size;
extern struct termios orig_termios;
extern int now_linenum;
extern Vector* linelen_vector;
extern long linecode_start;
extern bool last_print_is_full;

#endif