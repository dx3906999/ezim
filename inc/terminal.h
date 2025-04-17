#ifndef TERMINAL_H
#define TERMINAL_H
#include <sys/ioctl.h>
#include "editor.h"

#define CLS "\x1b[2J"
#define RTS "\x1b[H"
// extern int editor_mode;

void enableRawMode();
char readKey();
void handleResize(int sig);
void getTermialSize(struct winsize* size);
void printNextNLines(Piece* startPiece, PieceTable* pt, long start_index, long nlines, struct winsize* size, FILE* fp, long ver);
void printNextNLinesWithLineCode(Piece* startPiece, PieceTable* pt, long start_index, long start_linecode, long nlines, struct winsize* size, FILE* fp, long ver);
void printMode(volatile int mode,struct winsize* size);
void printLog(char* log, struct winsize* size);
void getCursorPosition(int *rows, int *cols);

#endif