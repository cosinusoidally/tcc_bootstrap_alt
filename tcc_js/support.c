#define realloc v_realloc

/* decls for real functions */
void *malloc(int size);
int strlen(char *s);

int puts(char *a);

int heap_size;
int stack_size;
char* heap;

int esp;
int ebp;
int malloc_base;

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
  puts("unimpl ri32");
  err();
}

/* unsigned right shift (urs) */

int urs(int a1,int a2) {
  puts("unimpl urs");
  err();
}

/* virtual heap memory functions */

int v_alloca(int x) {
  esp=esp-x;
  return esp;
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
//  puts("unimpl malloc");
//  err();
  int r=malloc_base;
  if(x!=((x>>2)<<2)){
    x=4+(x>>2) <<2;
  }
  malloc_base=malloc_base+x;
  if(malloc_base>(heap_size-stack_size)){
    puts("oom malloc");
    err();
  }
  return r;
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
//  puts("unimpl mk_c_string");
  printf("mk_c_string: %s\n",s);
  int l=strlen(s);
  int sh=v_malloc(l+1);
  int i;
  printf("strlen %d\n",l);
  for(i=0;i<l;i++){
    wi8(sh+i,s[i]);
  }
  wi8(sh+i,0);
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

int mk_argc_argv(char *s){
  int argc_argv=v_alloca(8);
  int hs=mk_c_string(s);
  return argc_argv;
}

/* initialise the runtime */
int init_runtime(void) {
  puts("init_runtime");
  heap_size=16*1024*1024;
  stack_size=256*1024;
  heap=(char *)malloc(heap_size/4);
  esp=heap_size-4;
  ebp=esp;
  malloc_base=4;

}

int main(void){
  puts("running js_to_c generated code");
  init_runtime();
  int args=mk_argc_argv("tcc -r test1.c");
  tcc_main(ri32(args),ri32(args+4));
}
