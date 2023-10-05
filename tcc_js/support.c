#define alloca v_alloca

#define malloc v_malloc
#define realloc v_realloc

/* decls for real functions */
#define malloc_real malloc
void *malloc_real(int size);

int puts(char *a);

int heap_size;
int stack_size;
char* heap;

/* virtual heap access */
int wi8(int o,int v) {
  heap[o]=v;
  return;
}

int ri8(int a) {
  return heap[a];
}

int wi32(int a1,int a2) {
  puts("unimpl wi32");
  err();
}

int ri32(int a1) {
  puts("unimpl urs");
  err();
}

/* unsigned right shift (urs) */

int urs(int a1,int a2) {
  puts("unimpl urs");
  err();
}

/* virtual heap memory functions */

int v_alloca(int x) {
  puts("unimpl alloca");
  err();
}

int enter(void) {
  puts("unimpl enter");
  err();
}
int leave(int a) {
  puts("unimpl leave");
  err();
}

void v_free(int x) {
  puts("unimpl urs");
  err();
}

int v_malloc(int x) {
  puts("unimpl malloc");
  err();
}

int v_realloc(int a,int b) {
  puts("unimpl realloc");
  err();
}

int v_memcmp(int a, int b, int c) {
  puts("unimpl memcmp");
  err();
}

int v_memcpy(int a, int b, int c) {
  puts("unimpl memcpy");
  err();
}

int v_memset(int a, int b, int c) {
  puts("unimpl memset");
  err();
}

/* virtual heap string functions */

int mk_char(int a) {
  puts("unimpl mk_char");
  err();
}

int mk_c_string(char *s){
  puts("unimpl mk_c_string");
}

int v_strcat(int a,int b) {
  puts("unimpl strcat");
  err();
}
int v_strcpy(int a1, int a2) {
  puts("unimpl strcpy");
  err();
}

int v_strdup(int a) {
  puts("unimpl strdup");
  err();
}

int v_strlen(int len) {
  puts("unimpl strlen");
  err();
}

int v_strrchr(int a, int b) {
  puts("unimpl urs");
  err();
}

/* virtual file functions */

int v_getc_unlocked(int a) {
  puts("unimpl getc_unlocked");
  err();
}

int v_fclose(int a) {
  puts("unimpl fclose");
  err();
}

int v_fopen(int a, int b) {
  puts("unimpl fopen");
  err();
}

int v_fwrite(int a,int b, int c, int d) {
  puts("unimpl fwrite");
  err();
}

/* error functions */
int err(void) {
  puts("err");
  exit(1);
}

/* initialise the runtime */
int init_runtime(void) {
  puts("init_runtime");
  heap_size=16*1024*1024;
  stack_size=256*1024;
  heap=(char *)malloc_real(heap_size/4);

  err();
}

int main(void){
  puts("running js_to_c generated code");
  tcc_main(0,0);
}
