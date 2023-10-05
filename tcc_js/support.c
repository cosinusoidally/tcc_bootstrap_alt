#define alloca v_alloca

#define malloc v_malloc
#define realloc v_realloc
#define main v_main

int malloc(int x) {
  puts("unimpl malloc");
  err();
}
int wi32(int a1,int a2){
  puts("unimpl wi32");
  err();
}
int wi8(int a1,int a2){
  puts("unimpl wi8");
  err();
}
int urs(int a1,int a2);
int error(int a1);
int ri32(int a1);
int err(void);
void free(int x);
int alloca(int x);
int mk_c_string(char *s){
  puts("unimpl mk_c_string");
}
int strcpy(int a1, int a2);
int strlen(int len);
int mk_char(int a);
int enter(void);
int ri8(int a);
int memcmp(int a, int b, int c);
int leave();
int realloc(int a,int b);
int memcpy(int a, int b, int c);
int fclose(int a);
int getc_unlocked(int a);
int strrchr(int a, int b);
int strcat(int a,int b);
int fopen(int a, int b);
int strdup(int a);
int fwrite(int a,int b, int c, int d);
int puts(char *a);	
int memset(int a, int b, int c);
int init_runtime(void);
