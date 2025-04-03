#include <stdio.h>
#include "display.h"

int main(int argc, char const *argv[])
{
    int height,width;
    get_terminal_size(&width,&height);
    printf("%d %d\n",height,width);
    printf("\x1b[6n\n");
    fflush(stdout);
    return 0;
}
