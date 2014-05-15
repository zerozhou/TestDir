/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "lzw.h"
#include "bitfile.h"

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/* node in dictionary tree */
typedef struct dict_node_t
{
    unsigned int codeWord;      /* code word for this entry */
    unsigned char suffixChar;   /* last char in encoded string */
    unsigned int prefixCode;    /* code for remaining chars in string */

    /* pointer to child nodes */
    struct dict_node_t *left;   /* child with < key */
    struct dict_node_t *right;  /* child with >= key */
} dict_node_t;

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

#define MIN_CODE_LEN    9                   /* min # bits in a code word */
#define MAX_CODE_LEN    20                  /* max # bits in a code word */

#define FIRST_CODE      (1 << CHAR_BIT)     /* value of 1st string code */

#define MAX_CODES       (1 << MAX_CODE_LEN)

#if (MIN_CODE_LEN <= CHAR_BIT)
#error Code words must be larger than 1 character
#endif

#if ((MAX_CODES - 1) > INT_MAX)
#error There cannot be more codes than can fit in an integer
#endif

/***************************************************************************
*                                  MACROS
***************************************************************************/
#define CODE_MS_BITS(BITS)      ((BITS) - CHAR_BIT)
#define MS_BITS_MASK(BITS)      (UCHAR_MAX << (CHAR_BIT - CODE_MS_BITS(BITS)))
#define CURRENT_MAX_CODES(BITS)     (1 << (BITS))


/* dictionary tree node create/free */
dict_node_t *MakeNode(unsigned int codeWord,
    unsigned int prefixCode, unsigned char suffixChar);
void FreeTree(dict_node_t *node);

/* searches tree for matching dictionary entry */
dict_node_t *FindDictionaryEntry(dict_node_t *root, int prefixCode,
    unsigned char c);

/* makes key from prefix code and character */
unsigned int MakeKey(unsigned int prefixCode, unsigned char suffixChar);

/* write encoded data */
void PutCodeWord(bit_file_t *bfpOut, int code, unsigned char codeLen);


/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : LZWEncodeFile
*   Description: This routine reads an input file 1 character at a time and
*                writes out an LZW encoded version of that file.
*   Parameters : inFile - Name of file to encode
*                outFile - Name of file to write encoded output to
*   Effects    : File is encoded using the LZW algorithm with CODE_LEN
*                codes.
*   Returned   : TRUE for success, otherwise FALSE.
***************************************************************************/
int LZWEncodeFile(const char *inFile, const char *outFile)
{
    FILE *fpIn;                         /* uncoded input */
    bit_file_t *bfpOut;                 /* encoded output */

    int code;                           /* code for current string */
    unsigned char currentCodeLen;       /* length of the current code */
    unsigned int nextCode;              /* next available code index */
    int c;                              /* character to add to string */

    dict_node_t *dictRoot;              /* root of dictionary tree */
    dict_node_t *node;                  /* node of dictionary tree */

    /* open input and output files */
    if (NULL == (fpIn = fopen(inFile, "rb")))
    {
        perror(inFile);
        return FALSE;
    }

    if (NULL == outFile)
    {
        bfpOut = MakeBitFile(stdout, BF_WRITE);
    }
    else
    {
        if (NULL == (bfpOut = BitFileOpen(outFile, BF_WRITE)))
        {
            fclose(fpIn);
            perror(outFile);
            return FALSE;
        }
    }

    /* initialize dictionary as empty */
    dictRoot = NULL;

    /* start with 9 bit code words */
    currentCodeLen = 9;

    nextCode = FIRST_CODE;  /* code for next (first) string 第一个字符的码字，并且FIRST_CODE=2^CHAR_BIT=2^8*/

    /* now start the actual encoding process */

    code = fgetc(fpIn);     /* start with code string = first character */

    /* create a tree root from 1st 2 character string */
    if ((c = fgetc(fpIn)) != EOF)
    {
        /* special case for NULL root */
        dictRoot = MakeNode(nextCode, code, c);
        nextCode++;

        /* write code for 1st char */
        PutCodeWord(bfpOut, code, currentCodeLen);

        /* new code is just 2nd char */
        code = c;
    }

    /* now encode normally */
    while ((c = fgetc(fpIn)) != EOF)
    {
        /* look for code + c in the dictionary */
        node = FindDictionaryEntry(dictRoot, code, c);

        if ((node->prefixCode == code) &&
            (node->suffixChar == c))
        {
            /* 如果code +c 在字典树中，则将前缀码用字典树中的码字表示，
             * 如AB的codeWord=265, 则ABA的entry=(265,A)*/
            code = node->codeWord;
        }
        else
        {
            /* 如果code + c不在字典表中，那么在有空间的情况下，将code+c添加到字典表中 */
            if (nextCode < MAX_CODES)   /*MAX_CODES = 2^currentCodeLen*/
            {
                dict_node_t *tmp;

                tmp = MakeNode(nextCode, code, c);
                nextCode++;

                if (MakeKey(code, c) <
                    MakeKey(node->prefixCode, node->suffixChar))
                {
                    node->left = tmp;
                }
                else
                {
                    node->right = tmp;
                }
            }
            else
            {
                fprintf(stderr, "Error: Dictionary Full\n");
            }

            /* 如果当前编码长度不够长的情况，需要增加编码长度，即 currentCodeLen + 1 */
            while ((code >= (CURRENT_MAX_CODES(currentCodeLen) - 1))/*2^currentCodeLen - 1*/ &&
                (currentCodeLen < MAX_CODE_LEN))
            {
                /* mark need for bigger code word with all ones */
                PutCodeWord(bfpOut, (CURRENT_MAX_CODES(currentCodeLen) - 1),
                    currentCodeLen);
                currentCodeLen++;
            }

            /* 将字符串的codeWord输出到bitfile中*/
            PutCodeWord(bfpOut, code, currentCodeLen);

            /* 获取下一个字符 */
            code = c;
        }
    }

    /* 到达文件末尾，将最后一个字符编码输出 */
    PutCodeWord(bfpOut, code, currentCodeLen);

    fclose(fpIn);
    BitFileClose(bfpOut);

    /* 释放字典树*/
    /*这里的字典树主要是利用字典树能够高校的获取字符的最长前缀码*/
    if (dictRoot != NULL)
    {
        FreeTree(dictRoot);
    }

    return TRUE;
}

