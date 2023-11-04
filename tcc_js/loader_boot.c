/* decls for real functions */
void *malloc(int size);
int strlen(char *s);
int fopen(int path, int mode);
int fclose(int f);
int fread(int a,int b, int c, int d);
int puts(char *a);
int dlsym(int h, int sym);
int exit(int x);
int memset(int a, int b, int c);
int memcpy(int a, int b, int c);

int prog_rel;
int data_rel;

int global_relocs_table_base;
int global_relocs_table;
int relocs_base;
int global_relocs_base;
int glo;
int glo_base;
int prog;
int ind;

int DATA_SIZE;
int TEXT_SIZE;
int RELOC_ADDR32;
int RELOC_REL32;

int init_globals(void){
  DATA_SIZE = (256*1024);
  TEXT_SIZE = (256*1024);
  RELOC_ADDR32 = 1; /* 32 bits relocation */
  RELOC_REL32 = 2; /* 32 bits relative relocation */
}

int r32(int o){
  int r;
  int *p;
  p=o;
  r=p[0];
  return r;
}

int w32(int o,int v){
  int *p;
  p=o;
  p[0]=v;
}


int dlsym_wrap(int h,int sym){
  return dlsym(h,sym);
}

int load_obj(void){
  puts("Loading object file");
  int f;
  int text_len;
  int data_len;
  int reloc_len;
  int global_reloc_len;
  int global_reloc_table_len;
  int entrypoint;
  int m0=0xdeadbe00;
  int m1=0xdeadbe01;
  int m2=0xdeadbe02;
  int m3=0xdeadbe03;
  int m4=0xdeadbe04;
  int i;
  int t;
  f = fopen("tcc_boot.o", "rb");
  fread(&entrypoint,1,4,f);
  fread(&text_len,1,4,f);
  fread(&data_len,1,4,f);
  fread(&reloc_len,1,4,f);
  fread(&global_reloc_len,1,4,f);
  fread(&global_reloc_table_len,1,4,f);
  fread(&t,1,4,f);
  if(!(t==m0)){
    puts("sync m0");
    exit(1);
  }
  global_relocs_table_base=malloc(global_reloc_table_len);
  global_relocs_table=global_relocs_table_base;
  fread(global_relocs_table_base,1,global_reloc_table_len,f);
  prog_rel=malloc(text_len);
  data_rel=malloc(data_len);

  fread(&t,1,4,f);
  if(!(t==m1)){
    puts("sync m1");
    exit(1);
  }
  relocs_base=malloc(reloc_len);
  fread(relocs_base,1,reloc_len,f);
  fread(&t,1,4,f);
  if(!(t==m2)){
    puts("sync m2");
    exit(1);
  }

  fread(data_rel,1,data_len,f);

  fread(&t,1,4,f);
  if(!(t==m3)){
    puts("sync m3");
    exit(1);
  }


  global_relocs_base=malloc(global_reloc_len);
  fread(global_relocs_base,1,global_reloc_len,f);

  fread(&t,1,4,f);
  if(!(t==m4)){
    puts("sync m4");
    exit(1);
  }
  fread(prog_rel,1,text_len,f);
  glo = malloc(DATA_SIZE);
  glo_base=glo;
/*  printf("glo: %x %x\n",glo,glo_base); */
  memset(glo, 0, DATA_SIZE);
  prog = malloc(TEXT_SIZE);
  ind = prog;
/*  printf("prog: %x \n",prog); */
  memcpy(prog, prog_rel, text_len);
  memcpy(glo_base, data_rel, data_len);
/*  printf("entrypoint: %x\n",(prog+entrypoint)); */
/*  printf("text_len: %x\n",(text_len)); */
/*  printf("data_len: %x\n",(data_len)); */
  fclose(f);

  int m=global_relocs_table_base+global_reloc_table_len;
  int l;
  int a;
  int n;
  int p;
  for(i=0;i<reloc_len;i=i+12){
    if(relocs_base+i+8==0){
      p=prog;
    } else {
      p=glo;
    }
    w32(prog+r32(relocs_base+i),r32(relocs_base+i+4)+p); 
  }
  int goff=0;
  int off;
  int addr;
  int reloc_type;
  int *ptr;
  while(global_relocs_table<m){
    l=strlen(global_relocs_table);
    a=dlsym_wrap(0,global_relocs_table);
/*    printf("global_reloc: %s %d %x ",global_relocs_table,l,a); */
    global_relocs_table+=l+1;
    n=r32(global_relocs_table);
    global_relocs_table+=4;
/*    printf("global_reloc_num: %d\n",n); */
    for(i=0;i<n;i=i+1){
      off=r32(global_relocs_base+goff+4);
      addr = (off+prog);
      ptr = addr;
      reloc_type=r32(global_relocs_base+goff);
      if(reloc_type == RELOC_ADDR32) {
/*        printf("Reloc type RELOC_ADDR32 at %x\n",addr); */
        ptr[0] = a;
      }
      if(reloc_type == RELOC_REL32) {
/*        printf("Reloc type RELOC_REL32 at %x\n",addr); */
        ptr[0] = a - addr - 4;
      }
      goff=goff+8;
    }
  }
  return prog+entrypoint;
}

int main(int argc, char **argv)
{
    int (*t)();
    char *p, *r, *outfile;
    int optind;

    optind = 1;
    int loader=0;

    init_globals();

    while (1) {
        r = argv[optind];
        if (r[0] != '-')
            break;
        optind++;
        if(r[1] == 'R') {
            t=(int (*)())load_obj();
            loader=1;
        } else {
            puts("option error");
            exit(1);
        }
    }

  puts("running loader");
  return (*t)(argc - optind, argv + optind);
}
