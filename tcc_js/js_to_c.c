#include "elf_loader_support_tcc.c"

int memcpy(int s1, int s2, int n);
int process_file(int name);

int l_size;
int ob;
int obo;

int gib;
int gibo;

int gdb;
int gdbo;

int db;
int dbo;

int infile;
int outfd;

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
  gdb=calloc(1024*1024,1);
  gdbo=0;
  db=calloc(1024*1024,1);
  dbo=0;
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

int gdputs(int s){
  int l;
  l=strlen(s);
  memcpy(gdb+gdbo,s,l);
  gdbo=gdbo+l;
}

int dbputs(int s){
  int l;
  l=strlen(s);
  memcpy(db+dbo,s,l);
  dbo=dbo+l;
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
  while((c=p[0])!=0){
    if(c=='='){
      giputs("    ");
      giputs(l);
      giputs("\n");
      p[0]=';';
      p[1]=0;
      break;
    }
    p=p+1;
  }
  gdputs("int ");
  gdputs(l);
  gdputs("\n");
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
  char *p;
  char c;
  p=l;

  while((c=p[0])!='"'){
    p=p+1;
  }
  p=p+1;
  l=p;
  while((c=p[0])!='"'){
    p=p+1;
  }
  p[0]=0;
  if(memcmp(l,"support.js",10)==0){
    return;
  }
  oputs("/* load: ");
  oputs(l);
  oputs(" */");
  oputs("\n");
  process_file(l);
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
  char *p;
  int *args;
  int args_n;
  int c;
  int i;
  int name;
  args_n=0;
  int decl;
  p=l;
  /* FIXME should be dynamic */
  args=malloc(20*4);
  oputs("/* ");
  oputs(l);
  oputs(" */");
  oputs("\n");
  l=l+9;
  decl=db+dbo;
  dbputs("int ");
  name=l;
  while((c=p[0])!='('){
    p=p+1;
  }
  p[0]=0;
  p=p+1;
  args[1]=p;
  while((c=p[0])!=')'){
    if(args_n==0){
      args_n=1;
    }
    if(c==','){
      args_n=args_n+1;
      p[0]=0;
      p=p+1;
      args[args_n]=p;
    } else {
      p=p+1;
    }
  }
  p[0]=0;
  p=p+1;
  dbputs(name);
  dbputs("(");
  if(args_n==0){
    dbputs("void");
  } else {
    dbputs("int ");
    dbputs(args[1]);
  }
  for(i=1;i<args_n;i=i+1){
    dbputs(", int ");
    dbputs(args[i+1]);
  }
  dbputs(")");
  oputs(decl);
  oputs(" {\n");
  dbputs(";\n");
  free(args);
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
  oputs("\n");
  fclose(f);
  return 0;
}

int print_fn_decls(void){
  fputs("\n",outfd);
  fputs("/* fn decls */",outfd);
  fputs("\n",outfd);
  fputs(db, outfd);
}

int print_declare_globals(void){
  fputs("/* declare globals */",outfd);
  fputs(gdb, outfd);
}

int print_init_globals(void){
  fputs("\n",outfd);
  fputs("/* init globals */",outfd);
  fputs("\n",outfd);
  fputs("void init_globals(void) {",outfd);
  fputs("\n",outfd);
  fputs(gib, outfd);
  fputs("}",outfd);
  fputs("\n",outfd);
  fputs("\n",outfd);
}

int print_converted(void){
  fwrite(ob,1,obo,outfd);
}

int main(int argc, int **argv){
  init_c();
  init_globals();
  if(argc==3){
    infile=argv[1];
    outfd=fopen(argv[2],"wb");
  } else {
    infile="tcc.js";
    outfd=stdout;
  }
  process_file(infile);

  fputs("#include \"support.c\"\n",outfd);
  print_fn_decls();
  print_declare_globals();
  print_init_globals();
  print_converted();
  
  return 0;
}
