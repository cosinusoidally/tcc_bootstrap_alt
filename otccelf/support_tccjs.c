#define v_malloc malloc
#define v_strlen strlen
#define v_strcpy strcpy
#define v_free free

int NULL;
int int_size;
int stack_size;
int v_stack;
int v_esp;
int v_ebp;

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

enter(){
  puts("enter not impl");
  exit(1);
}

v_memcmp(){
  puts("v_memcmp not impl");
  exit(1);
}

leave(){
  puts("leave not impl");
  exit(1);
}

v_realloc(){
  puts("v_realloc not impl");
  exit(1);
}

v_memcpy(){
  puts("v_mempcy not impl");
  exit(1);
}

v_fclose(){
  puts("v_fclose not impl");
  exit(1);
}

v_getc_unlocked(){
  puts("v_getc_unlocked not impl");
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

v_fopen(){
  puts("v_fopen not impl");
  exit(1);
}

v_strdup(){
  puts("v_strdup not impl");
  exit(1);
}

warning(){
  puts("warning not impl");
  exit(1);
}

v_fwrite(){
  puts("v_fwrite not impl");
  exit(1);
}

v_memset(){
  puts("v_memset not impl");
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
  v_stack=malloc(stack_size);
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
