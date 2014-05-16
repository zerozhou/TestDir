#ifndef _BITFILE_H_
#define _BITFILE_H_

#include <stdio.h>

typedef enum
{
    BF_READ = 0,
    BF_WRITE = 1,
    BF_APPEND= 2,
    BF_NO_MODE
} BF_MODES;

/* incomplete type to hide implementation */
struct bit_file_t;
typedef struct bit_file_t bit_file_t;

/***************************************************************************
*               文件位操作函数，是对压缩后的文件的处理
***************************************************************************/

/* open/close file */
bit_file_t *BitFileOpen(const char *fileName, const BF_MODES mode);
bit_file_t *MakeBitFile(FILE *stream, const BF_MODES mode);
int BitFileClose(bit_file_t *stream);
FILE *BitFileToFILE(bit_file_t *stream);

/* toss spare bits and byte align file */
int BitFileByteAlign(bit_file_t *stream);

/* fill byte with ones or zeros and write out results */
int BitFileFlushOutput(bit_file_t *stream, const unsigned char onesFill);

/* get/put character -------1 byte or 8 bits*/
int BitFileGetChar(bit_file_t *stream);
int BitFilePutChar(const int c, bit_file_t *stream);

/* get/put single bit ------1 bit*/
int BitFileGetBit(bit_file_t *stream);
int BitFilePutBit(const int c, bit_file_t *stream);

/* get/put number of bits (most significant bit to least significat bit) ------- n bits*/
int BitFileGetBits(bit_file_t *stream, void *bits, const unsigned int count);
int BitFilePutBits(bit_file_t *stream, void *bits, const unsigned int count);

/***************************************************************************
* get/put number of bits from integer types (short, int, long, ...)
* machine endiness is accounted for.
* size is the size of the data structure pointer to by bits.
***************************************************************************/
int BitFileGetBitsInt(bit_file_t *stream, void *bits, const unsigned int count,    /*4 bytes or 32 bits*/
    const size_t size);
int BitFilePutBitsInt(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size);

#endif /* _BITFILE_H_ */
