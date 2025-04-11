#ifndef TERMINAL_H
#define TERMINAL_H
#include <sys/ioctl.h>
#include "editor.h"

extern int editor_mode;

void enableRawMode();
char readKey();
void handleResize(int sig);
void getTermialSize(struct winsize* size);



#endif