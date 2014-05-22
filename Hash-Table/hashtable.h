#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include<sys/types.h>
#include<stdint.h>

/*********************************************************************************************************************************************
 *                                                      宏定义
 *
 * ******************************************************************************************************************************************/
#define HASH_LEN    table->key_num
#define HASH(x,y)   hash_table_do_hash(x,y,HASH_LEN)
#define hash_table_element_s   sizeof(hash_table_element_t) 
#define hash_table_s     sizeof(hash_table_t)
#define HT_REMOVE(table, key)   hash_table_remove(table, key, sizeof(*key))
#define HT_ADD(table, key, value)   hash_table_add(table, (void *) key, sizeof(*key), (void *) value, sizeof(*value))
#define HT_LOOKUP(table, key)    hash_table_lookup(table, key, sizeof(*key))
#define HT_HAS_KEY(table, key)   hash_table_has_key(table, key, sizeof(*key))

/*********************************************************************************************************************************************
 *                                                          枚举类型
 * ******************************************************************************************************************************************/

typedef enum hash_table_mode{
    MODE_COPY,  /*MODE_COPY = 0, 复制模式，{ key,value }均可复制*/
    MODE_VALUEREF, /*MODE_VALUEREF = 0, value 不可复制,可以被refer，key可以被复制*/
    MODE_ALLREF    /*MODE_ALLREF = 0, value 和 key 均不可以被复制，只可以被refer*/
} hash_table_mode_t;

/*********************************************************************************************************************************************
 *                                                          数据结构
 * ******************************************************************************************************************************************/

struct hash_table_element
{
    size_t key_len;  /*key 长度*/
    size_t value_len; /*value 长度*/
    void * key;      /*指向key的指针*/
    void * value;   /*指向value的指针*/
    struct hash_table_element *next;  /*遇到hash冲突，使用链表解决*/
};

typedef struct hash_table_element hash_table_element_t;

struct hash_table
{
    hash_table_element_t  ** store_house;  /*存储所有key,value 实体*/
    hash_table_mode_t mode;  /*hash_table_t 的模型*/

    size_t key_count; /*hash table 中现有的key 个数*/
    uint16_t key_num;  /*hash table 能够存储的key总数*/
    size_t key_ratio;/*载荷因子，key_count / key_num */
};

typedef struct hash_table hash_table_t;


/*********************************************************************************************************************************************
 *                                                          函数声明
 *                                                      （1）元素操作
 *                                                      （2）哈希表操作
 * ******************************************************************************************************************************************/

/*创建一个新元素*/
hash_table_element_t * hash_table_element_new(); 

/*删除hash table 中的元素*/
void hash_table_element_delete(hash_table_t *, hash_table_element_t *);

/*hash 函数的定义：给定一个key, 返回一个hash值*/
uint32_t hash_table_do_hash(void * key, size_t key_len, uint16_t max_key);

/*
 *++++++++++++++hash table的操作++++++++++++++
 */

/*创建一个新的hash table*/
hash_table_t * hash_table_new(hash_table_mode_t);

/*删除hash table*/
void hash_table_delete(hash_table_t *);

/*向hash table 中添加一个（key, value）
 * 添加成功，返回0，添加失败，返回-1
 * */
int hash_table_add(hash_table_t *, void *, size_t, void *, size_t);

/**
 *从hash table 中移除/删除一个key，value 
 *删除成功返回0, 失败返回-1
 */
int hash_table_remove(hash_table_t *, void *, size_t);

/*
 *在hash table中查找key
 *成功操作，返回一个void* 指针，指向key对应的value
 *失败，则返回NULL
 * */

void * hash_table_lookup(hash_table_t *, void *, size_t);
/*
 *判断key 是否在hash table中
 *成功操作，返回1,
 *失败，则返回0
 */
int hash_table_has_key(hash_table_t *, void *, size_t);



/*返回hash table中所有的keys个数*/
size_t hash_table_get_keys(hash_table_t *, void **);

/**
 * Function to get all elements (key - value pairs) from the given hash table
 * @param table hash table from which elements have to be retrieved
 * @param elements a pointer to an array of hash_table_element_t pointer (malloced by function)
 * @returns 1 when no memory 
 * @returns count of elements 
 */
size_t hash_table_get_elements(hash_table_t *, hash_table_element_t *** );

/**
 * Function to resize the hash table store house
 * @param table hash table to be resized
 * @param len new length of the hash table
 * @returns -1 when no elements in hash table
 * @returns -2 when no emmory for new store house
 * @returns 0 when sucess
 */
int hash_table_resize(hash_table_t *, size_t);

/**
 * Function to iterate through all elements of the hashtable
 * @param table hash table to be iterated
 * @param fct pointer to a function returning 1 if the element has to be removed
 * @param user arbitrary user pointer passed to the fct callback
 * @returns 0 when success
 */
int hash_table_iterate(hash_table_t *table, int (*fct)(void *user,
    void *value, void *key, size_t key_len), void *user);
#endif
