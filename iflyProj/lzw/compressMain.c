/*************************************************************************
	> File Name: main.c
	> Author: Zijian Zhou
	> Mail: zjzhou62@mail.ustc.edu.cn 
	> Created Time: Wed 14 May 2014 10:54:32 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "iFlyBuild.h"
int main(int argc, char *argv[])
{
    if ( argc !=3 )
    {
        printf("输入不正确，请输入三个参数，形如: ./main  songerA.txt  outPutRes.txt");
        exit(EXIT_FAILURE);
    }
    ProcessBuildRes(argv[1], argv[2]);

    return EXIT_SUCCESS;

}
