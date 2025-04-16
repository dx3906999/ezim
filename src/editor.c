#include <stdio.h>
#include "editor.h"
#include "terminal.h"
#include "piecetable.h"

// int editor_mode=0;
// PosInPiece pos_in_piece={0};
// PieceTable* global_piecetable=NULL;
// long now_ver;

Editor* newEditor(FILE* fp){
    Editor* editor=(Editor*)malloc(sizeof(Editor));
    if (!editor)
    {
        return NULL;
    }

    editor->editor_mode=M_NORMAL;
    editor->pos_in_piece=(PosInPiece*)malloc(sizeof(PosInPiece));
    editor->pos_in_piece->index_in_piece=0;
    editor->piecetable=newPieceTable(fp);
    editor->pos_in_piece->piece=editor->piecetable->piece_list->head;
    editor->now_ver=0;
    editor->fp=fp;
    editor->filename=NULL;

    
}

void freeEditor(Editor* editor){
    free(editor->pos_in_piece);
    freePieceTable(editor->piecetable);
    free(editor);
}


void updateEditorMode(volatile int* current_mode, char input_c){
    switch (*current_mode)
    {
    case M_INSERT:
        if (input_c==27)
        {
            *current_mode=M_NORMAL;
        }
        break;
    case M_NORMAL:
        if (input_c=='i')
        {
            *current_mode=M_INSERT;
        }
        else if (input_c==':')
        {
            *current_mode=M_COMMAND;
        }
        
        break;
    case M_COMMAND:
        if (input_c==27)
        {
            *current_mode=M_NORMAL;
        }
        break;
    default:
        break;
    }
}

void findNextChar(PosInPiece* pos, long ver, int direction, PieceList* pl){
    Piece* temp_piece=NULL;
    if (direction>=0)
    {
        if (pos->piece->end>=pos->piece->start+pos->index_in_piece+1)
        {
            pos->index_in_piece++;
        }
        else
        {
            temp_piece=findNextPiece(pos->piece,ver);
            if (temp_piece&&temp_piece->is_empty==false)
            {
                pos->piece=temp_piece;
                pos->index_in_piece=0;
            }
        }

    }
    else
    {
        if (pos->index_in_piece>0)
        {
            pos->index_in_piece--;
        }
        else
        {
            if (pl->head!=pos->piece)
            {
                temp_piece=pl->head;
                while (findNextPiece(temp_piece,ver)!=pos->piece)
                {
                    temp_piece=findNextPiece(temp_piece,ver);
                }
                pos->piece=temp_piece;
                pos->index_in_piece=temp_piece->end-temp_piece->start;
                
            }
            
        }
        
        
    }
    
    
}

void findNextLine(PosInPiece* pos, long ver, int direction, PieceList* pl){
    Piece* temp_piece;
    if (direction>=0)
    {
        
    }
}

void inNormalModeBackend(char input_c, Editor* editor){
    switch (input_c)
    {
    case 'h':
        findNextChar(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable->piece_list);
        break;
    case 'l':
        findNextChar(editor->pos_in_piece,editor->now_ver,1,editor->piecetable->piece_list);
        break;
    default:
        break;
    }
}

void inInsertModeBackend(char input_c, Editor* editor){

}

void inCommamdMode(char input_c, Editor* editor, struct winsize* size){
    int state=0;
    printf("\x1b[s");
    
    //clear line
    printf("\x1b[%d;%dH",size->ws_row,1);
    for (size_t i = 0; i < size->ws_col-8; i++)
    {
        printf(" ");
    }

    printf("\x1b[%d;%dH:",size->ws_row,1);
    while ((input_c=getc(stdin))!='\n'&&input_c!='\x0d')
    {
        switch (input_c)
        {
        case 'q':
            state|=COMMAND_QUIT;
            break;
        case 'w':
            state|=COMMAND_SAVE;
            break;
        default:
            break;
        }
        printf("%c",input_c);
    }

    if (state&COMMAND_SAVE)
    {
        fclose(editor->fp);
        editor->fp=fopen(editor->filename,"w");
        printPieces(editor->piecetable,editor->fp,editor->now_ver);
    }

    if (state&COMMAND_QUIT)
    {
        fclose(editor->fp);
        freeEditor(editor);
        exit(0);
    }
    // printf("\x1b[%d;%dH",size->ws_row,1);
    // for (size_t i = 0; i < size->ws_col-8; i++)
    // {
    //     printf(" ");
    // }
    
    printf("\x1b[u");
}

// void main_loop(){
    // while (1)
    // {
    //     int editor_mode_temp=editor_mode;
    //     char ch=getc(stdin);
    //     updateEditorMode(&editor_mode,ch);

    //     if (editor_mode!=editor_mode_temp)
    //     {
    //         continue;
    //     }

    //     switch ()
    //     {
    //     case constant expression:
    //         /* code */
    //         break;
        
    //     default:
    //         break;
    //     }
        
    // }
    

// }
