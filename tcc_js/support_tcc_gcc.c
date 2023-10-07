/* decls for real functions */
void *malloc(int size);
int strlen(char *s);
int fopen(int path, int mode);
int fclose(int f);
int fwrite(int a,int b, int c, int d);
int getc_unlocked(int f);
int puts(char *a);

int int_size;

void init_c(void){
  int_size=1;
}

int NULL=0;

void puts_num(int x){
  printf("%d\n",x);
}
