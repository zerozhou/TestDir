#include "hashtable.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>  /*this is a test*/

/*1-----元素操作-------------*/
/*申请一个哈希元素的内存空间*/
hash_table_element_t * hash_table_element_new()
{
    INFO("creating a new hash table element");
    return calloc(1, hash_table_element_s);
}

/**
 * 删除哈希表中的元素
 * @param table table from which element has to be deleted
 * @param element hash table element to be deleted
 */
void hash_table_element_delete(hash_table_t * table, hash_table_element_t * element)
{
    INFO("Deleting an hash table element");
    if (table->mode == MODE_COPY)  /*MODE_COPY = 0, MODE_VALUEREF = 1, MODE_ALLREF = 2*/
    {
        free(element->value);
        free(element->key);
    }
    else if (table->mode == MODE_VALUEREF)
    {
        free(element->key);
    }
    free(element);
}

/*2 ----- 哈希表进行操作-----------------*/
/**
 * Fuction 创建一个新的hash table
 * @param mode      hash_table_mode which the hash table should follow
 * @returns hash_table_t    object which references the hash table
 * @returns NULL when no memory
 */
hash_table_t * hash_table_new(hash_table_mode_t mode)
{
    INFO("Creating a new hash table");
    hash_table_t *table = calloc(1, hash_table_s);
    if (!table) 
    {
        INFO("No Memory while allocating hash_table");
        return NULL;
    }
    table->mode = mode;
    table->key_num = 128;
    table->key_ratio = 4;
    //table->store_house = (hash_table_element_t **) calloc(table->key_num, sizeof(int));  /*这里只需要申请能够存放指针数组的空间即可，由于是32位机器，所以一个地址占用4个字节*/
    table->store_house = (hash_table_element_t **) calloc(table->key_num, sizeof(hash_table_element_t *));
    if (!table->store_house)
    {
        INFO("No Memory while allocating hash_table store house");
        free(table);
        return NULL;
    }
    return table;
}

/**
 * Function 删除哈希表，注意这里的hash_table 和 hash_table_element_t是单独创建的，
 * 所以删除的时候需要遍历hash_table，将hash_table_element_t逐个删除，最后在删除hash_table
 */
void hash_table_delete(hash_table_t * table)
{
    INFO("Deleating a hash table");
    size_t i=0;
    for (;i<HASH_LEN;i++)
    {
        while (table->store_house[i])
        {
            hash_table_element_t * temp = table->store_house[i];
            table->store_house[i] = table->store_house[i]->next;
            hash_table_element_delete(table, temp);
        }
    }
    free(table->store_house);
    free(table);
}

/**
 * Function to add a key - value pair to the hash table, use HT_ADD macro
 * @param table hash table to add element to
 * @param key pointer to the key for the hash table
 * @param key_len length of the key in bytes
 * @param value pointer to the value to be added against the key
 * @param value_len length of the value in bytes
 * @returns 0 on sucess
 * @returns -1 when no memory
 */
