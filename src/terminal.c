#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "terminal.h"

struct winsize wd_size;
struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char readKey() {
    char c;
    read(STDIN_FILENO, &c, 1);
    return c;
}

void handleResize(int sig) {
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &wd_size) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
}

void getTermialSize(struct winsize* size){
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, size) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
}

void updateTermial(){
    
}
