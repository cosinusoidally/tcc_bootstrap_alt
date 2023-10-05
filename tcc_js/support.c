#define alloca v_alloca

#define malloc v_malloc
#define realloc v_realloc
#define main v_main

#define real_main main

int malloc(int x) {
  puts("unimpl malloc");
  err();
}

/* virtual heap access */
int wi32(int a1,int a2) {
  puts("unimpl wi32");
  err();
}
int ri32(int a1) {
  puts("unimpl urs");
  err();
}
int wi8(int a1,int a2) {
  puts("unimpl wi8");
  err();
}
int ri8(int a) {
  puts("unimpl ri8");
  err();
}

int urs(int a1,int a2) {
  puts("unimpl urs");
  err();
}
int error(int a1) {
  puts("unimpl err");
  err();
}
int err(void) {
  puts("err");
  exit(1);
}
void free(int x) {
  puts("unimpl urs");
  err();
}
int alloca(int x) {
  puts("unimpl alloca");
  err();
}
int mk_c_string(char *s){
  puts("unimpl mk_c_string");
}
int strcpy(int a1, int a2) {
  puts("unimpl strcpy");
  err();
}
int strlen(int len) {
  puts("unimpl strlen");
  err();
}
int mk_char(int a) {
  puts("unimpl mk_char");
  err();
}
int enter(void) {
  puts("unimpl enter");
  err();
}
int memcmp(int a, int b, int c) {
  puts("unimpl memcmp");
  err();
}
int leave(int a) {
  puts("unimpl leave");
  err();
}
int realloc(int a,int b) {
  puts("unimpl realloc");
  err();
}
int memcpy(int a, int b, int c) {
  puts("unimpl memcpy");
  err();
}
int fclose(int a) {
  puts("unimpl fclose");
  err();
}
int getc_unlocked(int a) {
  puts("unimpl getc_unlocked");
  err();
}
int strrchr(int a, int b) {
  puts("unimpl urs");
  err();
}
int strcat(int a,int b) {
  puts("unimpl strcat");
  err();
}
int fopen(int a, int b) {
  puts("unimpl fopen");
  err();
}
int strdup(int a) {
  puts("unimpl strdup");
  err();
}
int fwrite(int a,int b, int c, int d) {
  puts("unimpl fwrite");
  err();
}
int memset(int a, int b, int c) {
  puts("unimpl memset");
  err();
}


int init_runtime(void) {
  puts("init_runtime");
  err();
}

int puts(char *a);	