int hash_table_add(hash_table_t * table, void * key, size_t key_len, void * value, size_t value_len)
{
    
    if ((table->key_count / table->key_num) >= table->key_ratio)/*与设定的载荷因子进行比较，超过载荷因子，需要将has_table变大*/
    {
        LOG("Ratio(%d) reached the set limit %d\nExpanding hash_table", (table->key_count / table->key_num), table->key_ratio);
        hash_table_resize(table, table->key_num*2);
        //exit(0);
    }
    /*size_t hash = HASH(key, key_len);*/
    uint32_t hash = HASH(key, key_len);
    printf("hash = %u\n",hash);
    hash_table_element_t * element = hash_table_element_new();
    if (!element)
    {
        INFO("Cannot allocate memory for element");
        return -1; // No Memory
    }
    if (table->mode == MODE_COPY)
    {
        LOG("Adding a key-value pair to the hash table with hash -> %d, in COPY MODE", (int)hash);
        element->key = malloc(key_len);
        element->value = malloc(value_len);
        if (element->key && element->value)
        {
            memcpy(element->key, key, key_len);    /*从源key所指的内存地址的起始位置开始，复制key_len的字节到内存element->key*/
            memcpy(element->value, value, value_len);
        }
        else
        {
            if (element->key)
            {
                free(element->key);
                INFO("Cannot allocate memory for value");
            }
            if (element->value)
            {
                free(element->value);
                INFO("Cannot allocate memory for key");
            }
            free(element);
            return -1; //No Memory
        }
    }
    else if (table->mode == MODE_VALUEREF)
    {
        LOG("Adding a key-value pair to the hash table with hash -> %d, in VALUEREF MODE", (int)hash);
        element->key = malloc(key_len);
        if (element->key)
        {
            memcpy(element->key, key, key_len);
        }
        else
        {
            INFO("Cannot allocate memory for key");
            free(element);
            return -1; //No Memory
        }
        element->value = value;
    }
    else if (table->mode == MODE_ALLREF)
    {
        LOG("Adding a key-value pair to the hash table with hash -> %d, in ALLREF MODE", (int)hash);
        element->key = key;
        element->value = value;
    }
    element->key_len = key_len;
    element->value_len = value_len;
    element->next = NULL;
    /*使用hash值作为store_house的index，找到插入位置*/
    if (!table->store_house[hash])  /*插入位置是空值，说明可以插入*/
    {
        LOG("No Conflicts adding the first element at %d", (int)hash);
        table->store_house[hash] = element;
        table->key_count++;

    }
    else
    {
        LOG("Conflicts adding element at %d", (int)hash);  
        /*插入位置非空，说明该位置已经有数据，即遇到hash值相同的情况，使用链表进行冲突处理*/
        hash_table_element_t * temp = table->store_house[hash];
        while(temp->next)/*如果相同的hash值位置不仅仅有一个元素，则temp->next！= NULL*/
        {
            while(temp->next && temp->next->key_len!=key_len)
            {
                temp = temp->next;
            }
            if(temp->next)/*相同的hash值，有两种情况:key 相同，key不同*/
            {
                if (!memcmp(temp->next->key, key, key_len))  /*key相同*/
                {
                    //LOG("Found Key at hash -> %d\n", (int)hash);
                    printf("Found Key at hash -> %u\n", (unsigned int)hash);
                    hash_table_element_t *to_delete = temp->next;
                    temp->next = element;
                    element->next = to_delete->next;
                    hash_table_element_delete(table, to_delete);
                    // since we are replacing values no need to change key_count
                    return 0;
                }
                else/*key不同，直接将元素挂在末尾m*/
                {
                    temp = temp->next;
                }
            }
        }
        /*如果相同的hash值位置仅仅只有一个元素，即temp->next == NULL*/
        temp->next = element;
        table->key_count++;
    }
    return 0;
}

/**
 * 函数功能是从hash_table中移除给定的元素
 * @param table hash table from which element has to be removed
 * @param key pointer to the key which has to be removed
 * @param key_len size of the key in bytes
 * @returns 0 on sucess
 * @returns -1 when key is not found
 */
int hash_table_remove(hash_table_t * table, void * key, size_t key_len)
{
    INFO("Deleting a key-value pair from the hash table");
    if ((table->key_num/ table->key_count) >= table->key_ratio)
    {
        LOG("Ratio(%d) reached the set limit %d\nContracting hash_table", (table->key_num / table->key_count), table->key_ratio);
        hash_table_resize(table, table->key_num/2);
        //exit(0);
    }
    /*size_t hash = HASH(key, key_len);*/
    uint32_t hash = HASH(key, key_len);
    if (!table->store_house[hash])
    {
        LOG("Key Not Found -> No element at %d", (int)hash);
        return -1; // key not found
    }
    hash_table_element_t *temp = table->store_house[hash];
    hash_table_element_t *prev = temp;
    while(temp)
    {
        while(temp && temp->key_len!=key_len)
        {
            prev = temp;
            temp = temp->next;
        }
        if(temp)
        {
            if (!memcmp(temp->key, key, key_len))
            {
                if (prev == table->store_house[hash])
                {
                    table->store_house[hash] = temp->next;
                }
                else
                {
                    prev->next = temp->next;
                }
                hash_table_element_delete(table, temp);
                INFO("Deleted a key-value pair from the hash table");
                table->key_count--;                
                return 0;
            }
            prev=temp;
            temp=temp->next;
        }
    }
    INFO("Key Not Found");
    return -1; // key not found
}

