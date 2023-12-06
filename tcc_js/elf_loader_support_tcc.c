#include "elf_loader_support.c"

extern int stdout;
extern int stdin;
extern int stderr;

typedef int (*FUNCTION) ();

int call_wrap(FUNCTION t, int a, int b){
  return t(a,b);
}

int init_c(void){
  p_size=1;
}

/* adapted from bootstrappable_load.c */
char* int2str(int x, int base, int signed_p)
{
        /* Be overly conservative and save space for 32binary digits and padding null */
        char* p = (char *)calloc(34, sizeof(char));
        /* if calloc fails return null to let calling code deal with it */
        if(0 == p) return p;

        p = p + 32;
        unsigned i;
        int sign_p = 0;
        char* table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        if(signed_p && (10 == base) && (0 != (x & 0x80000000)))
        {
                /* Truncate to 31bits */
                i = -x & 0x7FFFFFFF;
                if(0 == i) return "-2147483648";
                sign_p = 1;
        } /* Truncate to 32bits */
        else i = x & (0x7FFFFFFF | (1 << 31));

        do
        {
                p[0] = table[i % base];
                p = p - 1;
                i = i / base;
        } while(0 < i);

        if(sign_p)
        {
                p[0] = '-';
                p = p - 1;
        }

        return p + 1;
}


void puts_num(int x){
  printf("%u\n",x);
}

int get_stdout(void){
  return &stdout;
}

int get_stdin(void){
  return &stdin;
}

int get_stderr(void){
  return &stderr;
}

int fputs_tramp(int a, int b){
  return fputs(a,b);
}

int calloc_tramp(int a, int b){
  return calloc(a,b);
}

int exit_tramp(int x){
  puts("exit not impl");
  exit(1);
}

int fputc_tramp(int a, int b){
  return fputc(a,b);
}

void free_tramp(int x){
  free(x);
  return;
}

int malloc_tramp(int x){
  return malloc(x);
}

int realloc_tramp(int x, int y){
  return realloc(x,y);
}

int open_tramp(int a, int b, int c){
  return open(a,b,c);
}

int close_tramp(int x){
  return close(x);
}

int read_tramp(int a, int b, int c){
  return read(a,b,c);
}

int fopen_tramp(int a, int b){
  return fopen(a,b);
}

int fclose_tramp(int x){
  return fclose(x);
}
