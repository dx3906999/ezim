#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "piecetable.h"

PieceList* newPieceList(){
    PieceList* pl=(PieceList*)malloc(sizeof(PieceList));
    if (!pl)
    {
        return NULL;
    }

    pl->head=NULL;
    pl->tail=NULL;
    pl->len=0;
    pl->piece_num=0;
    
}

void freePieceList(PieceList* pl){
    Piece* temp=NULL;
    while (pl->head)
    {
        temp=pl->head;
        pl->head=pl->head->bk;
        free(temp);
    }
    
}



PieceTable* newPieceTable(FILE* fp){
    PieceTable* piecetable=(PieceTable*)malloc(sizeof(PieceTable));
    if (!piecetable)
    {
        return NULL;
    }

    piecetable->add=newVector(0,sizeof(char));
    piecetable->piece_list=newPieceList();
    
    size_t f_size=0;
    fseek(fp,0,SEEK_END);
    f_size=ftell(fp);
    fseek(fp,0,SEEK_SET);

    piecetable->original=newVector(f_size,sizeof(char));

    Piece* p=(Piece*)malloc(sizeof(Piece));
    PUSHBACK_DL(piecetable->piece_list,p);
    piecetable->piece_list->piece_num++;
    piecetable->piece_list->len=f_size;

    if (f_size!=0)
    {
        fread(piecetable->original->data,sizeof(char),f_size,fp);
        p->is_add=false;
        p->start=0;
        p->end=f_size-1;
        p->is_empty=false;
    }
    else
    {
        p->is_add=true;
        p->start=0;
        p->end=0;
        p->is_empty=true;
    }
    
    return piecetable;
}

void printPieces(PieceTable* piecetable, FILE* fp){
    Piece* p=piecetable->piece_list->head;
    while (p)
    {
        if (!p->is_empty)
        {
            if (p->is_add)
            {
                for (size_t i = p->start; i <= p->end ; i++)
                {
                    putc(GETV(piecetable->add,char,i),fp);
                }
                
            }
            else
            {
                for (size_t i = p->start; i <= p->end; i++)
                {
                    putc(GETV(piecetable->original,char,i),fp);
                }
                
            }
            
        }
        
    }
    
}
