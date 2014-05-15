#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lzw.h"

/*将文件解压到本地，这里使用的是data.txt*/
int Initialize(const char *szResPath)
{
    const char *outfile = "data.txt";
    LZWDecodeFile(szResPath,outfile);
    return 1;

}

char * fgetword(char *s, FILE *stream)
{
    register int c;
    register char *cs;
    cs =s;
    while((c=getc(stream))!=EOF)
    {
        if('\n' == c||' ' == c || '\t' == c)
            break;
        *cs++=c;
    }
    *cs = '\0';
    return (c == EOF && cs==s)?NULL:s;

}

int ProcessSelect( const char *szSingerName, char *szResultString)
{
    FILE *fpFile;
    char tmpWord[100];

    fpFile = fopen ("data.txt", "rb" );
    if (NULL==fpFile)  
    {  
        fputs ("File error",stderr);  
        return EXIT_FAILURE;  
    }

    fgetword(tmpWord,fpFile);
    /*问题出在这里，使用fgetword获取了name，但是打印到终端确实乱码，更奇怪的是写入123.txt文件却能够正确显示中文*/
    printf("tmpWord = %s\n", tmpWord);
    FILE *fp2 = fopen("123.txt", "wb");
    fputs(tmpWord, fp2);

    while(!feof(fpFile))
    {
        if (strcmp(tmpWord, szSingerName) == 0)
        {    
            fgetword(tmpWord, fpFile);
            strcat (szResultString, szSingerName);
            strcat (szResultString, tmpWord);
        }
        fgetword(tmpWord,fpFile);
    }   
    free (fpFile);
    free(fp2);
    return EXIT_SUCCESS;
}

int unInitialize()
{
    
}

