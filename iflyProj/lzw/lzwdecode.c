#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "lzw.h"
#include "bitfile.h"

typedef struct
{
    unsigned char suffixChar;   /* last char in encoded string */
    unsigned int prefixCode;    /* code for remaining chars in string */
} decode_dictionary_t;

/***************************************************************************
*                                常量定义
***************************************************************************/

#define EMPTY           -1

#define MIN_CODE_LEN    9                   /* min # bits in a code word */
#define MAX_CODE_LEN    20                  /* max # bits in a code word */

#define FIRST_CODE      (1 << CHAR_BIT)     /* value of 1st string code */

#define MAX_CODES       (1 << MAX_CODE_LEN)

#define DICT_SIZE       (MAX_CODES - FIRST_CODE)

#if (MIN_CODE_LEN <= CHAR_BIT)
#error Code words must be larger than 1 character
#endif

#if ((MAX_CODES - 1) > INT_MAX)
#error There cannot be more codes than can fit in an integer
#endif

/***************************************************************************
*                                  宏定义
***************************************************************************/
#define CODE_MS_BITS(BITS)      ((BITS) - CHAR_BIT)
#define MS_BITS_MASK(BITS)      (UCHAR_MAX << (CHAR_BIT - CODE_MS_BITS(BITS)))
#define CURRENT_MAX_CODES(BITS)     (1 << (BITS))


/* 字典中字符串的码字就是字典的索引index */
decode_dictionary_t dictionary[DICT_SIZE];

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
unsigned char DecodeRecursive(unsigned int code, FILE *fpOut);

/* read encoded data */
int GetCodeWord(bit_file_t *bfpIn, unsigned char codeLen);


/***************************************************************************
*   Function   : LZWDecodeFile
*   Description: This routine reads an input file 1 encoded string at a
*                time and decodes it using the LZW algorithm.
*   Parameters : inFile - Name of file to decode
*                outFile - Name of file to write decoded output to
*   Effects    : File is decoded using the LZW algorithm with CODE_LEN
*                codes.
*   Returned   : TRUE for success, otherwise FALSE.
***************************************************************************/
int LZWDecodeFile(const char *inFile, const char *outFile)
{
    bit_file_t *bfpIn;                  /* encoded input */
    FILE *fpOut;                        /* decoded output */

    unsigned int nextCode;              /* value of next code */
    unsigned int lastCode;              /* last decoded code word */
    unsigned int code;                  /* code word to decode */
    unsigned char currentCodeLen;       /* length of code words now */
    unsigned char c;                    /* last decoded character */

    /* open input and output files */
    if (NULL == (bfpIn = BitFileOpen(inFile, BF_READ)))
    {
        perror(inFile);
        return FALSE;
    }

    if (NULL == outFile)
    {
        fpOut = stdout;
    }
    else
    {
        if (NULL == (fpOut = fopen(outFile, "w+b")))/*wb*/
        {
            BitFileClose(bfpIn);
            perror(outFile);
            return FALSE;
        }
    }

    /* start with 9 bit code words */
    currentCodeLen = 9;

    /* initialize for decoding */
    nextCode = FIRST_CODE;  /* code for next (first) string */

    /* first code from file must be a character.  use it for initial values */
    lastCode = GetCodeWord(bfpIn, currentCodeLen);
    c = lastCode;
    fputc(lastCode, fpOut);

    /* decode rest of file */
    while ((code = GetCodeWord(bfpIn, currentCodeLen)) != EOF)
    {

        /* look for code length increase marker */
        while (((CURRENT_MAX_CODES(currentCodeLen) - 1) == code) &&
            (currentCodeLen < MAX_CODE_LEN))
        {
            currentCodeLen++;
            code = GetCodeWord(bfpIn, currentCodeLen);
        }

        if (code < nextCode)
        {
            /* we have a known code.  decode it */
            c = DecodeRecursive(code, fpOut);
        }
        else
        {
            /***************************************************************
            * We got a code that's not in our dictionary.  This must be due
            * to the string + char + string + char + string exception.
            * Build the decoded string using the last character + the
            * string from the last code.
            ***************************************************************/
            unsigned char tmp;

            tmp = c;
            c = DecodeRecursive(lastCode, fpOut);
            /*printf("tmp=%c",tmp);*/
            fputc(tmp, fpOut);
        }

        /* if room, add new code to the dictionary */
        if (nextCode < MAX_CODES)
        {
            dictionary[nextCode - FIRST_CODE].prefixCode = lastCode;
            dictionary[nextCode - FIRST_CODE].suffixChar = c;
            nextCode++;
        }

        /* save character and code for use in unknown code word case */
        lastCode = code;
    }

    fclose(fpOut);
    BitFileClose(bfpIn);
    return TRUE;
}

/***************************************************************************
*   Function   : DecodeRecursive
*   Description: This function uses the dictionary to decode a code word
*                into the string it represents and write it to the output
*                file.  The string is actually built in reverse order and
*                recursion is used to write it out in the correct order.
*   Parameters : code - the code word to decode
*                fpOut - the file that the decoded code word is written to
*   Effects    : Decoded code word is written to a file
*   Returned   : The first character in the decoded string
***************************************************************************/
unsigned char DecodeRecursive(unsigned int code, FILE *fpOut)
{
    unsigned char c;
    unsigned char firstChar;

    if (code >= FIRST_CODE)
    {
        /* code word is string + c */
        c = dictionary[code - FIRST_CODE].suffixChar;
        code = dictionary[code - FIRST_CODE].prefixCode;

        /* evaluate new code word for remaining string */
        firstChar = DecodeRecursive(code, fpOut);
    }
    else
    {
        /* code word is just c */
        c = code;
        firstChar = code;
    }

    fputc(c, fpOut);
    return firstChar;
}

/***************************************************************************
*   Function   : GetCodeWord
*   Description: This function reads and returns a code word from an
*                encoded file.  In order to deal with endian issue the
*                code word is read least significant byte followed by the
*                remaining bits.
*   Parameters : bfpIn - bit file containing the encoded data
*                codeLen - number of bits in code word
*   Effects    : code word is read from encoded input
*   Returned   : The next code word in the encoded file.  EOF if the end
*                of file has been reached.
*
*   NOTE: If the code word contains more than 16 bits, this routine should
*         be modified to read in all the bytes from least significant to
*         most significant followed by any left over bits.
***************************************************************************/
int GetCodeWord(bit_file_t *bfpIn, unsigned char codeLen)
{
    int code = 0;
    int count;

    count = BitFileGetBitsInt(bfpIn, &code, codeLen, sizeof(code));
    /*printf("code = %d\n", code);*/
    /*printf("count=%d---codelen=%d", count, codeLen);noramlly, count is equal with codeLen, if reaching EOF of file, count = -1*/

    if (count < codeLen)
    {
        code = EOF;     /* BitFileGetBitsInt gives partial results at EOF */
    }

    return code;
}
