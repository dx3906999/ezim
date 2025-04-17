#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include "terminal.h"
#include "utils.h"
#include "piecetable.h"
#include "editor.h"
#include "main.h"



int main(int argc, char const *argv[])
{
    // enableRawMode();

    // printf("Raw mode ON (Press 'q' to quit)...\r\n");

    // char c;
    // while (1) {
    //     read(STDIN_FILENO, &c, 1);  // 逐字符读取输入
    //     if (c == 'q') break;
    //     printf("You pressed: %d \r\n", c);  // 显示按下的键值
    // }
    FILE* fp=NULL;
    if (argc>1)
    {
        if (access(argv[1],F_OK)==0)
        {
            if (access(argv[1],R_OK)==0&&access(argv[1],W_OK)==0)
            {
                fp=fopen(argv[1],"r+");
            }
            else
            {
                perror("You don't have access!\n\r");
                exit(0);
            }
            
        }
        else
        {
            fp=fopen(argv[1],"w+");
        }
        
        
    }
    else
    {
        fp=fopen("tempbuffer","w+");
    }

    signal(SIGWINCH, handleResize);
    getTermialSize(&wd_size);

    enableRawMode();
    // printf(CLS);
    system("clear");
    printf(RTS);
    Editor* editor=newEditor(fp);
    editor->filename=(argc>1)?argv[1]:"tempbuffer";

    //* 循环数组
    linelen_vector=newVectorCapa(wd_size.ws_row,sizeof(int),wd_size.ws_row);
    linecode_start=1;
    last_print_is_full=false;

    // printPieces(editor->piecetable,stdout,editor->now_ver);
    printNextNLinesWithLineCode(
        editor->pos_in_piece->piece,
        editor->piecetable,
        editor->pos_in_piece->index_in_piece,
        linecode_start,
        wd_size.ws_row-1,
        &wd_size,
        stdout,
        editor->now_ver
    );
    printMode(editor->editor_mode,&wd_size);

    // debug
    printf(RTS);
    printf("\x1b[5C");
    // printf("ok.\n\r");
    printLog("ok.",&wd_size);

    while (1)
    {
        int editor_mode_temp=editor->editor_mode;
        char ch=getchar();
        updateEditorMode(&editor->editor_mode,ch);
        printMode(editor->editor_mode,&wd_size);
        if (editor->editor_mode!=editor_mode_temp)
        {
            
        }

        switch (editor->editor_mode)
        {
        case M_NORMAL:
            inNormalMode(ch,editor);
            break;
        case M_COMMAND:
            inCommamdMode(ch,editor,&wd_size);
            editor->editor_mode=M_NORMAL;
            printMode(M_NORMAL,&wd_size);
            break;
        default:
            break;
        }
        
    }

    





    return 0;
}
