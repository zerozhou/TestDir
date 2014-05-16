#ifndef _LZW_H_
#define _LZW_H_

/***************************************************************************
*                                常量设置
***************************************************************************/
#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

/***************************************************************************
*                               主要的两个函数的声明
***************************************************************************/
 /* encode inFile */
int LZWEncodeFile(const char *inFile, const char *outFile);

/* decode inFile*/
int LZWDecodeFile(const char *inFile, const char *outFile);

#endif  /* ndef _LZW_H_ */
