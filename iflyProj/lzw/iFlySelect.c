#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lzw.h"

/*���ļ���ѹ�����أ�����ʹ�õ���data.txt*/
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
    /*����������ʹ��fgetword��ȡ��name�����Ǵ�ӡ���ն�ȷʵ���룬����ֵ���д��123.txt�ļ�ȴ�ܹ���ȷ��ʾ����*/
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

