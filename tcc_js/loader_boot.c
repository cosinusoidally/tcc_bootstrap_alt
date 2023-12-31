#include "loader_support_tcc.c"

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
  puts("dlsym lookup:");
  puts(sym);
  return dlsym(h,sym);
}

int load_obj(void){
  puts("Loading object file");
  int f;
  int *text_len=malloc(4);
  int *data_len=malloc(4);
  int *reloc_len=malloc(4);
  int *global_reloc_len=malloc(4);
  int *global_reloc_table_len=malloc(4);
  int *entrypoint=malloc(4);
  int m0=3735928320;
  int m1=3735928321;
  int m2=3735928322;
  int m3=3735928323;
  int m4=3735928324;
  int i;
  int *t=malloc(4);
  f = fopen("tcc_boot.o", "rb");
  fread(entrypoint,1,4,f);
  fread(text_len,1,4,f);
  fread(data_len,1,4,f);
  fread(reloc_len,1,4,f);
  fread(global_reloc_len,1,4,f);
  fread(global_reloc_table_len,1,4,f);
  fread(t,1,4,f);
  if(t[0]!=m0){
    puts("sync m0");
    exit(1);
  }
  global_relocs_table_base=malloc(global_reloc_table_len[0]);
  global_relocs_table=global_relocs_table_base;
  fread(global_relocs_table_base,1,global_reloc_table_len[0],f);
  prog_rel=malloc(text_len[0]);
  data_rel=malloc(data_len[0]);

  fread(t,1,4,f);
  if(t[0]!=m1){
    puts("sync m1");
    exit(1);
  }
  relocs_base=malloc(reloc_len[0]);
  fread(relocs_base,1,reloc_len[0],f);
  fread(t,1,4,f);
  if(t[0]!=m2){
    puts("sync m2");
    exit(1);
  }

  fread(data_rel,1,data_len[0],f);

  fread(t,1,4,f);
  if(t[0]!=m3){
    puts("sync m3");
    exit(1);
  }

  global_relocs_base=malloc(global_reloc_len[0]);
  fread(global_relocs_base,1,global_reloc_len[0],f);

  fread(t,1,4,f);
  if(t[0]!=m4){
    puts("sync m4");
    exit(1);
  }

  fread(prog_rel,1,text_len[0],f);
  glo = malloc(DATA_SIZE);
  glo_base=glo;
  memset(glo, 0, DATA_SIZE);
  prog = malloc(TEXT_SIZE);
  ind = prog;
  memcpy(prog, prog_rel, text_len[0]);
  memcpy(glo_base, data_rel, data_len[0]);
  fclose(f);

  int m=global_relocs_table_base+global_reloc_table_len[0];
  int l;
  int a;
  int n;
  int p;
  for(i=0;i<reloc_len[0];i=i+12){
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
    global_relocs_table = global_relocs_table + l + 1;
    n=r32(global_relocs_table);
    global_relocs_table = global_relocs_table+4;
    for(i=0;i<n;i=i+1){
      off=r32(global_relocs_base+goff+4);
      addr = (off+prog);
      ptr = addr;
      reloc_type=r32(global_relocs_base+goff);
      if(reloc_type == RELOC_ADDR32) {
        ptr[0] = a;
      }
      if(reloc_type == RELOC_REL32) {
        ptr[0] = a - addr - 4;
      }
      goff=goff+8;
    }
  }
  return prog+(entrypoint[0]);
}

int main(int argc, char **argv)
{
    FUNCTION t;
    char *p;
    char *r;
    char *outfile;
    int optind;
    int loader;

    init_c();
    init_globals();

    puts("loader starting");

    optind = 1;
    loader = 0;

    while (1) {
        r = argv[optind];
        if (r[0] != '-')
            break;
        optind = optind + 1;
        if(r[1] == 'R') {
            t=load_obj();
            loader=1;
        } else {
            puts("option error");
            exit(1);
        }
    }

  puts("running loader");
  return call_wrap(t, argc - optind, argv + (p_size*optind));
}
