#include <stdio.h>
#include <string.h>
#include "editor.h"
#include "terminal.h"
#include "piecetable.h"
#include "main.h"
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
    editor->pos_in_piece->piece=editor->piecetable->piece_list->head_edges->next;
    editor->now_ver=0;
    editor->fp=fp;
    editor->filename=NULL;
    editor->history_ver=newVectorCapa(0,sizeof(long),128);
    editor->redo_ver=newVectorCapa(0,sizeof(long),16);
    PUSHBACKV(editor->history_ver,long,0);
    
    return editor;
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
            //* rewrite
            // if (pl->head!=pos->piece)
            // {
            //     temp_piece=pl->head;
            //     while (findNextPiece(temp_piece,ver)!=pos->piece)
            //     {
            //         temp_piece=findNextPiece(temp_piece,ver);
            //     }
            //     pos->piece=temp_piece;
            //     pos->index_in_piece=temp_piece->end-temp_piece->start;
                
            // }
            temp_piece=findPrevPiece(pos->piece,ver);
            if (temp_piece&&temp_piece->is_empty==false)
            {
                pos->piece=temp_piece;
                pos->index_in_piece=temp_piece->end-temp_piece->start;
            }
            
        }
        
        
    }
    
    
}

// 好臭啊，懒得改了
void findNextLine(PosInPiece* pos, long ver, int direction, PieceTable* pt){
    Piece* temp_piece=pos->piece;
    Vector* bufferlinebreak=(pos->piece->is_add)?pt->add_linebreak:pt->original_linebreak;
    if (direction>=0)
    {
        if (pos->piece->is_inline||(!pos->piece->is_inline&&(GETV(bufferlinebreak,long,pos->piece->line_break_end)<(pos->piece->start+pos->index_in_piece))))
        {
            
            while (findNextPiece(temp_piece,ver))
            {
                temp_piece=findNextPiece(temp_piece,ver);
                if (!temp_piece->is_inline)
                {
                    break;
                }    
            }

            if (!temp_piece->is_inline)
            {
                bufferlinebreak=(temp_piece->is_add)?(pt->add_linebreak):(pt->original_linebreak);
                if (GETV(bufferlinebreak,long,temp_piece->line_break_start)!=temp_piece->end)
                {
                    pos->piece=temp_piece;
                    pos->index_in_piece=GETV(bufferlinebreak,long,temp_piece->line_break_start)+1-temp_piece->start;
                }
                else
                {
                    temp_piece=findNextPiece(temp_piece,ver);
                    if (temp_piece&&!temp_piece->is_empty)
                    {
                        pos->piece=temp_piece;
                        pos->index_in_piece=0;
                    }
                    //否则已经是最后一行
                }
            }
            //否则已经是最后一行
            
        }
        else
        {
            for (size_t i = pos->piece->line_break_start; i <= pos->piece->line_break_end; i++)
            {
                if (GETV(bufferlinebreak,long,i)>=pos->piece->start+pos->index_in_piece)
                {
                    if (GETV(bufferlinebreak,long,i)!=pos->piece->end)
                    {
                        pos->index_in_piece=GETV(bufferlinebreak,long,i)-pos->piece->start+1;
                    }
                    else
                    {
                        temp_piece=findNextPiece(pos->piece,ver);
                        if (temp_piece&&!temp_piece->is_empty)
                        {
                            pos->piece=findNextPiece(pos->piece,ver);
                            pos->index_in_piece=0;
                        }
                        //否则已经是最后一行，
                        
                    }
                    
                    break;
                }
                
            }
            
        }
        
        
    }
    else
    {
        //TODO
        int left_line_counter=0;
        // int last_line_break_index=0;
        if (pos->piece->is_inline||(!pos->piece->is_inline&&(GETV(bufferlinebreak,long,pos->piece->line_break_start)>=(pos->piece->start+pos->index_in_piece))))
        {
            temp_piece=pos->piece;
            Piece* temp_piece_prev=findPrevPiece(temp_piece,ver);
            int quit_flag=0;
            while (temp_piece_prev)
            {
                if (temp_piece_prev->is_inline==false)
                {
                    quit_flag=1;
                    break;
                }

                temp_piece=temp_piece_prev;
                temp_piece_prev=findPrevPiece(temp_piece_prev,ver);
                
            }

            if (quit_flag)
            {
                left_line_counter=temp_piece_prev->line_break_end-temp_piece_prev->start+1;
                if (left_line_counter>=2)
                {
                    bufferlinebreak=temp_piece_prev->is_add?pt->add_linebreak:pt->original_linebreak;
                    pos->piece=temp_piece_prev;
                    pos->index_in_piece=GETV(bufferlinebreak,long,temp_piece_prev->line_break_end-1)+1-temp_piece_prev->start;
                }
                else// left_line_couter==1
                {
                    quit_flag=0;
                    temp_piece=temp_piece_prev;
                    temp_piece_prev=findPrevPiece(temp_piece_prev,ver);
                    while (temp_piece_prev)
                    {
                        if (temp_piece_prev->is_inline==false)
                        {
                            quit_flag=1;
                            break;
                        }
                        
                        temp_piece=temp_piece_prev;
                        temp_piece_prev=findPrevPiece(temp_piece_prev,ver);
                    }
    
                    if (quit_flag)
                    {
                        bufferlinebreak=(temp_piece_prev->is_add)?(pt->add_linebreak):(pt->original_linebreak);
                        if (GETV(bufferlinebreak,long,temp_piece_prev->line_break_end)<temp_piece_prev->end)
                        {
                            pos->piece=temp_piece_prev;
                            pos->index_in_piece=GETV(bufferlinebreak,long,temp_piece_prev->line_break_end)+1-temp_piece_prev->start;
                        }
                        else
                        {
                            pos->piece=temp_piece;
                            pos->index_in_piece=0;
                        }
                        
                    }
                    else
                    {
                        pos->piece=temp_piece;
                        pos->index_in_piece=0;
                    }
                }
                
            }
            else
            {
                // 已经在第一行
            }
            
            
            
        }
        else
        {   
            
            for (size_t i = pos->piece->line_break_start; i <= pos->piece->line_break_end; i++)
            {
                if (GETV(bufferlinebreak,long,i)>=pos->piece->start+pos->index_in_piece)
                {
                    // pos->index_in_piece=GETV(bufferlinebreak,long,i-1)-pos->piece->start+1;
                    left_line_counter=i-pos->piece->line_break_start;
                    break;
                }

                if (i==pos->piece->line_break_end&&GETV(bufferlinebreak,long,i)<pos->piece->start+pos->index_in_piece)
                {
                    // pos->index_in_piece=
                    left_line_counter=pos->piece->line_break_end-pos->piece->line_break_start+1;
                }
                
            }

            if (left_line_counter>=2)
            {
                pos->index_in_piece=GETV(bufferlinebreak,long,pos->piece->line_break_start+left_line_counter-2)+1-pos->piece->start;
            }
            else// left_line_counter==1
            {
                temp_piece=pos->piece;
                Piece* temp_piece_prev=findPrevPiece(temp_piece,ver);
                int quit_flag=0;
                while (temp_piece_prev)
                {
                    if (temp_piece_prev->is_inline==false)
                    {
                        quit_flag=1;
                        break;
                    }
                    
                    temp_piece=temp_piece_prev;
                    temp_piece_prev=findPrevPiece(temp_piece_prev,ver);
                }

                if (quit_flag)
                {
                    bufferlinebreak=(temp_piece_prev->is_add)?(pt->add_linebreak):(pt->original_linebreak);
                    if (GETV(bufferlinebreak,long,temp_piece_prev->line_break_end)<temp_piece_prev->end)
                    {
                        pos->piece=temp_piece_prev;
                        pos->index_in_piece=GETV(bufferlinebreak,long,temp_piece_prev->line_break_end)+1-temp_piece_prev->start;
                    }
                    else
                    {
                        pos->piece=temp_piece;
                        pos->index_in_piece=0;
                    }
                    
                }
                else
                {
                    pos->piece=temp_piece;
                    pos->index_in_piece=0;
                }
                
            }
            
            

        }
        
        
    }
    
}


