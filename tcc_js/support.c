#include "support_tcc_gcc.c"

int heap_size;
int stack_size;
char* heap;

int esp;
int ebp;
int malloc_base;

/* unsigned right shift (urs) */

int urs(int v,int n) {
  if(n==0) {
    return v;
  }
  int m=0x80000000;
  int m2=0x40000000;
  int t=v&m;
  v=v & ~m;
  v=v >> n;
  if(t){
    v=v | (m2 >> (n-1));
  }
  return v;

}

/* virtual heap access */
int wi8(int o,int v) {
  heap[o]=v;
  return;
}

/* chars are signed but we want unsigned */
int ri8(int o) {
/*  int t=heap[a]; */
  int o1=o>>2;
  int s=o&3;
  int v1;
  int *h=heap;
  v1=h[o1*int_size];
  return (v1>>(s*8)) &0xFF;
}

int wi32(int o,int v) {
/*  puts("unimpl wi32"); err(); */
  wi8(o,v&0xFF);
  v=v>>8;
  wi8(o+1,v&0xFF);
  v=v>>8;
  wi8(o+2,v&0xFF);
  v=v>>8;
  wi8(o+3,v&0xFF);
}

int ri32(int o) {
/*  puts("unimpl ri32"); err(); */
  return ((ri8(o+3)<<24) | (ri8(o+2)<<16) | (ri8(o+1)<<8) |ri8(o));
}


/* virtual heap memory functions */

int v_alloca(int x) {
  esp=esp-x;
  return esp;
}

int enter(void) {
/*  puts("unimpl enter"); err(); */
  esp=esp-4;
  wi32(esp,ebp);
  ebp=esp;
}
int leave(int x) {
/*  puts("unimpl leave"); err(); */
  esp=ebp;
  ebp=ri32(esp);
  esp=esp+4;
  return x;
}

void v_free(int x) {
/*  puts("unimpl free"); err(); */
  return;
}

int v_malloc(int x) {
/*  puts("unimpl malloc"); err(); */
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

int v_realloc(int x,int size) {
/*  puts("unimpl realloc"); err(); */
  int i;
  int r=v_malloc(size);
  for(i=0;i<size;i=i+1){
    wi8(r+i,ri8(x+i));
  }
  return r;
}

int v_memcmp(int s1, int s2, int n) {
/*  puts("unimpl memcmp"); err(); */
  int i;
  int r=0;
  for(i=0;i<n;i=i+1){
/* FIXME not quite right */
    if(ri8(s1)!=ri8(s2)){
      r=1;
    }
  }
  return r;
}

int v_memcpy(int dest, int src, int count) {
/*  puts("unimpl memcpy"); err(); */
  int i;
  for(i=0;i<count;i=i+1){
    wi8(dest+i,ri8(src+i));
  }

}

int v_memset(int x, int v, int size) {
/*  puts("unimpl memset"); err(); */
  int i;
  for(i=0;i<size;i=i+1){
    wi8(x+i,v);
  }
}

/* virtual heap string functions */

int mk_char(int a) {
/*  puts("unimpl mk_char"); err(); */
  return a;
}

/* might not be needed? */
int string_cache(int sc){
/*  printf("sc: %d\n",sc); */
  return 0;
}

int mk_c_string(char *s){
/*  puts("unimpl mk_c_string"); */
/*  printf("mk_c_string: %s\n",s); */
  int sc=string_cache(s);
  if(sc){
    return sc;
  }
  int l=strlen(s);
  int sh=v_malloc(l+1);
  int i;
/*  printf("strlen %d\n",l); */
  for(i=0;i<l;i=i+1){
    wi8(sh+i,s[i]);
  }
  wi8(sh+i,0);
  return sh;
}

int v_strcat(int dest,int src) {
/*  puts("unimpl strcat"); err(); */
  int d=dest;
  int c;
  while(ri8(dest)){
    dest=dest+1;
  }
  while(c=ri8(src)){
    src=src+1;
    wi8(dest,c);
    dest=dest+1;
  }
  wi8(dest,0);
  return d;
}

int v_strcpy(int dest, int src) {
/*  puts("unimpl strcpy"); err(); */
  int c;
  while((c=ri8(src))!=0){
    src=src+1;
    wi8(dest,c);
    dest=dest+1;
  }
}

int v_strdup(int src) {
/*  puts("unimpl strdup"); err(); */
  int i;
  int l=v_strlen(src);
  int dest=v_malloc(l+1);
  for(i=0;i<l;i=i+1){
    wi8(dest+i, ri8(src+i));
  }
  wi8(dest+l, 0);
  return dest;

}

int v_strlen(int s) {
/*  puts("unimpl strlen"); err(); */
  int l=0;
  while(ri8(s)){
    s=s+1;
    l=l+1;
  }
  return l;
}

int v_strrchr(int s, int c) {
/*  puts("unimpl strrchr"); err(); */
  int c1;
  while(c1=ri8(s)){
    s=s+1;
    if(c1==c){
      return s-1;
    }
  }
  return 0;
}

/* virtual file functions */

int v_getc_unlocked(int a) {
/*  puts("unimpl getc_unlocked"); err(); */
  return getc_unlocked(a);
}

int v_fclose(int a) {
/*  puts("unimpl fclose"); err(); */
  return fclose(a);
}

int v_fopen(int a, int b) {
/*  puts("unimpl fopen"); err(); */
/* FIXME ljw will we have trouble with this being signed? */
  return fopen(heap+a,heap+b);
}

int v_fwrite(int a,int b, int c, int d) {
//  puts("unimpl fwrite");
//  err();
  return fwrite(heap+a ,b ,c , d);
}

/* error functions */
int err(void) {
  puts("err");
  exit(1);
}

int error(char *m){
  puts("error");
  puts(m);
  err();
}

int warning(char *m){
  puts("warning");
  puts(m);
  err();
}

int expect(char *m){
  puts("expect");
  puts(m);
  err();
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
/* reverse argv */
  int argv_r=argv;
  for(i=0;i<argc;i=i+1){
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
  heap=malloc(heap_size);
  esp=heap_size-4;
  ebp=esp;
  malloc_base=4;

}

int main(void){
  init_c();
  puts("running js_to_c generated code");
  init_runtime();
  int args=mk_argc_argv("tcc -r test-dlsym_wrap.c");
  int v_argc=ri32(args);
  int v_argv=ri32(args+4);
/*  printf("argc: %d\n",v_argc); */
/*  printf("argv: %d\n",v_argv); */
  tcc_main(v_argc,v_argv);
}
