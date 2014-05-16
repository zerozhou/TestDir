#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lzw.h"


/*将文件解压到本地，这里使用的是data.txt*/
int Initialize(const char *szResPath)
{

    int flags;
    const char *outfile = "data.txt";
    flags = LZWDecodeFile(szResPath,outfile);
    if ( FALSE == flags )
    {
        printf("加载失败！\n");
        return FALSE;
    }
    return TRUE;

}

int ProcessSelect(const char *szSingerName, char *szResultString)
{
    FILE *fpFile = NULL;
    char tmpName[100];
    char nameHot[100];

    fpFile = fopen ("data.txt", "rb" );
    if (NULL==fpFile)  
    {  
        fputs ("File error",stderr);  
        return EXIT_FAILURE;  
    }

    strcat(szResultString,"{");
    fscanf (fpFile, "%s", tmpName);
    while(!feof(fpFile))
    {
        if (strcmp(tmpName, szSingerName) == 0)
        {
            fscanf(fpFile, "%s", nameHot);
            strcat(szResultString,"[");
            strcat(szResultString,tmpName);
            strcat(szResultString," , ");
            strcat(szResultString,nameHot);
            strcat(szResultString,"], ");
        }
        fscanf (fpFile, "%s",tmpName);
    }
    
    int len = strlen(szResultString);
    szResultString[len-2] = '}';
    szResultString[len-1] = '\0';

    fclose (fpFile);
    /*free(fp2);*/
    return 1;
}

int unInitialize()
{
    
}

