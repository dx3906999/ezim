#ifndef PIECETABLE_H
#define PIECETABLE_H

#include "strings.h"

typedef struct Piece{
    struct Piece* fd;
    struct Piece* bk;
    StrChunk* head_chunk;
    StrChunk* tail_chunk;
    int h_index;
    int t_index;// included
    long len;
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
    PieceList* piece_list;// with a final piece
    Piece* now_piece;// in the piece_list
    long now_index;
}PieceTable;



#endif