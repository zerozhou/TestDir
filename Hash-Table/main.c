#include<stdio.h>
#include "hashtable.h"
#include <string.h>
#include <stdlib.h>
int main()
{
    char *keyTagID;
    int value;
    FILE *fpIn;

    keyTagID = (char *)malloc(sizeof(char)*44);
    fpIn = fopen("tagID.txt","rb");
    hash_table_t *table = hash_table_new(MODE_COPY);

    fscanf (fpIn, "%s", keyTagID);
    fscanf (fpIn, "%d", &value);

    while (!feof(fpIn))
    {
        hash_table_add(table, keyTagID, strlen(keyTagID), (void *) &value, sizeof(int));
        fscanf (fpIn, "%s", keyTagID);
        fscanf (fpIn, "%d", &value);
        printf("------------------------------------------\n");
        printf("add the keyTagID = %s, and the value = %d\n", keyTagID, value);
    }
    //hash_table_add(table, keyTagID, strlen(keyTagID), (void *) &value, sizeof(int));
/*
    if (hash_table_has_key(table, keyTagID, key_len))
    {
        printf("Key found %s\n", keyTagID);
    }
    else
    {
        printf("Key NOT found %s\n", keyTagID);
    }
*/
    keyTagID = "BB0222000E3000E2002075600F0078156074447EAA68";
    //keyTagID = "BB0222000E3000E2002075600F0073156074307EAD57";
    int *valueTmp = NULL;
    valueTmp = (int* )hash_table_lookup(table, keyTagID,strlen(keyTagID));
    printf("find the keyTagID = %s, and the value = %d\n", keyTagID, *valueTmp);
/*
    int i = 1;
    int val = 100;
    int val2 = 200;
    int j = 2;
    int x =0;
    for (x=0;x<7000;x++)
    {
        // use the macro
        HT_ADD(table, &j, &val);
        // or use the function
        //hash_table_add(table, &j, i, (void *) &val, sizeof(int));
        val++;
        j++;
    }
    hash_table_add(table, &j, i, (void *) &val2, 1);
    j--; j--;
    printf("test j = %d\n",j);
    hash_table_remove(table, &j, i);
    HT_REMOVE(table, &j);
    if (hash_table_has_key(table, &j, i))
    {
        printf("Key found %d\n", j);
    }
    else
    {
        printf("Key NOT found %d\n", j);
    }
    val = -100;
    val2 = -200;
    j--;
    int *value = NULL;
    value = (int* ) HT_LOOKUP(table, &j);
    void** keys = NULL;
    size_t num = hash_table_get_keys(table, keys);
    printf("found %d keys\n", (int)num);
    printf("j -> %d \n", j);
    if (value)
        printf("value is %d\n", *value);
    else
        printf("*value is %p\n", value);
*/
    return 0;
}