void inNormalMode(char input_c, Editor* editor){
    PosInPiece temp_pos=*editor->pos_in_piece;
    int temp_row;
    int temp_col;
    String* buffer;
    char ch[2]={0};
    // int linelen=0;
    switch (input_c)
    {
    case 'h':
        getCursorPosition(&temp_row,&temp_col);
        // int linelen=GETV(linelen_vector,int,temp_row-1);
        if ((temp_col-1)>5)
        {
            printf("\x1b[%d;%dH", temp_row, temp_col-1);
            findNextChar(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable->piece_list);
        }
        else if (temp_row-1>=1)
        {
            printf("\x1b[%d;%dH", temp_row-1, 6+GETV(linelen_vector,int,temp_row-1-1));
            findNextChar(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable->piece_list);
        }
        else if (temp_row==1)
        {
            if (linecode_start>1)
            {
                findNextChar(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable->piece_list);
                findNextLine(&page_start_pos,editor->now_ver,-1,editor->piecetable);
                linecode_start--;
                // system("clear");
                printf(CLS);
                printf(RTS);
                printNextNLinesWithLineCode(
                    page_start_pos.piece,
                    editor->piecetable,
                    page_start_pos.index_in_piece,
                    linecode_start,
                    wd_size.ws_row-1,
                    &wd_size,
                    stdout,
                    editor->now_ver
                );
                printMode(editor->editor_mode,&wd_size);
                printf("\x1b[%d;%dH",temp_row,6+GETV(linelen_vector,int,temp_row-1));
            }
        }
        
        buffer=editor->pos_in_piece->piece->is_add?editor->piecetable->add:editor->piecetable->original;
        ch[0]=GETV(buffer,char,editor->pos_in_piece->piece->start+editor->pos_in_piece->index_in_piece);
        printLog((ch[0]!='\n')?ch:"Enter",&wd_size);
        break;
    case 'l':
        getCursorPosition(&temp_row,&temp_col);
        // int linelen=GETV(linelen_vector,int,temp_row-1);
        if (!(editor->pos_in_piece->piece==editor->piecetable->piece_list->tail_piece&&editor->pos_in_piece->index_in_piece+editor->pos_in_piece->piece->start==editor->pos_in_piece->piece->end))
        {
            if ((temp_col+1)<=6+GETV(linelen_vector,int,temp_row-1)&&!(temp_col==wd_size.ws_col))
            {
                printf("\x1b[%d;%dH", temp_row, temp_col+1);
                findNextChar(editor->pos_in_piece,editor->now_ver,1,editor->piecetable->piece_list);
            }
            else if (temp_row+1<=wd_size.ws_row-1)
            {
                printf("\x1b[%d;%dH", temp_row+1, 6);
                findNextChar(editor->pos_in_piece,editor->now_ver,1,editor->piecetable->piece_list);
            }
            else if (temp_row+1>wd_size.ws_row-1)
            {
                if (last_print_is_full)
                {
                    findNextChar(editor->pos_in_piece,editor->now_ver,1,editor->piecetable->piece_list);
                    findNextLine(&page_start_pos,editor->now_ver,1,editor->piecetable);
                    linecode_start++;
                    // system("clear");
                    printf(CLS);
                    printf(RTS);
                    printNextNLinesWithLineCode(
                        page_start_pos.piece,
                        editor->piecetable,
                        page_start_pos.index_in_piece,
                        linecode_start,
                        wd_size.ws_row-1,
                        &wd_size,
                        stdout,
                        editor->now_ver
                    );
                    printMode(editor->editor_mode,&wd_size);
                    printf("\x1b[%d;%dH",temp_row,6);
                }
            }
            
        }
        
        buffer=editor->pos_in_piece->piece->is_add?editor->piecetable->add:editor->piecetable->original;
        ch[0]=GETV(buffer,char,editor->pos_in_piece->piece->start+editor->pos_in_piece->index_in_piece);
        // printLog(ch,&wd_size);
        printLog((ch[0]!='\n')?ch:"Enter",&wd_size);
        break;
    case 'j':// todo
        getCursorPosition(&temp_row,&temp_col);
        if (last_print_is_full)
        {
            findNextLine(editor->pos_in_piece,editor->now_ver,1,editor->piecetable);
            linecode_start++;
            // system("clear");
            printf(CLS);
            printf(RTS);
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
        }
        printf("\x1b[%d;%dH", temp_row, temp_col);
        break;
    case 'k'://todo
        getCursorPosition(&temp_row,&temp_col);
        if (linecode_start>1)
        {
            findNextLine(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable);
            linecode_start--;
            // system("clear");
            printf(CLS);
            printf(RTS);
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
        }
        printf("\x1b[%d;%dH", temp_row, temp_col);
        break;
    case 26:
        if (editor->now_ver>0)
        {
            editor->now_ver--;
        }
        page_start_pos.piece=findNextPieceByPE(editor->piecetable->piece_list->head_edges,editor->now_ver);
        page_start_pos.index_in_piece=0;
        *editor->pos_in_piece=page_start_pos;
        linecode_start=1;
        printf(CLS);
        printf(RTS);
        printNextNLinesWithLineCode(
            page_start_pos.piece,
            editor->piecetable,
            page_start_pos.index_in_piece,
            linecode_start,
            wd_size.ws_row-1,
            &wd_size,
            stdout,
            editor->now_ver
        );
        printMode(editor->editor_mode,&wd_size);
        printf("\x1b[%d;%dH", 1,6);
        break;
    case 25:
        if (editor->now_ver<GETV(editor->history_ver,long,editor->history_ver->len-1))
        {
            editor->now_ver++;
        }
        page_start_pos.piece=findNextPieceByPE(editor->piecetable->piece_list->head_edges,editor->now_ver);
        page_start_pos.index_in_piece=0;
        linecode_start=1;
        *editor->pos_in_piece=page_start_pos;
        printf(CLS);
        printf(RTS);
        printNextNLinesWithLineCode(
            page_start_pos.piece,
            editor->piecetable,
            page_start_pos.index_in_piece,
            linecode_start,
            wd_size.ws_row-1,
            &wd_size,
            stdout,
            editor->now_ver
        );
        printMode(editor->editor_mode,&wd_size);
        printf("\x1b[%d;%dH", 1,6);
        break;
        
    default:
        break;
    }
}

