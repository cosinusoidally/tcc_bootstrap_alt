#include "elf_loader_support_tcc.c"

int process_file(int name){
  int f;
  int c;
  int lb;
  lb=malloc(256);
  f=fopen(name,"rb");
  while((c=fgetc(f)) != (-1)) {

  } 
  free(lb); 
  return 0;
}

int main(void){
  init_c();
  puts("Hello world");
  process_file("tcc.js");
  return 0;
}
