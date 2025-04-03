#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__linux__)
#include <sys/ioctl.h>
#include <termios.h>
#endif

#include <stdio.h>
#include <stdlib.h>

void get_terminal_size(int* width, int* height) {
    #if defined(_WIN32)
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        *width = (int)(csbi.srWindow.Right-csbi.srWindow.Left+1);
        *height = (int)(csbi.srWindow.Bottom-csbi.srWindow.Top+1);
    #elif defined(__linux__)
        struct winsize w;
        ioctl(fileno(stdout), TIOCGWINSZ, &w);
        *width = (int)(w.ws_col);
        *height = (int)(w.ws_row);
    #endif
}

void get_cursor_position(int *x, int *y) {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            *x = csbi.dwCursorPosition.X;
            *y = csbi.dwCursorPosition.Y;
        } else {
            *x = *y = -1;
        }
    #else
        printf("\033[6n");
        fflush(stdout);
        
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        char buf[32] = {0};
        int i = 0;
        while (i < sizeof(buf) - 1) {
            read(STDIN_FILENO, &buf[i], 1);
            if (buf[i] == 'R') break;
            i++;
        }
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        
        if (sscanf(buf, "\033[%d;%dR", y, x) != 2) {
            *x = *y = -1;
        }
    #endif
}



