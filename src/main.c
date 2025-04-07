#include <stdio.h>
#include "display.h"

int main(int argc, char const *argv[])
{
    // int height,width;
    // get_terminal_size(&width,&height);
    // printf("%d %d\n",height,width);
    // printf("\x1b[6n\n");
    // fflush(stdout);
    // return 0;
    printf("按任意键显示，按q退出...\n");
    
    while (1) {
        if (kbhit()) {
            int c = getch();
            printf("你按下了: %c (ASCII: %d)\n", c, c);
            if (c == 'q') break;
        }
        
        // 这里可以执行其他任务
        // ..
    }
    
    return 0;
}
