#include "elf_loader_support_tcc.c"

char *elf_buf;
int sh_name_o;
int sh_size_o;
int sh_entsize_o;
int sh_offset_o;
int obj_name_o;
int obj_text_o;
int obj_data_o;
int obj_bss_o;
int obj_strtab_o;
int obj_symtab_o;
int obj_rel_text_o;
int obj_text_size_o;
int obj_data_size_o;
int obj_bss_size_o;
int obj_strtab_size_o;
int obj_symtab_size_o;
int obj_symtab_ent_size_o;
int obj_rel_text_size_o;
int obj_linked_o;
int obj_exports_o;
int obj_und_o;
int obj_struct_size;
int r_info_o;
int st_name_o;
int exp_name_o;
int exp_address_o;
int exp_size;
int und_name_o;
int und_val_o;
int und_size;
int st_name_o;
int st_value_o;
int st_info_o;
int st_shndx_o;
int STT_OBJECT;
int STT_FUNC;

int init_globals(void){
  elf_buf=malloc(256*1024);
}

int ru8(int o) {
  char *b;
  b=o;
  return b[0] & 0xFF;
}

int ri32(int o){
  int r;
  r=ru8(o+3);
  r=r << 8;
  r = r | ru8(o+2);
  r=r << 8;
  r = r | ru8(o+1);
  r=r << 8;
  r = r | ru8(o);
  return r;
}

void hex_dump(int e,int l){
  int i;
  int j;
  int k;
  int off;
  int off_l;
  int v;

  i=0;
  while(i<l) {
    off=int2str(i,16,0);
    off_l=strlen(off);
    for(k=0;k<8-off_l;k=k+1){
      fputc('0',stdout);
    }
    fputs(off,stdout);
    fputs(": ",stdout);
    j=0;
    while(j<8) { 
      v=ru8(e+i);
      if(v<16) {
        fputc('0', stdout);
      }
      fputs(int2str(v,16,0), stdout);
      v=ru8(e+i+1);
      if(v<16) {
        fputc('0', stdout);
      }
      fputs(int2str(v,16,0), stdout);
      fputs(" ", stdout);
      i=i+2;
      j=j+1;
    }
    fputs(" ", stdout);
    i=i-16;
    j=0;
    while(j<16) {
      v=ru8(e+i);
      if(((' ' <= v) !=0) && ((v <= '~')!=0)) {
        fputc(v, stdout);
      } else {
        fputc('.', stdout);
      }
      i=i+1;
      j=j+1;
    }
    fputs("\n",stdout);
  }
  fputs("\n", stdout);
}

int get_section_header(int e, char *str) {
  int e_shoff;
  int e_shentsize;
  int e_shnum;
  int e_shstrndx;
  int sh_offset;
  int sh_name;
  int sh_name_str;
  int i;
  int j;
  int o;
  int sl;
  e_shoff=ri32(e+0x20);
  e_shentsize=ri32(e+0x2E) & 0xFFFF;
  e_shnum=ri32(e+0x30) & 0xFFFF;
  e_shstrndx=ri32(e+0x32) & 0xFFFF;
  puts(".shstrtab:");
  o=e+e_shoff+(e_shstrndx*e_shentsize);
  sh_offset=ri32(o+16);
  fputs("get_section: ",stdout);
  fputs(str,stdout);
  fputs("\n", stdout);
  o=e_shoff;
  for(i=0;i<e_shnum;i=i+1){
    sh_name=ri32(e+o);
    fputs("sh_name: ",stdout);
    fputs(int2str(sh_name,16,0),stdout);
    sh_name_str=e+sh_offset+sh_name;
    fputs(" sh_name_str: ",stdout);
    fputs(sh_name_str,stdout);
    fputs("\n",stdout);
    if(strcmp(str,sh_name_str) == 0){
      return e+o;
    }
    o=o+e_shentsize;
  }
  puts("section header not found");
  return 0;
}



