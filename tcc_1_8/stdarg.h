typedef char *va_list;

/* fix a buggy dependency on GCC in libio.h */
typedef va_list __gnuc_va_list;
