int v_malloc(int x);
int wi32(int a1,int a2);
int wi8(int a1,int a2);
int urs(int a1,int a2);
int error(int a1);
int ri32(int a1);
int err(void);
void v_free(int x);
int v_alloca(int x);
int mk_c_string(char *s);
int v_strcpy(int a1, int a2);
int v_strlen(int len);
int mk_char(int a);
int enter(void);
int ri8(int a);
int v_memcmp(int a, int b, int c);
int leave(int a);
int v_realloc(int a,int b);
int v_memcpy(int a, int b, int c);
int v_fclose(int a);
int v_getc_unlocked(int a);
int v_strrchr(int a, int b);
int v_strcat(int a,int b);
int v_fopen(int a, int b);
int v_strdup(int a);
int v_fwrite(int a,int b, int c, int d);
int v_memset(int a, int b, int c);
int init_runtime(void);
void puts_num(int x);

int fputs(int a,int b);
int stdout;

int puts(char *a) {
  fputs(a,stdout);
  fputs("\n",stdout);
}

int getc_unlocked(int f){
/*  puts("getc_unlocked not impl"); err(); */
  return fgetc(f);
}
int fwrite(int ptr,int size, int nitems, int stream) {
/*
  puts("fwrite not impl");
  err();
*/
  int t=size*nitems;
  char *c=ptr;
  int c;
  while(t>0){
    fputc(c[0],stream);
    t=t-1;
    c=c+1;
  }
}
int tcc_main(int argc, int argv);

int int_size;

void init_c(void){
  int_size=1;
}

void puts_num(int x){
  char *s;
  s=int2str(x,10,0);
  puts(s);
  free(s);
  return;
}
