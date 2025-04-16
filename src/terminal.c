#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "terminal.h"
#include "piecetable.h"
#include "utils.h"

struct winsize wd_size;
struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    system("tput rmcup");
}

void enableRawMode() {
    system("tput smcup");
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

void printNextNLines(Piece* startPiece, PieceTable* pt, long start_index, long nlines, struct winsize* size, FILE* fp, long ver){
    Piece* p=startPiece;
    char ch=0;
    long line_count=0;
    String* buffer=NULL;
    long index=start_index;
    for (int i = 0; i < nlines && p; i++)
    {
        if (!p->is_empty)
        {
            buffer=(p->is_add)?pt->add:pt->original;
            if (p->is_inline)
            {
                for (size_t j = index; j <= p->end; j++)
                {
                    ch=GETV(buffer,char,j);
                    putc(ch,fp);
                    line_count++;
                }

                p=findNextPiece(p,ver);
                index=0;
                i--;
            }
            else
            {
                for (size_t j = index; j <= p->end; j++)
                {
                    ch=GETV(buffer,char,j);
                    
                    line_count=(ch=='\n')?0:(line_count+1);
                    if (ch=='\n')
                    {
                        if (i!=nlines-1)
                        {
                            putc(ch,fp);
                        }
                        
                        if (fp==stdout&&i!=nlines-1)
                        {
                            putc('\r',fp);
                        }

                        if (j==p->end)
                        {
                            index=0;
                            p=findNextPiece(p,ver);
                        }
                        else
                        {
                            index=j+1;
                        }
                        break;
                    }
                    else
                    {
                        putc(ch,fp);
                    }
                    

                    if (j==p->end&&ch!='\n')
                    {
                        index=0;
                        p=findNextPiece(p,ver);
                        i--;
                        break;
                    }
                    
                }

            }
            
        }

        i+=line_count/size->ws_col;

    }
    
}


void printNextNLinesWithLineCode(Piece* startPiece, PieceTable* pt, long start_index, long start_linecode, long nlines, struct winsize* size, FILE* fp, long ver){
    Piece* p=startPiece;
    char ch=0;
    long line_count=0;
    String* buffer=NULL;
    long index=start_index;
    long linecode=start_linecode;
    for (int i = 0; i < nlines && p; i++,linecode++)
    {
        fprintf(fp,"\x1b[2m%4d \x1b[0m",linecode);
        if (!p->is_empty)
        {
            buffer=(p->is_add)?pt->add:pt->original;
            if (p->is_inline)
            {
                for (size_t j = index; j <= p->end; j++)
                {
                    ch=GETV(buffer,char,j);
                    if (line_count%(size->ws_col-5)==0&&line_count!=0)
                    {
                        fprintf(fp,"     ");
                    }
                    putc(ch,fp);
                    line_count++;
                }

                p=findNextPiece(p,ver);
                index=0;
                i--;
            }
            else
            {
                for (size_t j = index; j <= p->end; j++)
                {
                    ch=GETV(buffer,char,j);
                    
                    line_count=(ch=='\n')?0:(line_count+1);
                    if (ch=='\n')
                    {
                        if (i!=nlines-1)
                        {
                            putc(ch,fp);
                        }
                        
                        if (fp==stdout&&i!=nlines-1)
                        {
                            putc('\r',fp);
                        }
                        // index=j+1;

                        if (j==p->end)
                        {
                            index=0;
                            p=findNextPiece(p,ver);
                        }
                        else
                        {
                            index=j+1;
                        }
                        
                        break;
                    }
                    else
                    {
                        putc(ch,fp);
                        if (line_count%(size->ws_col-5)==0&&line_count!=0)
                        {
                            fprintf(fp,"     ");
                        }
                    }
                    

                    if (j==p->end&&ch!='\n')
                    {
                        index=0;
                        p=findNextPiece(p,ver);
                        i--;
                        break;
                    }
                    
                }

            }
            
        }

        i+=line_count/(size->ws_col-5);

    }
    
}

void printMode(volatile int mode,struct winsize* size){
    printf("\x1b[s");
    printf("\x1b[%d;%dH",size->ws_row,size->ws_col-7);
    switch (mode)
    {
    case M_COMMAND:
        printf("Command");
        break;
    case M_INSERT:
        printf(" Insert");
        break;
    case M_NORMAL:
        printf(" Normal");
        break;
    default:
        break;
    }
    printf("\x1b[u");
}