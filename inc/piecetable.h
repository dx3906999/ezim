#ifndef PIECETABLE_H
#define PIECETABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

// #define INSERT_AFTER(p,add) {(add)->fd=(p);(add)->bk=(p)->bk;if((p)->bk){(p)->bk->fd=(add);}(p)->bk=(add);}
// #define PUSHBACK_DL(dlist,p) {if((dlist)->tail){INSERT_AFTER((dlist)->tail,p);(dlist)->tail=(p);}else{(dlist)->head=(dlist)->tail=(p);(p)->fd=(p)->bk=NULL;}}


typedef Vector String;

typedef struct PieceEdge{
    long ver;
    struct PieceEdge* fdp_next_edge;
    struct PieceEdge* bkp_next_edge;
    struct Piece* next;
    struct Piece* prev;
    long next_index_in_all_nodes;
}PieceEdge;

typedef struct Piece{
    PieceEdge* bk_edges;
    PieceEdge* fd_edges;
    long start;
    long end;//included
    int line_break_start;
    int line_break_end;
    bool is_add;
    bool is_empty;
    bool is_inline;
    long index_in_all_nodes;
}Piece;

typedef struct PieceList{
    PieceEdge* head_edges;
    Piece* tail_piece;
    Vector* all_nodes;// to free the PieceList
}PieceList;

typedef struct PieceTable{
    String* original;
    String* add;
    PieceList* piece_list;
    Vector* original_linebreak;
    Vector* add_linebreak;
    // long now_ver;
    long ver_counter;
}PieceTable;

PieceList* newPieceList();
void freePieceListR(Vector* all_nodes,Piece* p);
void freePieceList(PieceList* pl);
PieceTable* newPieceTable(FILE* fp);
PieceEdge* newPieceEdge(Piece* fdp,Piece* bkp,long ver);
void freePieceTable(PieceTable* pt);
Piece* cutPiece(Piece* p, long first_len, PieceTable* piecetable);
Piece* findNextPiece(Piece* p, long ver);
Piece* findPrevPiece(Piece* p,long ver);
void deletePiece(Piece* p, PieceTable* piecetable, long ver);
// Piece* insertPiece(Piece* p,PieceTable* piecetable, long ver);
Piece* insertAfterPiece(Piece* p,PieceTable* piecetable, long ver);
Piece* insertBeforePiece(Piece* p,PieceTable* piecetable,long ver);
void printPieces(PieceTable* piecetable, FILE* fp, long ver);
Piece* findNextPieceByPE(PieceEdge* pe, long ver);
Piece* findPrevPieceByPE(PieceEdge* pe,long ver);

#endif