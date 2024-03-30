char* heap;
int int_size;

int ri8(int o) {
/*  int t=heap[a]; */
  int o1=o>>2;
  int s=o&3;
  int v1;
  int *h=heap;
  v1=h[o1*int_size];
  return (v1>>(s*8)) &0xFF;
}

int wi8(int o,int v) {
  heap[o]=v;
  return;
}


int expr(void);
int decl(int l);

int puts(char *a){
  fputs(a, stdout);
  fputs("\n", stdout);
}

int v_esp; int v_ebp; int v_stack_size; int v_stack;

int init_c(void){
  puts("init_c called");
  v_stack_size=64*1024;
  v_stack=calloc(1,v_stack_size);
  v_esp=v_stack+v_stack_size-4;
  v_ebp=v_esp;
  heap = 0;
  int_size = 1;
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

int wi32(int o, int v) {
  wi8(o,v&0xFF);
  v=v>>8;
  wi8(o+1,v&0xFF);
  v=v>>8;
  wi8(o+2,v&0xFF);
  v=v>>8;
  wi8(o+3,v&0xFF);
}

int ri32(int o) {
  return (ri8(o)&255)       | (ri8(o+1)&255)<<8 |
         (ri8(o+2)&255)<<16 | (ri8(o+3)&255)<<24;
}

int v_alloca(int x) {
  v_esp=v_esp-x;
  return v_esp;
}

int enter(void) {
/* FIXME detect overflow */
  v_esp=v_esp-4;
  wi32(v_esp,v_ebp);
  v_ebp=v_esp;
}

int leave(int x) {
  v_esp=v_ebp;
  v_ebp=ri32(v_esp);
  v_esp=v_esp+4;
  return x;
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
int strcpy(int dest, int src){
  int c; int p;
  p = dest;
  c = 0;
  while(1){
    c=ri8(src);
    if(c == 0){
      break;
    }
    src = src + 1;
    wi8(dest,c);
    dest = dest + 1;
  }
  return p;
}

int fwrite(int a, int b, int c, int d){
  puts("fwrite not impl");
  exit(1);
}
int printf(int a){
  puts("printf not impl");
  exit(1);
}
