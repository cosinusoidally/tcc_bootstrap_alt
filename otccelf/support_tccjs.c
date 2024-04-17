int NULL;
int int_size;
int stack_size;
int v_stack;
int v_esp;
int v_ebp;

v_malloc(s){
  printf("malloc %d\n",s);
  return malloc(s);
}

v_strlen(s){
  int r;
  r = strlen(s);
  printf("strlen %d %s\n", r, s);
  return r;
}

v_strcpy(dst, src){
  int r;
  r = strcpy(dst, src);
  printf("strcpy %s %s\n", dst, src);
  return r;
}

v_free(v){
  return 0;
}

v_realloc(ptr, size){
  printf("realloc ptr: %d size: %d\n", ptr, size);
  return realloc(ptr, size);
}

v_memcpy(dest, src,n){
  printf("memcpy dest: %d src: %d n %d\n", dest, src, n);
  return memcpy(dest, src, n);
}

v_memset(s, c, n){
  printf("memset s %d c %c n %d\n", s, c, n);
  return memset(s, c, n);
}

v_fopen(pathname, mode){
  printf("fopen pathname %s mode %s\n", pathname, mode);
  return fopen(pathname, mode);
}

v_getc_unlocked(f){
  int c;
  c = getc_unlocked(f);
  printf("getc_unlocked: %c\n",c);
  return c;
}

wi8(o, v){
  *(char*)o = (v & 255);
}

ri8(o){
 return *(char*)o;
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

urs(){
  puts("urs not impl");
  exit(1);
}

v_alloca(x) {
  v_esp=v_esp-x;
  return v_esp;
}

mk_char(c){
  return c;
}

mk_c_string(s){
  return s;
}

expect(){
  puts("expect not impl");
  exit(1);
}

enter() {
/* FIXME detect overflow */
  printf("enter esp: 0x%x ebp: 0x%x\n", v_esp, v_ebp);
  v_esp=v_esp-4;
  wi32(v_esp,v_ebp);
  v_ebp=v_esp;
}

leave(x) {
  v_esp=v_ebp;
  v_ebp=ri32(v_esp);
  v_esp=v_esp+4;
  printf("leave esp: 0x%x ebp: 0x%x\n", v_esp, v_ebp);
  return x;
}

v_fclose(){
  puts("v_fclose not impl");
  exit(1);
}

v_strrchr(){
  puts("v_strrchr not impl");
  exit(1);
}

v_strcat(){
  puts("v_strcat not impl");
  exit(1);
}

v_strdup(){
  puts("v_strdup not impl");
  exit(1);
}

v_memcmp(s1, s2, n){
  printf("memcmp s1 %d s2 %d n %d\n", s1, s2, n);
  return memcmp(s1, s2, n);
}

warning(){
  puts("warning not impl");
  exit(1);
}

v_fwrite(){
  puts("v_fwrite not impl");
  exit(1);
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
  stack_size=256*1024;
  v_stack=v_malloc(stack_size);
  v_esp=v_stack+stack_size-4;
  v_ebp=v_esp;
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
