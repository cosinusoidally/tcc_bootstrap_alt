#define NULL ((void *)0)
typedef unsigned int size_t;

int puts(const char *s);
/* stdlib.h */
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

/* string.h */
size_t strlen(const char *s);
int memcmp(const void *s1, const void *s2, size_t n);

#define PROT_READ      0x1
#define PROT_WRITE     0x2
#define PROT_EXEC      0x4
#define MAP_PRIVATE    0x02
#define MAP_ANONYMOUS  0x20

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, int off);
