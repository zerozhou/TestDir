/*************************************************************************
	> File Name: charinit.c
	> Author: Zijian Zhou
	> Mail: zjzhou62@mail.ustc.edu.cn 
	> Created Time: Wed 14 May 2014 12:11:32 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
int main()
{
    const char *str = "01010011";
    char  buffer=0;
    int i;
    int len = strlen(str);

    for ( i=0; i<len; ++i )
    {
        int tmp =1;
        if(str[i]=='1')
        {
            buffer += (tmp << len-i-1);
        }
    }
    printf("buffer = %d--%c\n", buffer,buffer);
    return 0;

}
