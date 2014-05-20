#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DEBUG

#include<stdio.h>
/*_FILE_ 和 _LINE_ 是内建于C语言预处理器中的宏，它们会被扩展为所在文件的文件名和所处代码行的行号。*/
#define INFO(format) fprintf(stderr,"%s:%d:%s -> " format "\n", __FILE__, __LINE__, __func__)
#define LOG(format, ...) fprintf(stderr,"%s:%d:%s -> " format "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)

#else

#define LOG(...)
#define INFO(...)

#endif // debug

#endif // debug.h