/**
 * 函数功能是在hash_table中查找给定的key
 * @param table table to look key in
 * @param key pointer to key to be looked for
 * @param key_len size of the key to be searched
 * @returns NULL when key is not found in the hash table
 * @returns void* pointer to the value in the table
 */
void * hash_table_lookup(hash_table_t * table, void * key, size_t key_len)
{
    uint32_t hash = HASH(key, key_len);
    LOG("Looking up a key-value pair for hash -> %d", (int)hash);
    if (!table->store_house[hash])
    {
        LOG("Key not found at hash %d, no entries", (int)hash);
        return NULL; // key not found
    }
    hash_table_element_t *temp = table->store_house[hash];
    while(temp)
    {
        while(temp && temp->key_len!=key_len)
        {
            temp = temp->next;
        }
        if(temp)
        {
            if (!memcmp(temp->key, key, key_len))
            {
                LOG("Found Key at hash -> %d", (int)hash);
                /*printf("temp->value = %d", *(int *)temp->value);  *this is a test*/
                return temp->value;
            }
            else
            {
                temp = temp->next;
            }
        }
    }
    LOG("Key not found at hash %d", (int)hash);
    return NULL; // key not found   
}

/**
 * 函数功能是判断一个key在hash_table中是否存在
 * @param key pointer to key to be looked for
 * @param key_len size of the key to be searched
 * @returns 0 when key is not found
 * @returns 1 when key is found
 */
int hash_table_has_key(hash_table_t * table, void * key, size_t key_len)
{
    /*size_t hash = HASH(key, key_len);*/
    uint32_t hash = HASH(key, key_len);
    LOG("Searching for key with hash -> %d", (int)hash);
    if (!table->store_house[hash])
    {
        LOG("Key not found with hash -> %d, no entries", (int)hash);
        return 0; // key not found
    }
    hash_table_element_t *temp = table->store_house[hash];
    while(temp)
    {
        while(temp && temp->key_len!=key_len)
        {
            temp = temp->next;
        }
        if(temp)
        {
            if (!memcmp(temp->key, key, key_len))
            {
                LOG("Key Found with hash -> %d", (int)hash);
                return 1; // key found
            }
            temp=temp->next;
        }
    }
    LOG("Key not found with hash -> %d", (int)hash);
    return 0; // key not found   
}

/**
 * 函数功能是返回hash_table中的所有keys
 * @param table hash table from which key are to be reterived
 * @param keys a void** pointer where keys are filled in (memory allocated internally and must be freed)
 * @return total number of keys filled in keys 
 */
size_t hash_table_get_keys(hash_table_t * table, void ** keys)
{
    size_t i = 0;
    size_t count = 0;
    keys = calloc(table->key_count, sizeof(void *));
    for(i=0;i<HASH_LEN;i++)
    {
        if (table->store_house[i])
        {
            keys[count++] = table->store_house[i];
            hash_table_element_t *temp = table->store_house[i];
            #ifdef DEBUG
            size_t num = 1;
            #endif
            while(temp->next)
            {
                keys[count++] = temp->next;
                temp = temp->next;
                #ifdef DEBUG
                num++;
                #endif
            }
            #ifdef DEBUG
            LOG("found %d key(s) at hash -> %d", (int)num, (int)i);
            #endif 
        }
    }
    return count;
}

/**
 * 函数功能是从hash_table中返回所有元素（key, value）
 * @param table hash table from which elements have to be retrieved
 * @param elements a pointer to an array of hash_table_element_t pointer (malloced by function)
 * @returns 1 when no memory 
 * @returns count of elements 
 */
size_t hash_table_get_elements(hash_table_t * table, hash_table_element_t *** elements)
{
    size_t i = 0;
    size_t count = 0;
    (*elements) = (hash_table_element_t **) calloc(table->key_count, sizeof(hash_table_element_t *));
    if (!*elements) 
    {
        INFO("No Memory to allocate elements array");
        return 1;
    }
    for(i=0;i<HASH_LEN;i++)
    {
        if (table->store_house[i])
        {
            (*elements)[count++] = table->store_house[i];
            hash_table_element_t *temp = table->store_house[i];
            #ifdef DEBUG
            size_t num = 1;
            #endif
            while(temp->next)
            {
                (*elements)[count++] = temp->next;
                temp = temp->next;
                #ifdef DEBUG
                num++;
                #endif
            }
            #ifdef DEBUG
            LOG("found %d key(s) at hash -> %d", (int)num, (int)i);
            #endif 
        }
    }
    return count;
}

