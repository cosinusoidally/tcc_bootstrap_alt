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
  puts("process_load");
}

int process_line(int l) {
  if(memcmp("var ",l,4)==0){
    process_global_var(l);
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

int main(void){
  init_c();
  init_globals();
  process_file("tcc.js");
  return 0;
}
