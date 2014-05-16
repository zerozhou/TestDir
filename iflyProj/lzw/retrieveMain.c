/*************************************************************************
	> File Name: retrieveMain.c
	> Author: Zijian Zhou
	> Mail: zjzhou62@mail.ustc.edu.cn 
	> Created Time: Thu 15 May 2014 02:04:39 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "iFlySelect.h"

#define MAX_LEN 300

int main(int argc, char *argv[])
{

    char ResultString[MAX_LEN]; 
    /*const  char singerName[20];*/
    int rsflag;


    if ( 3 != argc )
    {
        fputs ("参数不正确",stderr);
        exit(EXIT_FAILURE);
    }

    rsflag = Initialize(argv[1]);
    if (0 == rsflag)
    {
        printf("资源加载失败！");
        exit (EXIT_FAILURE);
    }
    rsflag = ProcessSelect(argv[2], ResultString);

    if( rsflag < 0 )
    {
        printf("检索失败！\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("检索结果：\t %s\n", ResultString);
        exit (EXIT_SUCCESS);
        
    }
}
