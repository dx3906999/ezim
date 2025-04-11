#ifndef PIECETABLE_H
#define PIECETABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

#define INSERT_AFTER(p,add) {(add)->fd=(p);(add)->bk=(p)->bk;if((p)->bk){(p)->bk->fd=(add);}(p)->bk=(add);}
#define PUSHBACK_DL(dlist,p) {if((dlist)->tail){INSERT_AFTER((dlist)->tail,p);(dlist)->tail=(p);}else{(dlist)->head=(dlist)->tail=(p);(p)->fd=(p)->bk=NULL;}}


typedef Vector String;

typedef struct Piece{
    struct Piece* fd;
    struct Piece* bk;
    long start;
    long end;//included
    bool is_add;
    bool is_empty;
}Piece;

typedef struct PieceList{
    Piece* head;
    Piece* tail;
    long len;
    long piece_num;
}PieceList;

typedef struct PieceTable{
    String* original;
    String* add;
    PieceList* piece_list;
}PieceTable;



#endif