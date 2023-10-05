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

// chars are signed but we want unsigned
int ri8(int o) {
//  int t=heap[a];
  int o1=o>>2;
  int s=o&3;
  int v1=((int*)heap)[o1];
  return (v1>>(s*8)) &0xFF;
}

int wi32(int o,int v) {
//  puts("unimpl wi32");
//  err();
  wi8(o,v&0xFF);
  v=v>>8;
  wi8(o+1,v&0xFF);
  v=v>>8;
  wi8(o+2,v&0xFF);
  v=v>>8;
  wi8(o+3,v&0xFF);
}

int ri32(int o) {
//  puts("unimpl ri32");
//  err();
  return ((ri8(o+3)<<24) | (ri8(o+2)<<16) | (ri8(o+1)<<8) |ri8(o));
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

// might not be needed?
int string_cache(int sc){
  printf("sc: %d\n",sc);
  return 0;
}

int mk_c_string(char *s){
//  puts("unimpl mk_c_string");
  printf("mk_c_string: %s\n",s);
  int sc=string_cache(s);
  if(sc){
    return sc;
  }
  int l=strlen(s);
  int sh=v_malloc(l+1);
  int i;
  printf("strlen %d\n",l);
  for(i=0;i<l;i++){
    wi8(sh+i,s[i]);
  }
  wi8(sh+i,0);
  return sh;
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
  int argc=0;
  int argv;
  int c;
  int i=0;
  int hs=mk_c_string(s);
  argv=v_alloca(4);
  wi32(argv,hs);
  while((c=ri8(hs+i))!=0){
    i=i+1;
    if(c==' ') {
      argc=argc+1;
      wi8(hs+i-1,0);
      argv=v_alloca(4);
      wi32(argv,hs+i);
    }
  }
  argc=argc+1;
// reverse argv
  int argv_r=argv;
  for(i=0;i<argc;i++){
    argv=v_alloca(4);
    wi32(argv,ri32(argv_r));
    argv_r=argv_r+4;
  }
  wi32(argc_argv,argc);
  wi32(argc_argv+4,argv);
  return argc_argv;
}

/* initialise the runtime */
int init_runtime(void) {
  puts("init_runtime");
  heap_size=16*1024*1024;
  stack_size=256*1024;
  heap=(char *)malloc(heap_size);
  esp=heap_size-4;
  ebp=esp;
  malloc_base=4;

}

int main(void){
  puts("running js_to_c generated code");
  init_runtime();
  int args=mk_argc_argv("tcc -r test1.c");
  int v_argc=ri32(args);
  int v_argv=ri32(args+4);
  printf("argc: %d\n",v_argc);
  printf("argv: %d\n",v_argv);
  tcc_main(v_argc,v_argv);
}
