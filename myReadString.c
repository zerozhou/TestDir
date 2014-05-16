/*************************************************************************
	> File Name: fword.c
	> Author: Zijian Zhou
	> Mail: zjzhou62@mail.ustc.edu.cn
	> Created Time: Thu 15 May 2014 03:54:03 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define true 1
#define false 0
char *myReadString (char *s, FILE *stream)
{
    register char  c;
    register int  flag;
    register char *cs;
    cs = s;
    /*fseek(stream, 0, 0); */  /*设置文件指针位置*/
    c = getc(stream);
    flag = false;
    while(c!=EOF)
    {
        
        if (('\n' != c) && (' ' != c) && ('\t' != c))
        {
            *cs++ = c;
            flag = true;
            printf("读取中----%c---\n",c);
        }
        else if (flag == true)
        {
            printf("终止读取\n");
            break;
        }
        c = getc(stream);
    }
    *cs = '\0';
    return s;
}

int main()
{
    FILE *fp, *fp1;
    char *tmpString = NULL;
    tmpString = (char *)malloc(100);

    fp = fopen("data", "rb");
    myReadString (tmpString, fp);
    printf("\n=====================\n");
    printf("第一个字符串---tmpString =%s##\n", tmpString);
    
    myReadString (tmpString, fp);
    printf("\n第二个字符串---tmpString =%s##\n", tmpString);
    
    myReadString (tmpString, fp);
    printf("\n第三个字符串---tmpString =%s##\n", tmpString);
    
    myReadString (tmpString, fp);
    printf("\n第四个字符串---tmpString =%s##\n", tmpString);
   
    free(tmpString);
    tmpString = NULL;
    fclose(fp);
    return 0;


}
