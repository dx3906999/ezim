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
int now_linenum;
Vector* linelen_vector;
long linecode_start;
bool last_print_is_full;
PosInPiece page_start_pos;

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


void printNextNLinesWithLineCode(Piece* startPiece, PieceTable* pt, long start_index_in_piece, long start_linecode, long nlines, struct winsize* size, FILE* fp, long ver){
    Piece* p=startPiece;
    char ch=0;
    long line_count=0;
    String* buffer=NULL;
    long index=start_index_in_piece;
    long linecode=start_linecode;
    int linelen_index=0;
    resizeVector(linelen_vector,nlines);
    for (size_t i = 0; i < nlines; i++)
    {
        GETV(linelen_vector,int,i)=0;
    }
    fprintf(fp,"\x1b[2m%4d \x1b[0m",linecode);
    for (int i = 0; i < nlines && p; i++)
    {
        
        if (!p->is_empty)
        {
            // fprintf(fp,"\x1b[2m%4d \x1b[0m",linecode);
            buffer=(p->is_add)?pt->add:pt->original;
            if (p->is_inline)
            {
                for (size_t j = index+p->start; j <= p->end; j++)
                {
                    ch=GETV(buffer,char,j);
                    if (line_count%(size->ws_col-5)==0&&line_count!=0)
                    {
                        fprintf(fp,"     ");
                        linelen_index++;
                    }
                    putc(ch,fp);
                    line_count++;
                    GETV(linelen_vector,int,linelen_index)+=1;
                }

                p=findNextPiece(p,ver);
                index=0;
                i--;
            }
            else
            {
                for (size_t j = index+p->start; j <= p->end; j++)
                {
                    ch=GETV(buffer,char,j);
                    // GETV(linelen_vector,int,i+line_count/(size->ws_col-5))=line_count%(size->ws_col-5);
                    // line_count=(ch=='\n')?0:(line_count+1);
                    if (ch=='\n')
                    {
                        // GETV(linelen_vector,int,linelen_index)+=1;
                        line_count=0;
                        linecode++;
                        linelen_index++;
                        if (i!=nlines-1)
                        {
                            putc(ch,fp);
                        }
                        
                        if (fp==stdout&&i!=nlines-1)
                        {
                            putc('\r',fp);
                            fprintf(fp,"\x1b[2m%4d \x1b[0m",linecode);
                        }
                        
                        
                        // index=j+1;

                        if (j==p->end)
                        {
                            index=0;
                            p=findNextPiece(p,ver);
                        }
                        else
                        {
                            index=j+1-p->start;
                        }
                        
                        break;
                    }
                    else
                    {

                        if (line_count%(size->ws_col-5)==0&&line_count!=0)
                        {
                            fprintf(fp,"     ");
                            linelen_index++;
                        }
                        line_count++;
                        GETV(linelen_vector,int,linelen_index)+=1;
                        putc(ch,fp);
                    }
                    

                    if (j==p->end&&ch!='\n')
                    {
                        index=0;
                        p=findNextPiece(p,ver);
                        i--;
                        // linelen_index--;
                        break;
                    }
                    
                }

            }
            
        }
        else
        {
            do
            {
                p=findNextPiece(p,ver);
            } while (p!=NULL&&p->is_empty);
            i--;
            index==0;
        }
        

        i+=line_count/(size->ws_col-5);
        last_print_is_full=(i==nlines-1);
    }
    
    now_linenum=linecode+1-start_linecode;
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

void printLog(char* log, struct winsize* size){
    printf("\x1b[s");
    // printf("\x1b[%d;%dH",size->ws_row,1);
    //clear line
    printf("\x1b[%d;%dH",size->ws_row,1);
    for (size_t i = 0; i < size->ws_col-8; i++)
    {
        printf(" ");
    }
    printf("\x1b[%d;%dH",size->ws_row,1);
    printf("%s",log);
    printf("\x1b[u");
}

void getCursorPosition(int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;

    write(STDOUT_FILENO, "\x1b[6n", 4);

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }

    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[') return;
    sscanf(&buf[2], "%d;%d", rows, cols);
}

void rollLine(int diretion){

}



void changeCursorChar(int direction){

}