void inInsertModeBackend(char input_c, Editor* editor){
    int temp_row;
    int temp_col;
    getCursorPosition(&temp_row,&temp_col);
    if ((input_c>=32&&input_c<=126)||input_c=='\x0d')//todo \n
    {
        input_c=(input_c=='\x0d')?'\n':input_c;
        insertChar(editor->pos_in_piece,input_c,editor->piecetable,editor->now_ver,editor->history_ver);
        editor->now_ver=GETV(editor->history_ver,long,editor->history_ver->len-1);

        if (editor->pos_in_piece->piece==page_start_pos.piece&&editor->pos_in_piece->index_in_piece==page_start_pos.index_in_piece)
        {
            findNextChar(&page_start_pos,editor->now_ver,-1,editor->piecetable->piece_list);
        }
        // findNextChar(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable->piece_list);
        // system("clear");
        printf(CLS);
        printf(RTS);
        printNextNLinesWithLineCode(
            page_start_pos.piece,
            editor->piecetable,
            page_start_pos.index_in_piece,
            linecode_start,
            wd_size.ws_row-1,
            &wd_size,
            stdout,
            editor->now_ver
        );
        printMode(editor->editor_mode,&wd_size);
        // printf("\x1b[%d;%dH", temp_row, temp_col+1);//todo where is the cursor
        // inNormalMode('l',editor);
        // printMode(editor->editor_mode,&wd_size);
        if ((temp_col+1)<=6+GETV(linelen_vector,int,temp_row-1)&&!(temp_col==wd_size.ws_col))
        {
            printf("\x1b[%d;%dH", temp_row, temp_col+1);
            // findNextChar(editor->pos_in_piece,editor->now_ver,1,editor->piecetable->piece_list);
        }
        else if (temp_row+1<=wd_size.ws_row-1)
        {
            printf("\x1b[%d;%dH", temp_row+1, 6);
            // findNextChar(editor->pos_in_piece,editor->now_ver,1,editor->piecetable->piece_list);
        }
        else if (temp_row+1>wd_size.ws_row-1)
        {
            if (last_print_is_full)
            {
                // findNextChar(editor->pos_in_piece,editor->now_ver,1,editor->piecetable->piece_list);
                findNextLine(&page_start_pos,editor->now_ver,1,editor->piecetable);
                linecode_start++;
                // system("clear");
                printf(CLS);
                printf(RTS);
                printNextNLinesWithLineCode(
                    page_start_pos.piece,
                    editor->piecetable,
                    page_start_pos.index_in_piece,
                    linecode_start,
                    wd_size.ws_row-1,
                    &wd_size,
                    stdout,
                    editor->now_ver
                );
                printMode(editor->editor_mode,&wd_size);
                printf("\x1b[%d;%dH",temp_row,6);
            }
        }
            
    }
    else if (input_c==127)
    {
        Piece* temp_p=editor->pos_in_piece->piece;
        PosInPiece temp_pos={0};
        int del_count=0;
        Vector* temp_linelen=newVectorCapa(linelen_vector->len,linelen_vector->type_size,linelen_vector->capacity);
        while (input_c==127)
        {
            del_count++;
            temp_p=editor->pos_in_piece->piece;
            memcpy(temp_linelen->data,linelen_vector->data,temp_linelen->len*temp_linelen->type_size);
            getCursorPosition(&temp_row,&temp_col);
            temp_pos=*editor->pos_in_piece;
            findNextChar(&temp_pos,editor->now_ver,-1,editor->piecetable->piece_list);
            if (!(temp_pos.piece==editor->pos_in_piece->piece&&temp_pos.index_in_piece==editor->pos_in_piece->index_in_piece) &&temp_pos.piece==page_start_pos.piece&&temp_pos.index_in_piece==page_start_pos.index_in_piece)
            {
                findNextChar(&page_start_pos,editor->now_ver,1,editor->piecetable->piece_list);
            }

            String* buffer=NULL;
            Vector* linebreak_buffer=NULL;
            if (temp_p->is_empty)// actually temp_p->is_empty must be false
            {
                temp_p=findNextPiece(temp_p,editor->now_ver);
            }
            else if (editor->pos_in_piece->index_in_piece==0)
            {
                if (findPrevPiece(temp_p,editor->now_ver))
                {
                    temp_p=findPrevPiece(temp_p,editor->now_ver);
                    if (temp_p->end-temp_p->start>0)
                    {
                        if (del_count-2>=0)
                        {
                            Piece* del_p=findNextPiece(temp_p,GETV(editor->history_ver,long,editor->history_ver->len-2));
                            if (del_p&&findNextPiece(del_p,GETV(editor->history_ver,long,editor->history_ver->len-2))==findNextPiece(temp_p,editor->now_ver)&&del_p->is_add==temp_p->is_add&&del_p->start==temp_p->end+1)
                            {
                                buffer=temp_p->is_add?editor->piecetable->add:editor->piecetable->original;
                                linebreak_buffer=temp_p->is_add?editor->piecetable->add_linebreak:editor->piecetable->original_linebreak;
                                if (GETV(buffer,char,temp_p->end)=='\n')
                                {
                                    // temp_p->line_break_end--;
                                    del_p->line_break_start=temp_p->line_break_end;
                                    del_p->line_break_end=(del_p->is_inline)?(del_p->line_break_start):del_p->line_break_end;
                                    temp_p->line_break_end--;
                                    del_p->is_inline=false;
                                    temp_p->is_inline=(temp_p->line_break_end<temp_p->line_break_start)?true:false;
                                    
                                }

                                temp_p->end--;
                                del_p->start--;
                                
                            }
                            else//todo \n
                            {
                                deletePiece(cutPiece(temp_p,temp_p->end-temp_p->start,editor->piecetable),editor->piecetable,editor->now_ver);
                                PUSHBACKV(editor->history_ver,long,editor->piecetable->ver_counter);
                            }
                            
                        }
                        else//todo \n
                        {
                            deletePiece(cutPiece(temp_p,temp_p->end-temp_p->start,editor->piecetable),editor->piecetable,editor->now_ver);
                            PUSHBACKV(editor->history_ver,long,editor->piecetable->ver_counter);
                        }
                        

                    }
                    else//todo \n
                    {
                        deletePiece(temp_p,editor->piecetable,editor->now_ver);
                        PUSHBACKV(editor->history_ver,long,editor->piecetable->ver_counter);
                    }
                    
                }
                //otherwise temp_p is head, nothing
                
            }
            else
            {
                temp_p=editor->pos_in_piece->piece;
                editor->pos_in_piece->piece=cutPiece(editor->pos_in_piece->piece,editor->pos_in_piece->index_in_piece,editor->piecetable);
                editor->pos_in_piece->index_in_piece=0;
                if (temp_p->start==temp_p->end)
                {
                    deletePiece(temp_p,editor->piecetable,editor->now_ver);
                    PUSHBACKV(editor->history_ver,long,editor->piecetable->ver_counter);
                }
                else
                {
                    deletePiece(cutPiece(temp_p,temp_p->end-temp_p->start,editor->piecetable),editor->piecetable,editor->now_ver);
                    PUSHBACKV(editor->history_ver,long,editor->piecetable->ver_counter);
                }
                
            }
            
            editor->now_ver=GETV(editor->history_ver,long,editor->history_ver->len-1);
            
            printf(CLS);
            printf(RTS);
            printNextNLinesWithLineCode(
                page_start_pos.piece,
                editor->piecetable,
                page_start_pos.index_in_piece,
                linecode_start,
                wd_size.ws_row-1,
                &wd_size,
                stdout,
                editor->now_ver
            );
            printMode(editor->editor_mode,&wd_size);

            if ((temp_col-1)>5)
            {
                printf("\x1b[%d;%dH", temp_row, temp_col-1);
                // findNextChar(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable->piece_list);
            }
            else if (temp_row-1>=1)
            {
                printf("\x1b[%d;%dH", temp_row-1, 6+GETV(temp_linelen,int,temp_row-1-1));
                // findNextChar(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable->piece_list);
            }
            else if (temp_row==1)
            {
                if (linecode_start>1)
                {
                    // findNextChar(editor->pos_in_piece,editor->now_ver,-1,editor->piecetable->piece_list);
                    findNextLine(&page_start_pos,editor->now_ver,-1,editor->piecetable);
                    linecode_start--;
                    // system("clear");
                    printf(CLS);
                    printf(RTS);
                    printNextNLinesWithLineCode(
                        page_start_pos.piece,
                        editor->piecetable,
                        page_start_pos.index_in_piece,
                        linecode_start,
                        wd_size.ws_row-1,
                        &wd_size,
                        stdout,
                        editor->now_ver
                    );
                    printMode(editor->editor_mode,&wd_size);
                    printf("\x1b[%d;%dH",temp_row,6+GETV(linelen_vector,int,temp_row-1)-GETV(temp_linelen,int,temp_row-1));
                }
                else
                {
                    printf("\x1b[%d;%dH",temp_row,temp_col);
                }
                
            }
            else
            {
                printf("\x1b[%d;%dH",temp_row,temp_col);
            }
            

            input_c=getc(stdin);

        }
        
        // editor->now_ver=GETV(editor->history_ver,long,editor->history_ver->len-1);

        if (input_c==27)
        {
            editor->editor_mode=M_NORMAL;
            printMode(editor->editor_mode,&wd_size);
        }
        else if ((input_c>=32&&input_c<=126)||input_c=='\x0d')
        {
            inInsertModeBackend(input_c,editor);
        }
        
        free(temp_linelen);   

    }
    // getCursorPosition(&temp_row,&temp_col);


    
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
        case 27:
            goto inCommamdMode_end;
        case 127:
            printf("\x1b[1D \x1b[1D");
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
        // printLog("111111111111111111",&wd_size);
        fclose(editor->fp);
        freeEditor(editor);
        exit(0);
    }
    // printf("\x1b[%d;%dH",size->ws_row,1);
    // for (size_t i = 0; i < size->ws_col-8; i++)
    // {
    //     printf(" ");
    // }
    inCommamdMode_end:
    printf("\x1b[u");
}


