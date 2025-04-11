#include <stdio.h>
#include "editor.h"
#include "terminal.h"

int editor_mode=0;

void updateEditorMode(int* current_mode, char input_c){
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
    default:
        break;
    }
}


