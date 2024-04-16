#define v_malloc malloc
#define v_strlen strlen
#define v_strcpy strcpy
#define v_free free

int NULL;
int int_size;
int stack_size;
int v_stack;
int esp;
int ebp;

wi8(){
  puts("wi8 not impl");
  exit(1);
}

wi32(){
  puts("wi32 not impl");
  exit(1);
}

ri8(){
  puts("ri8 not impl");
  exit(1);
}

ri32(){
  puts("ri32 not impl");
  exit(1);
}

mk_c_string(){
  puts("mk_c_string not impl");
  exit(1);
}

urs(){
  puts("urs not impl");
  exit(1);
}

int NULL;

v_alloca(){
  puts("v_alloca not impl");
  exit(1);
}

mk_char(){
  puts("v_alloca not impl");
  exit(1);
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

mk_argc_argv(){
  puts("mk_argc_argv not impl");
  exit(1);
}

init_runtime() {
  puts("init_runtime");
  stack_size=256*1024;
  v_stack=malloc(stack_size);
  esp=v_stack+stack_size-4;
  ebp=esp;
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
