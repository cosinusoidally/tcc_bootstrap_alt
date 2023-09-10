#define NULL ((void *)0)
typedef unsigned int size_t;

int puts(const char *s);
/* stdlib.h */
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

/* string.h */
size_t strlen(const char *s);
int memcmp(const void *s1, const void *s2, size_t n);
