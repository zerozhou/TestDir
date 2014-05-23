/*************************************************************************
	> File Name: uint_t.c
	> Author: zjzhou62
	> Mail: zjzhou62@mail.ustc.edu.cn
	> Created Time: Thu 22 May 2014 09:23:40 AM CST
 ************************************************************************/

#include<stdio.h>
#include <inttypes.h>
int main()
{
//    printf("uint16_t : size = %d\n", sizeof(uint16_t));
    printf("int16_t : size = %d\n", sizeof(int16_t));
    printf("int32_t : size = %d\n", sizeof(int32_t));
  //  printf("uint32_t :  size = %d\n", sizeof(uint32_t));
    printf("int : size = %d\n", sizeof(int));
    printf("unsigned int : size = %d\n", sizeof(unsigned int));
    printf("size_t : size = %d\n", sizeof(size_t));
    printf("short int : size = %d\n", sizeof(short int));
   // printf("unsigned short int : size = %d\n", sizeof(unsigned short int));
    printf("long int : size = %d\n", sizeof(long int));
   // printf("unsigned long int : size = %d\n", sizeof(unsigned long int));
    return 0;
}
