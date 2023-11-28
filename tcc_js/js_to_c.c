#include "elf_loader_support_tcc.c"

int process_file(int name){
  int f;
  int c;
  int lb;
  lb=malloc(256);
  f=fopen(name,"rb");
  while((c=fgetc(f)) != (-1)) {
    fputc(c,stdout);
  } 
  free(lb); 
  return 0;
}

int main(void){
  init_c();
  process_file("tcc.js");
  return 0;
}
