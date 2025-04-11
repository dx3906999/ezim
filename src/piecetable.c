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

//* need repiring
//Only O(n)
Piece* findPiece(PieceList* pl,long index, long* offset){
    index=index<pl->len?index:pl->len;
    Piece* p=NULL;
    if (index>(pl->len/2))
    {
        p=pl->tail;
        index=pl->len-1-index;
        while (p&&index>p->len-1)
        {
            p=p->fd;
            index-=p->len;
        }

        *offset=p->len-1-index;
    }
    else
    {
        p=pl->head;
        while (p&&index>p->len-1)
        {
            p=p->bk;
            index-=p->len;
        }

        *offset=index;
        
    }
    
    return p;
}


PieceTable* newPieceTable(FILE* fp, bool is_new){
    PieceTable* pt=(PieceTable*)malloc(sizeof(PieceTable));


    pt->original=newString();
    pt->add=newString();
    pt->piece_list=newPieceList();
    Piece* p=(Piece*)malloc(sizeof(Piece));
    // Piece* final=(Piece*)malloc(sizeof(Piece));

    if (fp!=0)
    {
        size_t f_size=0;
        fseek(fp,0,SEEK_END);
        f_size=ftell(fp);
        fseek(fp,0,SEEK_SET);
        if (f_size>0)
        {
            char* buf=(char*)malloc(f_size);
    
            fread(buf,sizeof(char),f_size,fp);
            appendString(pt->original,buf,f_size);
            free(buf);
        }
    }
    
    
    pt->piece_list->head=p;
    pt->piece_list->tail=p;
    pt->piece_list->len=pt->original->len;
    pt->piece_list->piece_num=2;

    // final->bk=NULL;
    // final->fd=p;
    // final->head_chunk=NULL;
    // final->tail_chunk=NULL;
    // final->h_index=-1;
    // final->t_index=-1;
    // final->len=-1;

    p->bk=NULL;
    p->fd=NULL;
    p->head_chunk=pt->original->head;
    p->tail_chunk=pt->original->tail;
    p->h_index=0;
    p->t_index=(p->tail_chunk)?(pt->original->tail->len-1):0;
    p->len=pt->original->len;
    
    //* caution
    // pt->now_piece=final;
    // pt->now_index=0;

    return pt;
}

// bool isFinalPiece(Piece* p){
//     return (p->len==-1)?true:false;
// }

void freePieceTable(PieceTable* pt){
    freeString(pt->original);
    freeString(pt->add);
    freePieceList(pt->piece_list);
}



// problem
Piece* cutPiece(Piece* p, long first_len){
    if (first_len>p->len)
    {
        return NULL;
    }

    Piece* new_p=(Piece*)malloc(sizeof(Piece));
    long temp_len=first_len;
    int temp_index=p->h_index;
    StrChunk* sc=p->head_chunk;
    if (!new_p)
    {
        return NULL;
    }
    new_p->bk=p->bk;
    new_p->fd=p;
    p->bk->fd=new_p;
    p->bk=p;
    new_p->tail_chunk=p->tail_chunk;
    new_p->t_index=p->t_index;
    new_p->len=p->len-first_len;
    p->len=first_len;

    
    while (temp_len>sc->len-temp_index)
    {
        sc=sc->bk;
        temp_index=0;
        temp_len-=sc->len-temp_index;
    }

    p->tail_chunk=sc;
    p->t_index=temp_len-1;
    new_p->h_index=temp_len;
    new_p->head_chunk=sc;

    return new_p;

}

// Piece* insertPiece(PieceTable* pt){
//     Piece* p=(Piece*)malloc(sizeof(Piece));
//     if (!p)
//     {
//         return NULL;
//     }
//     p->len=0;
//     //* p's something
//     p->head_chunk=pt->add->tail;
//     p->tail_chunk=pt->add->tail;
//     p->h_index=pt->add->tail->len;
//     p->t_index=pt->add->tail->len;


//     if (pt->now_index==0)
//     {
//         p->bk=pt->now_piece;
//         p->fd=pt->now_piece->fd;

//         if (pt->now_piece->fd==NULL)
//         {
//             pt->piece_list->head=p;
//         }
//         else
//         {
//             pt->now_piece->fd->bk=p;
//         }

//         pt->now_piece->fd=p;
        
//     }
//     else
//     {
//         Piece* second_piece=cutPiece(pt->now_piece,pt->now_index+1);
//         pt->now_piece->bk=p;
//         second_piece->fd=p;
//         p->fd=pt->now_piece;
//         p->bk=second_piece;
//         pt->now_piece=p;
//         pt->now_index=0;
//     }
    
    
    
// }
