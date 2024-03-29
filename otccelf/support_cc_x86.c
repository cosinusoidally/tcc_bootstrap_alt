int wi8(int o, int v);
int ri8(int o);
int wi32(int o, int v);
int ri32(int o);
int expr(void);
int decl(int l);

int puts(char *a){
  fputs(a, stdout);
}

int v_esp; int v_ebp; int v_stack_size; int v_stack;

int init_c(void){
  v_stack_size=64*1024;
  v_stack=calloc(1,v_stack_size);
  v_esp=v_stack+v_stack_size-4;
  v_ebp=v_esp;
}

int mk_char(int c){
  return c;
}

int mk_c_string(int s){
  return s;
}

int dummy(void){
  puts("dummy called");
}

int v_alloca(int x);
int enter(void);
int leave(int x);

int puts(char *a){
  fputs(a, stdout);
}
int isalnum(int a){
  puts("isalnum not impl");
  exit(1);
}
int isspace(int a){
  puts("isspace not impl");
  exit(1);
}
int isdigit(int a){
  puts("isdigit not impl");
  exit(1);
}
int strtol(int a, int b, int c){
  puts("strtol not impl");
  exit(1);
}
int strstr(int a, int b){
  puts("strstr not impl");
  exit(1);
}
int memcpy(int a, int b, int c){
  puts("memcpy not impl");
  exit(1);
}
int strcpy(int a, int b){
  puts("strcpy not impl");
  exit(1);
}
int fwrite(int a, int b, int c, int d){
  puts("fwrite not impl");
  exit(1);
}
int printf(int a){
  puts("printf not impl");
  exit(1);
}