void init_offsets(void){
  sh_name_o=0;
  sh_size_o=20;
  sh_entsize_o=36;
  sh_offset_o=16;
  obj_struct_size=4*32;
  obj_name_o=0;
  obj_text_o=1;
  obj_data_o=2;
  obj_bss_o=3;
  obj_text_size_o=4;
  obj_data_size_o=5;
  obj_bss_size_o=6;
  obj_strtab_o=7;
  obj_strtab_size_o=8;
  obj_symtab_o=9;
  obj_symtab_size_o=10;
  obj_symtab_ent_size_o=11;
  obj_rel_text_o=12;
  obj_rel_text_size_o=13;
  obj_linked_o=14;
  obj_exports_o=15;
  obj_und_o=16;
  r_info_o=4;
  st_name_o=0;
  exp_name_o=0;
  exp_address_o=1;
  exp_size=8;
  und_name_o=0;
  und_val_o=1;
  und_size=8;
  st_name_o=0;
  st_value_o=4;
  st_info_o=0xC;
  st_shndx_o=0xE;
  STT_OBJECT=1;
  STT_FUNC=2;
}
void print_relocs(char *name,int *o){
  int i;
  int r_info;
  int r_sym;
  int ptr;
  int size;
  int sym_name;
  ptr=o[obj_rel_text_o];
  size=o[obj_rel_text_size_o];
  fputs("\n",stdout);
  fputs(name,stdout);
  fputs("\n",stdout);
  for(i=0;i<size;i=i+8){
    r_info=ri32(ptr+i+r_info_o);
    /* FIXME use unsigned shift */
    r_sym=r_info>>8;
    fputs("r_info: 0x",stdout);
    fputs(int2str(r_sym,16,0),stdout);
    fputs("\n",stdout);
    sym_name=o[obj_strtab_o]+ri32(o[obj_symtab_o]+(16*r_sym));
    fputs("sym_name: ",stdout);
    fputs(sym_name,stdout);
    fputs("\n",stdout);
    hex_dump(ptr+i,8);
  }
  fputs("\n",stdout);

}

