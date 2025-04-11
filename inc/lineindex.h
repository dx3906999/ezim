#ifndef LINEINDEX_H
#define LINEINDEX_H
#include "piecetable.h"
#include "utils.h"

typedef struct LineIndex
{
    Piece* piece;
    int offset;
}LineIndex;

typedef struct LIManager{
    Vector li_vector;
    
}LIManager;



#endif