/***************************************************************************
*   Function   : MakeKey
*   Description: This routine creates a simple key from a prefix code and
*                an appended character.  The key may be used to establish
*                an order when building/searching a dictionary tree.
*   Parameters : prefixCode - code for all but the last character of a
*                             string.
*                suffixChar - the last character of a string
*   Effects    : None
*   Returned   : Key built from string represented as a prefix + char.  Key
*                format is {ms nibble of c} + prefix + {ls nibble of c}
***************************************************************************/
unsigned int MakeKey(unsigned int prefixCode, unsigned char suffixChar)
{
    unsigned int key;

    /* position ms nibble */
    key = suffixChar & 0xF0;
    key <<= MAX_CODE_LEN;

    /* include prefix code */
    key |= (prefixCode << 4);

    /* inclulde ls nibble */
    key |= (suffixChar & 0x0F);

    return key;
}

/***************************************************************************
*   Function   : MakeNode
*   Description: This routine creates and initializes a dictionary entry
*                for a string and the code word that encodes it.
*   Parameters : codeWord - code word used to encode the string prefixCode +
*                           suffixChar
*                prefixCode - code for all but the last character of a
*                             string.
*                suffixChar - the last character of a string
*   Effects    : Node is allocated for new dictionary entry
*   Returned   : Pointer to newly allocated node
***************************************************************************/
dict_node_t *MakeNode(unsigned int codeWord,
    unsigned int prefixCode, unsigned char suffixChar)
{
    dict_node_t *node;

    node = malloc(sizeof(dict_node_t));

    if (NULL != node)
    {
        node->codeWord = codeWord;
        node->prefixCode = prefixCode;
        node->suffixChar = suffixChar;

        node->left = NULL;
        node->right = NULL;
    }
    else
    {
        perror("allocating dictionary node");
        exit(EXIT_FAILURE);
    }

    return node;
}

/***************************************************************************
*   Function   : FreeTree
*   Description: This routine will free all nodes of a tree rooted at the
*                node past as a parameter.
*   Parameters : node - root of tree to free
*   Effects    : frees allocated tree node from initial parameter down.
*   Returned   : none
***************************************************************************/
void FreeTree(dict_node_t *node)
{
    /* free left branch */
    if (node->left != NULL)
    {
        FreeTree(node->left);
    }

    /* free right branch */
    if (node->right != NULL)
    {
        FreeTree(node->right);
    }

    /* free root */
    free(node);
}

/***************************************************************************
*   Function   : FindDictionaryEntry
*   Description: This routine searches the dictionary tree for an entry
*                with a matching string (prefix code + suffix character).
*                If one isn't found, the parent node for that string is
*                returned.
*   Parameters : prefixCode - code for the prefix of string
*                c - last character in string
*   Effects    : None
*   Returned   : If string is in dictionary, pointer to node containing
*                string, otherwise pointer to suitable parent node.  NULL
*                is returned for an empty tree.
***************************************************************************/
dict_node_t *FindDictionaryEntry(dict_node_t *root, int prefixCode,
    unsigned char c)
{
    unsigned int searchKey, key;

    if (NULL == root)
    {
        return NULL;
    }

    searchKey = MakeKey(prefixCode, c);     /* key of string to find */

    while (1)
    {
        /* key of current node */
        key = MakeKey(root->prefixCode, root->suffixChar);

        if (key == searchKey)
        {
            /* current node contains string */
            return root;
        }
        else if (searchKey < key)
        {
            if (NULL != root->left)
            {
                /* check left branch for string */
                root = root->left;
            }
            else
            {
                /* string isn't in tree, it can be added as a left child */
                return root;
            }
        }
        else
        {
            if (NULL != root->right)
            {
                /* check right branch for string */
                root = root->right;
            }
            else
            {
                /* string isn't in tree, it can be added as a right child */
                return root;
            }
        }
    }
}

/***************************************************************************
*   Function   : PutCodeWord
*   Description: This function writes a code word to an encoded file.
*                In order to deal with endian issue the code word is
*                written least significant byte followed by the remaining
*                bits.
*   Parameters : bfpOut - bit file containing the encoded data
*                code - code word to add to the encoded data
*                codeLen - length of the code word
*   Effects    : code word is written to the encoded output
*   Returned   : None
***************************************************************************/
void PutCodeWord(bit_file_t *bfpOut, int code, unsigned char codeLen)
{
    BitFilePutBitsInt(bfpOut, &code, codeLen, sizeof(code));
}
