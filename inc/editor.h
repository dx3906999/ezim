#ifndef EDITOR_H
#define EDITOR_H

#include "piecetable.h"
#include "terminal.h"

#define M_INSERT 0
#define M_NORMAL 1
#define M_COMMAND 2

#define COMMAND_SAVE 0x1
#define COMMAND_QUIT 0x2

typedef struct PosInPiece{
    Piece* piece;
    long index_in_piece;
}PosInPiece;

typedef struct Editor{
    volatile int editor_mode;
    PosInPiece* pos_in_piece;
    PieceTable* piecetable;
    FILE* fp;
    const char* filename;
    long now_ver;
}Editor;



Editor* newEditor(FILE* fp);
void freeEditor(Editor* editor);
void updateEditorMode(volatile int* current_mode, char input_c);
void findNextChar(PosInPiece* pos, long ver, int direction, PieceList* pl);
void findNextLine(PosInPiece* pos, long ver, int direction, PieceTable* pt);
void inNormalMode(char input_c, Editor* editor);
void inInsertModeBackend(char input_c, Editor* editor);
void inCommamdMode(char input_c, Editor* editor, struct winsize* size);


#endif