void insertChar(PosInPiece* pos, char input_c, PieceTable* pt,long ver,Vector* undo_ver){
    Piece* temp_p=NULL;
    if (pos->piece->is_empty)//*problem
    {
        temp_p=insertAfterPiece(pos->piece,pt,ver);
        pos->piece=findNextPiece(temp_p,ver);
        pos->index_in_piece=0;
        PUSHBACKV(undo_ver,long,pt->ver_counter);
        temp_p->is_empty=false;
        PUSHBACKV(pt->add,char,input_c);
        temp_p->end=temp_p->start=pt->add->len-1;
    }
    else if (pos->index_in_piece==0)
    {
        temp_p=findPrevPiece(pos->piece,ver);
        if (temp_p&&temp_p->is_add&&temp_p->end==pt->add->len-1)
        {
            PUSHBACKV(pt->add,char,input_c);
            temp_p->end++;
        }
        else
        {
            temp_p=insertBeforePiece(pos->piece,pt,ver);
            PUSHBACKV(undo_ver,long,pt->ver_counter);
            temp_p->is_empty=false;
            PUSHBACKV(pt->add,char,input_c);
            temp_p->end=temp_p->start=pt->add->len-1;
        }
    }
    else// cut the piece
    {
        Piece* p_next=NULL;
        p_next=cutPiece(pos->piece,pos->index_in_piece,pt);
        temp_p=insertAfterPiece(pos->piece,pt,ver);
        PUSHBACKV(undo_ver,long,pt->ver_counter);
        temp_p->is_empty=false;
        PUSHBACKV(pt->add,char,input_c);
        temp_p->end=temp_p->start=pt->add->len-1;
        pos->piece=p_next;
        pos->index_in_piece=0;
    }
    

    if (input_c=='\n')
    {
        // temp_p->is_inline=false;
        PUSHBACKV(pt->add_linebreak,long,temp_p->end);
        if (temp_p->is_inline)
        {
            temp_p->is_inline=false;
            temp_p->line_break_start=temp_p->line_break_end=pt->add_linebreak->len-1;
        }
        else
        {
            temp_p->line_break_end=pt->add_linebreak->len-1;
        }
        
    }
    
    // pos->piece=temp_p;
    // pos->index_in_piece=temp_p->end;
    
}
