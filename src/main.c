#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "terminal.h"
#include "utils.h"



int main(int argc, char const *argv[])
{
    // enableRawMode();

    // printf("Raw mode ON (Press 'q' to quit)...\r\n");

    // char c;
    // while (1) {
    //     read(STDIN_FILENO, &c, 1);  // 逐字符读取输入
    //     if (c == 'q') break;
    //     printf("You pressed: %d \r\n", c);  // 显示按下的键值
    // }
    Vector* v=newVector(sizeof(int));
    GETV(v,int,0)=100;
    printf("%d\n",GETV(v,int,0));




    return 0;
}
