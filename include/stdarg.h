/*  stdarg.h
    Definitions for accessing parameters in functions that accept
    a variable number of arguments.
*/

typedef void *va_list;

#define __size(x)           ((sizeof(x)+sizeof(int)-1) & ~(sizeof(int)-1))
#define va_start(ap,parmN)  ((void)((ap) = (va_list)((char *)(&parmN)+__size(parmN))))
#define va_arg(ap,type)     (*(type *)(((*(char **)&(ap))+=__size(type))-(__size(type))))
#define va_end(ap)          ((void)0)
