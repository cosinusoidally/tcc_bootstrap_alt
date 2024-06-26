int NULL;
int int_size;

int heap_size;
int stack_size;
int heap;

int esp;
int ebp;
int malloc_base;

v_malloc(x){
/*  puts("v_malloc not impl"); exit(1); */
  int r;
  r=malloc_base;
  if(x!=((x>>2)<<2)){
    x=4+(x>>2) <<2;
  }
  malloc_base=malloc_base+x;
  if(malloc_base>(heap_size-stack_size)){
    puts("oom malloc");
    exit(1);
  }
  return r;
}

v_strlen(s){
/*  puts("v_strlen not impl"); exit(1); */
  int l;
  l = 0;
  while(ri8(s)){
    s=s+1;
    l=l+1;
  }
  return l;
}

v_strcpy(dest, src){
/*  puts("v_strcpy not impl"); exit(1); */
  int c;
  while((c=ri8(src))!=0){
    src=src+1;
    wi8(dest,c);
    dest=dest+1;
  }
}

v_free(v){
/* puts("v_free not impl"); exit(1); */
  return 0;
}

v_realloc(x, size){
/*  puts("v_realloc not impl"); exit(1); */
  int i;
  int r;
  r = v_malloc(size);
  for(i=0;i<size;i=i+1){
    wi8(r+i,ri8(x+i));
  }
  return r;
}

v_memcpy(dest, src, count){
/* puts("v_memcpy not impl"); exit(1); */
  int i;
  for(i=0;i<count;i=i+1){
    wi8(dest+i,ri8(src+i));
  }
}

v_memset(x, v, size){
/* puts("v_memset not impl"); exit(1); */
  int i;
  for(i=0;i<size;i=i+1){
    wi8(x+i,v);
  }
}

v_fopen(a, b){
/* puts("v_fopen not impl"); exit(1); */
  int filename;
  int mode;
  int r;
  filename = heap+a;
  mode = heap+b;
  r = fopen(filename, mode);
  printf("v_fopen filename: %s mode: %s\n", filename,mode);
  return r;
}

v_getc_unlocked(a){
/* puts("v_getc_unlocked not impl"); exit(1); */
  int c;
  c = getc_unlocked(a);
  printf("getc_unlocked: %c\n",c);
  return c;
}

wi8(o, v){
  int o1;
  o1 = heap + o;
  *(char*)o1 = (v & 255);
}

/* real heap rather than virtual */
r_ri8(o){
  return *(char*)o;
}

ri8(o){
  int o1;
  o1 = heap + o;
  return *(char*)o1;
}

wi32(o, v) {
  wi8(o,v&0xFF);
  v=v>>8;
  wi8(o+1,v&0xFF);
  v=v>>8;
  wi8(o+2,v&0xFF);
  v=v>>8;
  wi8(o+3,v&0xFF);
}

ri32(o) {
  return (ri8(o)&255)       | (ri8(o+1)&255)<<8 |
         (ri8(o+2)&255)<<16 | (ri8(o+3)&255)<<24;
}

/* unsigned right shift (urs) */
urs(v, n){
/* puts("urs not impl"); exit(1); */
  int m;
  int m2;
  int t;
  if(n==0) {
    return v;
  }
  m2=0x40000000;
/* was 0x80000000 but that literal is too large for tcc_js.c to parse */
  m=m2 << 1;
  t=v&m;
  v=v & ~m;
  v=v >> n;
  if(t){
    v=v | (m2 >> (n-1));
  }
  return v;
}

v_alloca(x) {
  esp=esp-x;
  return esp;
}

mk_char(c){
  return c;
}

mk_c_string(s){
/*  puts("unimpl mk_c_string"); */
/*  printf("mk_c_string: %s\n",s); */
  int l;
  int sh;
  int i;

  l=strlen(s);
  sh=v_malloc(l+1);
/*  printf("strlen %d\n",l); */
  for(i=0;i<l;i=i+1){
    wi8(sh+i,r_ri8(s+i));
  }
  wi8(sh+i,0);
  return sh;
}

expect(){
  puts("expect not impl");
  exit(1);
}

enter() {
/* FIXME detect overflow */
  printf("enter esp: 0x%x ebp: 0x%x\n", esp, ebp);
  esp=esp-4;
  wi32(esp,ebp);
  ebp=esp;
}

leave(x) {
  esp=ebp;
  ebp=ri32(esp);
  esp=esp+4;
  printf("leave esp: 0x%x ebp: 0x%x\n", esp, ebp);
  return x;
}

v_fclose(a){
/*  puts("v_fclose not impl"); exit(1); */
  return fclose(a);
}

v_strrchr(s, c){
/* puts("v_strrchr not impl"); exit(1); */
  int c1;
  while(c1=ri8(s)){
    s=s+1;
    if(c1==c){
      return s-1;
    }
  }
  return 0;
}

v_strcat(dest, src){
/* puts("v_strcat not impl"); exit(1); */
  int d;
  int c;
  d = dest;
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

v_strdup(src){
/* puts("v_strdup not impl"); exit(1); */
  int i;
  int l;
  int dest;
  l=v_strlen(src);
  dest=v_malloc(l+1);
  for(i=0;i<l;i=i+1){
    wi8(dest+i, ri8(src+i));
  }
  wi8(dest+l, 0);
  return dest;
}

v_memcmp(s1, s2, n){
/* puts("v_memcmp not impl"); exit(1); */
  int i;
  int r;
  r=0;
  for(i=0;i<n;i=i+1){
    if(ri8(s1+i)!=ri8(s2+i)){
/* FIXME ljw not quite right */
      r=1;
    }
  }
  return r;
}

warning(){
  puts("warning not impl");
  exit(1);
}

v_fwrite(a, b, c, d){
/* puts("v_fwrite not impl"); exit(1); */
  return fwrite(heap+a ,b ,c , d);
}

mk_argc_argv(s){
  int argv_r;
  int argc_argv;
  int argc;
  int argv;
  int c;
  int i;
  int hs;

  argc_argv=v_alloca(8);
  argc=0;
  argv;
  i=0;
  hs=mk_c_string(s);

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
  argv_r=argv;
  for(i=0;i<argc;i=i+1){
    argv=v_alloca(4);
    wi32(argv,ri32(argv_r));
    argv_r=argv_r+4;
  }
  wi32(argc_argv,argc);
  wi32(argc_argv+4,argv);
  return argc_argv;
}

init_runtime() {
  puts("init_runtime");
  heap_size=16*1024*1024;
  stack_size=256*1024;
  heap = malloc(heap_size);
  printf("heap: %d\n", heap);
  esp=heap_size-4;
  ebp=esp;
  malloc_base=4;
}


init_c(){
  int_size=1;
  NULL=0;
}

main(){
  int args;
  int v_argc;
  int v_argv;
  init_c();
  puts("running js_to_c generated code");
  init_runtime();
  args=mk_argc_argv("tcc -r test-dlsym_wrap.c");
  v_argc=ri32(args);
  v_argv=ri32(args+4);
  printf("argc: %d\n",v_argc);
  printf("argv: %d\n",v_argv);
  tcc_main(v_argc,v_argv);
}
