#include "elf_loader_support_tcc.c"

int l_size;
int ob;
int obo;

int wu8(int o, int v) {
  char *b;
  b=o;
  b[0] = v & 0xFF;
}

int init_globals(void){
  l_size=256;
  ob=malloc(1024*1024);
  obo=0;
}

int memcmp(int s1, int s2, int n) {
  int i;
  int r;
  char *p1;
  char *p2;
  r=0;
  p1=s1;
  p2=s2;
  for(i=0;i<n;i=i+1){
    if(p1[i]!=p2[i]){
/* FIXME ljw not quite right */
      r=1;
    }
  }
  return r;
}

int process_global_var(int l){
  puts("/* global var */");
  puts(l);
}

int process_load(int l){
  puts("/* process_load */");
  puts(l);
}

int process_line(int l) {
  if(memcmp("var ",l,4)==0){
    process_global_var(l);
    return;
  }
  if(memcmp("load(",l,4)==0){
    process_load(l);
    return;
  }
  puts(l);
}

int process_file(int name){
  int f;
  int c;
  int lb;
  int lo;
  lb=malloc(l_size);
  f=fopen(name,"rb");
  lo=0;
  while((c=fgetc(f)) != (-1)) {
    wu8(lb+lo,c);
    if(c=='\n'){
      wu8(lb+lo,0);
      lo=0;
      process_line(lb);
    } else {
      lo=lo+1;
    }
  }
  free(lb); 
  return 0;
}

int print_fn_decls(void){
  puts("");
  puts("/* fn decls */");

}

int print_declare_globals(void){
  puts("");
  puts("/* declare globals */");

}

int print_init_globals(void){
  puts("");
  puts("/* init globals */");

}

int print_converted(void){
  puts("");
  puts("/* js_to_c converted code */");

}

int main(void){
  init_c();
  init_globals();
  process_file("tcc.js");

  puts("#include \"support.c\"");
  print_fn_decls();
  print_declare_globals();
  print_init_globals();
  print_converted();
  
  return 0;
}
