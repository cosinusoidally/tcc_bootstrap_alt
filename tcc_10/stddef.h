#ifndef _STDDEF_H
#define _STDDEF_H

#define NULL ((void *)0)
typedef unsigned int size_t;
typedef int wchar_t;
typedef int ptrdiff_t;
#define offsetof(type, field) ((size_t) &((type *)0)->field)

/* need to do that because of glibc 2.1 bug (should have a way to test
   presence of 'long long' without __GNUC__, or TCC should define
   __GNUC__ ? */
#ifndef __int8_t_defined
#define __int8_t_defined
typedef	char int8_t;
typedef	short int int16_t;
typedef	int int32_t;
typedef long long int int64_t;
#endif

#endif
