#include <string.h>
#include "lzw.h"

int Initialize()
{
    

}

int ProcessBuildRes(const char *szListPath, const char *szResPath)
{
    return LZWEncodeFile(szListPath, szResPath);
}

int unInitialize()
{
    
}