int decode_elf(int e, int os){
  int e_shoff;
  int e_shentsize;
  int e_shnum;
  int e_shstrndx;
  int i;
  int j;
  int o;
  int sh_type;
  int sh_offset;
  int sh_size;
  int sh_entsize;
  int sl;
  int text;
  int data;
  int strtab;
  int symtab;
  int rel_text;
  int text_mem;
  int data_mem;
  int strtab_mem;
  int symtab_mem;
  int rel_text_mem;
  int *obj_struct;

  obj_struct=os;

  if(ru8(e+0)!=0x7F) { puts("magic 0");exit(1);}
  if(ru8(e+1)!='E') { puts("magic 1");exit(1);}
  if(ru8(e+2)!='L') { puts("magic 2");exit(1);}
  if(ru8(e+3)!='F') { puts("magic 3");exit(1);}
  puts("ELF magic ok");
  e_shoff=ri32(e+0x20);
  e_shentsize=ri32(e+0x2E) & 0xFFFF;
  e_shnum=ri32(e+0x30) & 0xFFFF;
  e_shstrndx=ri32(e+0x32) & 0xFFFF;
  text=get_section_header(e,".text");
  data=get_section_header(e,".data");
  strtab=get_section_header(e,".strtab");
  symtab=get_section_header(e,".symtab");
  rel_text=get_section_header(e,".rel.text");
  fputs("e_shoff: ",stdout);
  fputs(int2str(e_shoff,10,0),stdout);
  fputs("\n",stdout);
  fputs("e_shentsize: ",stdout);
  fputs(int2str(e_shentsize,10,0),stdout);
  fputs("\n",stdout);
  fputs("e_shnum: ",stdout);
  fputs(int2str(e_shnum,10,0),stdout);
  fputs("\n",stdout);
  fputs("e_shstrndx: ",stdout);
  fputs(int2str(e_shstrndx,10,0),stdout);
  fputs("\n",stdout);
  fputs("\n",stdout);
  o=e_shoff;
  for(i=0;i<e_shnum;i=i+1){
    fputs("sh_name: ",stdout);
    fputs(int2str(ri32(e+o),16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_type: ",stdout);
    sh_type=ri32(e+o+4);
    fputs(int2str(sh_type,16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_offset: ",stdout);
    sh_offset=ri32(e+o+16);
    fputs(int2str(sh_offset,16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_size: ",stdout);
    sh_size=ri32(e+o+20);
    fputs(int2str(sh_size,16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_entsize: ",stdout);
    fputs(int2str(ri32(e+o+36),16,0),stdout);
    fputs("\n",stdout);
    if(sh_type==3){
      puts("SHT_STRTAB");
    }
    if(i==e_shstrndx){
      puts(".shstrtab:");
      o=e+sh_offset;
      for(j=0;j<e_shnum;j=j+1){
        sl=strlen(o);
        fputs(int2str(sl,10,0),stdout);
        fputs(" ",stdout);
        fputs(o,stdout);
        o=o+sl+1;
        fputs("\n",stdout);
      }
    }
    fputs("\n",stdout);
    o=o+e_shentsize;
  }

  fputs(".text:\n",stdout);
  fputs("sh_name: 0x",stdout);
  fputs(int2str(ri32(text+sh_name_o),16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_offset: 0x",stdout);
  sh_offset=ri32(text+sh_offset_o);
  fputs(int2str(sh_offset,16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_size: 0x",stdout);
  sh_size=ri32(text+sh_size_o);
  fputs(int2str(sh_size,16,0),stdout);
  fputs("\n",stdout);
  text_mem=malloc(sh_size);
  memcpy(text_mem,e+sh_offset,sh_size);
  fputs("text_mem address: 0x",stdout);
  fputs(int2str(text_mem,16,0),stdout);
  fputs("\n",stdout);
  hex_dump(text_mem,sh_size);
  fputs("\n",stdout);
  obj_struct[obj_text_o]=text_mem;
  obj_struct[obj_text_size_o]=sh_size;

  fputs(".data:\n",stdout);
  fputs("sh_name: 0x",stdout);
  fputs(int2str(ri32(data+sh_name_o),16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_offset: 0x",stdout);
  sh_offset=ri32(data+sh_offset_o);
  fputs(int2str(sh_offset,16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_size: 0x",stdout);
  sh_size=ri32(data+sh_size_o);
  fputs(int2str(sh_size,16,0),stdout);
  fputs("\n",stdout);
  data_mem=malloc(sh_size);
  memcpy(data_mem,e+sh_offset,sh_size);
  fputs("data_mem address: 0x",stdout);
  fputs(int2str(data_mem,16,0),stdout);
  fputs("\n",stdout);
  hex_dump(data_mem,sh_size);
  obj_struct[obj_data_o]=data_mem;
  obj_struct[obj_data_size_o]=sh_size;

  fputs(".strtab:\n",stdout);
  fputs("sh_name: 0x",stdout);
  fputs(int2str(ri32(strtab+sh_name_o),16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_offset: 0x",stdout);
  sh_offset=ri32(strtab+sh_offset_o);
  fputs(int2str(sh_offset,16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_size: 0x",stdout);
  sh_size=ri32(strtab+sh_size_o);
  fputs(int2str(sh_size,16,0),stdout);
  fputs("\n",stdout);
  strtab_mem=malloc(sh_size);
  memcpy(strtab_mem,e+sh_offset,sh_size);
  fputs("strtab_mem address: 0x",stdout);
  fputs(int2str(strtab_mem,16,0),stdout);
  fputs("\n",stdout);
  hex_dump(strtab_mem,sh_size);
  obj_struct[obj_strtab_o]=strtab_mem;
  obj_struct[obj_strtab_size_o]=sh_size;

  fputs(".symtab:\n",stdout);
  fputs("sh_name: 0x",stdout);
  fputs(int2str(ri32(symtab+sh_name_o),16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_offset: 0x",stdout);
  sh_offset=ri32(symtab+sh_offset_o);
  fputs(int2str(sh_offset,16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_size: 0x",stdout);
  sh_size=ri32(symtab+sh_size_o);
  fputs(int2str(sh_size,16,0),stdout);
  fputs("\n",stdout);
  sh_entsize=ri32(symtab+sh_entsize_o);
  fputs("sh_entsize: 0x",stdout);
  fputs(int2str(sh_entsize,16,0),stdout);
  fputs("\n",stdout);
  symtab_mem=malloc(sh_size);
  memcpy(symtab_mem,e+sh_offset,sh_size);
  fputs("symtab_mem address: 0x",stdout);
  fputs(int2str(symtab_mem,16,0),stdout);
  fputs("\n",stdout);
  hex_dump(symtab_mem,sh_size);
  obj_struct[obj_symtab_o]=symtab_mem;
  obj_struct[obj_symtab_size_o]=sh_size;

  fputs(".rel.text:\n",stdout);
  fputs("sh_name: 0x",stdout);
  fputs(int2str(ri32(rel_text+sh_name_o),16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_offset: 0x",stdout);
  sh_offset=ri32(rel_text+sh_offset_o);
  fputs(int2str(sh_offset,16,0),stdout);
  fputs("\n",stdout);
  fputs("sh_size: 0x",stdout);
  sh_size=ri32(rel_text+sh_size_o);
  fputs(int2str(sh_size,16,0),stdout);
  fputs("\n",stdout);
  sh_entsize=ri32(rel_text+sh_entsize_o);
  fputs("sh_entsize: 0x",stdout);
  fputs(int2str(sh_entsize,16,0),stdout);
  fputs("\n",stdout);
  rel_text_mem=malloc(sh_size);
  memcpy(rel_text_mem,e+sh_offset,sh_size);
  fputs("rel_text_mem address: 0x",stdout);
  fputs(int2str(rel_text_mem,16,0),stdout);
  fputs("\n",stdout);
  hex_dump(rel_text_mem,sh_size);
  obj_struct[obj_rel_text_o]=rel_text_mem;
  obj_struct[obj_rel_text_size_o]=sh_size;
  print_relocs(".rel.text", os);

  return os;
}

int load_elf(char *name){
  int f;
  int l;
  int c;
  int i;
  int j;
  int v;
  int k;
  int e;
  int off_l;
  char *off;
  int *obj_struct;
  obj_struct=calloc(obj_struct_size,1);
  obj_struct[obj_name_o]=name;
  obj_struct[obj_linked_o]=0;
  l=0;
  puts("loading elf file:");
  puts(name);
  f=fopen(name,"rb");
  while((c=fgetc(f)) >= 0){
    elf_buf[l]=c;
    l=l+1;
  }
  puts("file length");
  puts_num(l);
  e=malloc(l);
  memcpy(e,elf_buf,l);
  decode_elf(e, obj_struct);
/*  hex_dump(e,l); */
  return obj_struct;
}

int mk_host_obj(void){
  int *obj_struct;
  obj_struct=calloc(obj_struct_size,1);
  obj_struct[obj_name_o]="host.o";
  obj_struct[obj_linked_o]=1;
  return obj_struct;
}

int get_main(void){
  char *m;
  m=malloc(1024);
  m[0]=0xB8;
  m[1]=0;
  m[2]=0;
  m[3]=0;
  m[4]=0;
  m[5]=0xC3;
  return m;
}

int reloc_internal(int o){
  puts("reloc_internal");
}

int gen_und_exports(int o){
  int *obj=o;
  int symtab;
  int symtab_size;
  int entsize;
  int i;
  int sym;
  int st_name;
  int st_name_str;
  int st_value;
  int st_info;
  int st_bind;
  int st_type;
  int st_shndx;
  symtab=obj[obj_symtab_o];
  symtab_size=obj[obj_symtab_size_o];
  entsize=16;
  puts("gen_und_exports (export table and undefined symbol table)");
  /* FIXME this should not be 16 it should be calculated */
  obj[obj_exports_o]=calloc(exp_size*16,1);
  obj[obj_und_o]=calloc(und_size*16,1);
  for(i=0;i<obj[obj_symtab_size_o];i=i+entsize){
    sym=i+symtab;
    hex_dump(sym,entsize);
    st_name=ri32(sym+st_name_o);
    st_name_str=obj[obj_strtab_o]+st_name;
    st_value=ri32(sym+st_value_o);
    st_info=ru8(sym+st_info_o);
    st_type=st_info & 0xF;
    st_bind=st_info>>4;
    st_shndx=ri32(sym+st_shndx_o) & 0xFFFF;

    fputs("st_name: 0x",stdout);fputs(int2str(st_name,16,0),stdout);
    fputs("\n",stdout);
    fputs("st_name_str: ",stdout);fputs(st_name_str,stdout);
    fputs("\n",stdout);
    fputs("st_value: 0x",stdout);fputs(int2str(st_value,16,0),stdout);
    fputs("\n",stdout);
    fputs("st_info: 0x",stdout);fputs(int2str(st_info,16,0),stdout);
    fputs("\n",stdout);
    fputs("st_type: 0x",stdout);fputs(int2str(st_type,16,0),stdout);
    fputs("\n",stdout);
    fputs("st_bind: 0x",stdout);fputs(int2str(st_bind,16,0),stdout);
    fputs("\n",stdout);
    fputs("st_shndx: 0x",stdout);fputs(int2str(st_shndx,16,0),stdout);
    fputs("\n",stdout);


    if((st_type==STT_OBJECT) | (st_type==STT_FUNC)){
      puts("OBJECT or FUNCTION");
      if(st_shndx==0){
        puts("UND");
      } else {
        puts("export");
      }
    }
  }
}

int resolve_und(int o){
  puts("resolve_und");
}

int link(int o){
  int *objs;
  int *obj;
  int name;
  int i;
  objs=o;
  i=0;
  puts("============================");
  puts("linking");
  while(obj=objs[i]){
    name=obj[obj_name_o];
    puts(name);
    if(obj[obj_linked_o]!=0){
      fputs("already linked\n",stdout);
    } else {
      fputs("linking\n",stdout);
      reloc_internal(obj);
      gen_und_exports(obj);
    }
    puts("");
    i=i+1;
  }
  resolve_und(o);
}

int main(int argc, char **argv)
{
  FUNCTION t;
  int optind;
  int *objs;
  objs=calloc(12,1);

  puts("elf loader starting");

  optind = 1;

  init_c();
  init_globals();
  init_offsets();

  puts("running elf files");
  objs[0]=mk_host_obj();
  objs[1]=load_elf("elf_test.o");
  link(objs);
  puts(argv[optind]);
  t=get_main();
  return call_wrap(t, argc - optind, argv + (p_size*optind));
}
