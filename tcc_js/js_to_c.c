#include "elf_loader_support_tcc.c"

int l_size;

int wu8(int o, int v) {
  char *b;
  b=o;
  b[0] = v & 0xFF;
}

int init_globals(void){
  l_size=256;
}

int process_line(int l) {
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

int main(void){
  init_c();
  init_globals();
  process_file("tcc.js");
  return 0;
}
