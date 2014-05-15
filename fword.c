/*************************************************************************
	> File Name: fword.c
	> Author: Zijian Zhou
	> Mail: zjzhou62@mail.ustc.edu.cn 
	> Created Time: Thu 15 May 2014 03:54:03 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char *fword (char *s, FILE *stream)
{
    register char  c, tmp;
    register char *cs;
    cs = s;
    c = getc(stream);
    tmp = getc(stream);
    while(c!=EOF)
    {
        //tmp = getc(stream);
        /*
        if('\n' == c ||' '  == c ||'\t' == c)
            break;
        *cs++ =c;
        */
        //printf("0---c = %c\n", c);

        if (('\n' != c || ' ' != c ||'\t' != c)&& ('\n' == tmp ||' ' == tmp ||'\t' == tmp))/*终止条件*/
        {
            *cs++ = c;
            printf("1----cs =%s, c = %c\n", cs, c);
            break;
        }

        else if (('\n'== c || ' ' ==c ||'\t'==c)&&('\n' == tmp ||' ' == tmp ||'\t' == tmp))/*跳过开头的空白字符*/
        {
            c = tmp;
            tmp = getc(stream);
            continue;
        }
        else if (('\n'== c || ' ' ==c ||'\t'==c)&&('\n' != tmp ||' '  != tmp ||'\t' != tmp))/*开始读取字符串*/
        {
            c = tmp;
            tmp = getc(stream);
            
            //printf("cs =%s", cs);
            //*cs++ = c;
            printf("2---cs =%s, c = %c\n", cs, c);
        }
        else  /*('\n'!= c || ' ' !=c ||'\t'!=c)&&('\n' != tmp ||' '  != tmp ||'\t' != tmp)*/
        {
            *cs++ = c;
            c = tmp;
            tmp = getc(stream);
            
            printf("3---cs =%s, c = %c\n", cs, c);
        }
    }
    *cs = '\0';
    return s;
}

int main()
{
    FILE *fp, *fp1;
    char *tmp;
    
    char *test = "何炅";



    printf("1==== %x---sizeof(test)= %d\n", *test, strlen(test));
    fp = fopen("data.txt", "r");
    fp1 = fopen("1.txt", "w"); 
    tmp =(char *)malloc(100);

    fscanf(fp,"%s", tmp);
    printf("\n2==== %x---sizeof(tmp) = %d\n", *(tmp+0), strlen(tmp));
    fputs(tmp, fp1);
    //int a =strcmp(tmp, "何炅");
    //printf("a = %d\n", a);
/*
    fword(tmp,fp);
    fputs(tmp,fp1);
    printf("tmp = %s\n", tmp);
    printf("\n-------\n");
    fword(tmp,fp);
    fputs(tmp,fp1);
    fputs("\n",fp1);

    printf("tmp = %s\n", tmp);
*/
    free(tmp);
    return 0;


}
