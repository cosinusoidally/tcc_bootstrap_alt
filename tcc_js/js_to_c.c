#include "elf_loader_support_tcc.c"

int memcpy(int s1, int s2, int n);

int l_size;
int ob;
int obo;

int gib;
int gibo;

int wu8(int o, int v) {
  char *b;
  b=o;
  b[0] = v & 0xFF;
}

int init_globals(void){
  l_size=256;
  ob=calloc(1024*1024,1);
  obo=0;
  gib=calloc(1024*1024,1);
  gibo=0;
}

int oputs(int s){
  int l;
  l=strlen(s);
  memcpy(ob+obo,s,l);
  obo=obo+l;
}

int giputs(int s){
  int l;
  l=strlen(s);
  memcpy(gib+gibo,s,l);
  gibo=gibo+l;
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

int fwrite(int ptr,int size, int nitems, int stream) {
  int t=size*nitems;
  char *c=ptr;
  while(t>0){
    fputc(c[0],stream);
    t=t-1;
    c=c+1;
  }
}


int process_global_var(int l){
  char *p;
  char c;
  l=l+4;
  p=l;
  giputs("  ");
  giputs(l);
  giputs("\n");
  while((c=p[0])!=0){
    if(c=='='){
      p[0]=';';
      p[1]=0;
      break;
    }
    p=p+1;
  }
  puts(l);
}

int process_local_var(int l,int n){
  int i;
  for(i=0;i<n;i=i+1){
    oputs(" ");
  }
  oputs("int ");
  oputs(l);
  oputs("\n");
}

int process_load(int l){
  oputs("/* process_load */");
  oputs("\n");
  oputs(l);
  oputs("\n");
}

int eat_whitespace(int l){
  char *o;
  o=l;
  while(memcmp(" ",o,1)==0){
    o=o+1;
  }
  return o;
}

int num_whitespace(int l){
  char *o;
  o=l;
  while(memcmp(" ",o,1)==0){
    o=o+1;
  }
  return o-l;
}

int process_function(int l){
  oputs("/* ");
  oputs(l);
  oputs(" */");
  oputs("\n");
  l=l+9;
  oputs("int ");
  oputs(l);
  oputs("\n");
}

int process_line(int l) {
  int t;
  int n;
  if(memcmp("var ",l,4)==0){
    process_global_var(l);
    return;
  }
  if(memcmp("load(",l,4)==0){
    process_load(l);
    return;
  }
  if(memcmp("function ",l,9)==0){
    process_function(l);
    return;
  }
  t=eat_whitespace(l);
  n=num_whitespace(l);
  if(memcmp("// ",t,3)==0){
    return;
  }
  if(memcmp("print(",t,6)==0){
    return;
  }
  if(memcmp("var ",t,4)==0){
    process_local_var(t+4,n);
    return;
  }
  oputs(l);
  oputs("\n");
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
  puts("void init_globals(void) {");
  fputs(gib, stdout);
  puts("}");
}

int print_converted(void){
  puts("");
  puts("/* js_to_c converted code */");
  puts("");
  fwrite(ob,1,obo,stdout);
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