/**
 * 函数功能: 这是一个hash函数，给定一个key，返回它的hash值
 * @param key pointer to the key
 * @param key_len length of the key
 * @param max_key max value of the hash to be returned by the function 
 * @returns hash value belonging to [0, max_key)
 */
uint32_t hash_table_do_hash(void * key, size_t key_len, uint16_t max_key)
{
    unsigned char *str = (unsigned char *) key;     /*uint8_t : unsigned char*/
    size_t seed = 131; /*  31   131   1313   131313  */
    uint32_t hash = 0;                   /*uint32_t : 4 bytes   == unsigned int*/
    size_t  i = 0;
    for ( i = 0; i < key_len; key++, ++i)
    {
        hash = (hash*seed) + (*str);
    }
    hash = hash % max_key;
    return hash;
}
/*
uint16_t hash_table_do_hash(void * key, size_t key_len, uint16_t max_key)
{
    uint16_t *ptr = (uint16_t *) key;
    uint16_t hash = 0xbabe; // WHY NOT
    size_t i = 0;
    for(;i<(key_len/2);i++)
    {
        hash^=(i<<4 ^ *ptr<<8 ^ *ptr);
        ptr++;
    }
    hash = hash % max_key;
    return hash;
}
*/
/**
 * 函数功能：调整hash_table的大小
 * @param table hash table to be resized
 * @param len new length of the hash table
 * @returns -1 when no elements in hash table
 * @returns -2 when no emmory for new store house
 * @returns 0 when sucess
 */
int hash_table_resize(hash_table_t *table, size_t len)
{
    LOG("resizing hash table from %d to %d", table->key_num, len);
    hash_table_element_t ** elements;
    size_t count;
    // FIXME traversing the elements twice, change it some time soon
    count = hash_table_get_elements(table, &elements);
    if (!count) 
    {
        INFO("Got No Elements from the hash table");
        return -1;
    }
    // keep the current store house in case we dont get more memory
    hash_table_element_t ** temp = table->store_house;
    table->store_house = calloc(len, sizeof(hash_table_element_t *));
    if (!table->store_house)
    {
        table->store_house = temp;
        INFO("No Memory for new store house");
        return -2;
    }
    table->key_num = len;
    // fool the new hash table so if refers even previously copied values
    int mode = table->mode;
    table->mode = MODE_ALLREF;
    // the new table starts from scratch
    table->key_count = 0;
    while(count>0)
    {
        hash_table_element_t *elem = elements[--count];
        hash_table_add(table, elem->key, elem->key_len, elem->value, elem->value_len);
    }
    table->mode = mode;
    // free old store house
    free(temp);
    return 0;
}

/**
 * Function to iterate through all elements of the hashtable
 * @param table hash table to be iterated
 * @param fct pointer to a function returning 1 if the element has to be removed
 * @param user arbitrary user pointer passed to the fct callback
 * @returns 0 when success
 */
int hash_table_iterate(hash_table_t *table, int (*fct)(void *user,
    void *value, void *key, size_t key_len), void *user)
{
    LOG("iterating hash table");
    int i;

    for(i=0;i<HASH_LEN;i++)
    {
        if (table->store_house[i])
        {
            hash_table_element_t *temp = table->store_house[i];
            hash_table_element_t *prev = NULL;
            while(temp)
            {
              int r = fct(user, temp->value, temp->key, temp->key_len);
              if (r){
                hash_table_element_t *next = temp->next;
                hash_table_element_delete(table,temp);
                if(prev == NULL)
                  table->store_house[i] = next;
                else
                  prev->next = next;
                temp = next;
                table->key_count--;
              } else {
                prev = temp;
                temp = temp->next;
              }
            }
        }
    }
    return 0;
}
