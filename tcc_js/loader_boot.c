typedef struct __FILE FILE;

int prog_rel;
int data_rel;

int global_relocs_table_base;
int global_relocs_table;
int relocs_base;
int global_relocs_base;
int glo,glo_base,prog,ind;
#define DATA_SIZE (256*1024)
#define TEXT_SIZE (256*1024)
#define RELOC_ADDR32 1  /* 32 bits relocation */
#define RELOC_REL32  2  /* 32 bits relative relocation */

int r32(int o){
  int r;
  r=((int *)(o))[0];
  return r;
}

int w32(int o,int v){
  *(int *)o=v;
}

int load_obj(void){
  printf("Loading object file\n");
  FILE *f;
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
    printf("sync m0 %x\n",t);
    exit(1);
  }
  global_relocs_table_base=(int)malloc(global_reloc_table_len);
  global_relocs_table=global_relocs_table_base;
  fread((void *)global_relocs_table_base,1,global_reloc_table_len,f);
  prog_rel=(int)malloc(text_len);
  data_rel=(int)malloc(data_len);

  fread(&t,1,4,f);
  if(!(t==m1)){
    printf("sync m1 %x\n",t);
    exit(1);
  }
  relocs_base=(int)malloc(reloc_len);
  fread((void *)relocs_base,1,reloc_len,f);
  fread(&t,1,4,f);
  if(!(t==m2)){
    printf("sync m2 %x\n",t);
    exit(1);
  }

  fread((void *)data_rel,1,data_len,f);

  fread(&t,1,4,f);
  if(!(t==m3)){
    printf("sync m3 %x\n",t);
    exit(1);
  }


  global_relocs_base=(int)malloc(global_reloc_len);
  fread((void *)global_relocs_base,1,global_reloc_len,f);

  fread(&t,1,4,f);
  if(!(t==m4)){
    printf("sync m4 %x\n",t);
    exit(1);
  }
  fread((void *)prog_rel,1,text_len,f);
  glo = malloc(DATA_SIZE);
  glo_base=glo;
  printf("glo: %x %x\n",glo,glo_base);
  memset((void *)glo, 0, DATA_SIZE);
  prog = malloc(TEXT_SIZE);
  ind = prog;
  printf("prog: %x \n",prog);
  memcpy((char *)prog,(char *)prog_rel,text_len);
  memcpy((char *)glo_base,(char *)data_rel,data_len);
  printf("entrypoint: %x\n",(prog+entrypoint));
  printf("text_len: %x\n",(text_len));
  printf("data_len: %x\n",(data_len));
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
  while(global_relocs_table<m){
    l=strlen((char *)global_relocs_table);
    a=dlsym(0,(char *)global_relocs_table);
    printf("global_reloc: %s %d %x ",global_relocs_table,l,a);
    global_relocs_table+=l+1;
    n=r32(global_relocs_table);
    global_relocs_table+=4;
    printf("global_reloc_num: %d\n",n);
    for(i=0;i<n;i++){
      off=r32(global_relocs_base+goff+4);
      addr=(unsigned int)(off+prog);
      switch(r32(global_relocs_base+goff)) {
        case RELOC_ADDR32:
          printf("Reloc type RELOC_ADDR32 at %x\n",addr);
          *(int *)addr=a;
          break;
        case RELOC_REL32:
          printf("Reloc type RELOC_REL32 at %x\n",addr);
          *(int *)addr = a - addr - 4;
          break;
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

  printf("running loader\n");
  return (*t)(argc - optind, argv + optind);
}
