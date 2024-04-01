#include "elf_loader_support_tcc.c"

int elf_buf;
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
int obj_rel_data_o;
int obj_text_size_o;
int obj_data_size_o;
int obj_bss_size_o;
int obj_strtab_size_o;
int obj_symtab_size_o;
int obj_symtab_ent_size_o;
int obj_rel_text_size_o;
int obj_rel_data_size_o;
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
int ST_GLOBAL;
int r_offset_o;
int R_386_32;
int R_386_PC32;

init_globals(){
/* FIXME this should be dynamic */
  elf_buf=malloc(1024*1024);
}

wi8(o, v){
  *(char*)o = (v & 255);
}

ri8(o){
 return *(char*)o;
}

wi32(o, v) {
  wi8(o,v&0xFF);
  v=v>>8;
  wi8(o+1,v&0xFF);
  v=v>>8;
  wi8(o+2,v&0xFF);
  v=v>>8;
  wi8(o+3,v&0xFF);
}

ri32(o) {
  return (ri8(o)&255)       | (ri8(o+1)&255)<<8 |
         (ri8(o+2)&255)<<16 | (ri8(o+3)&255)<<24;
}

hex_dump(e, l){
  int i;
  int j;
  int k;
  int off;
  int off_l;
  int v;

  if(verbose == 0){
    return;
  }

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
      v=ri8(e+i);
      if(v<16) {
        fputc('0', stdout);
      }
      fputs(int2str(v,16,0), stdout);
      v=ri8(e+i+1);
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
      v=ri8(e+i);
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

dump_symtab(o){
  if(verbose){
    puts("===========");
    puts("dump_symtab");
  }
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
  for(i=0;i<obj[obj_symtab_size_o];i=i+entsize){
    sym=i+symtab;
    hex_dump(sym,entsize);
    st_name=ri32(sym+st_name_o);
    st_name_str=obj[obj_strtab_o]+st_name;
    st_value=ri32(sym+st_value_o);
    st_info=ri8(sym+st_info_o);
    st_type=st_info & 0xF;
    st_bind=st_info>>4;
    st_shndx=ri32(sym+st_shndx_o) & 0xFFFF;
    if(verbose){
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
    }
  }
}

get_section_header(e, str) {
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
  if(verbose){puts(".shstrtab:");}
  o=e+e_shoff+(e_shstrndx*e_shentsize);
  sh_offset=ri32(o+16);
  if(verbose){
    fputs("get_section: ",stdout);
    fputs(str,stdout);
    fputs("\n", stdout);
  }
  o=e_shoff;
  for(i=0;i<e_shnum;i=i+1){
    sh_name=ri32(e+o);
    if(verbose){
      fputs("sh_name: ",stdout);
      fputs(int2str(sh_name,16,0),stdout);
    }
    sh_name_str=e+sh_offset+sh_name;
    if(verbose){
      fputs(" sh_name_str: ",stdout);
      fputs(sh_name_str,stdout);
      fputs("\n",stdout);
    }
    if(strcmp(str,sh_name_str) == 0){
      return e+o;
    }
    o=o+e_shentsize;
  }
  if(verbose){puts("section header not found");}
  return 0;
}

init_offsets(){
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
  obj_rel_data_o=17;
  obj_rel_data_size_o=18;
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
  ST_GLOBAL=1;
  r_offset_o=0;
  R_386_32=1;
  R_386_PC32=2;
}

print_relocs(name, o){
  int i;
  int r_info;
  int r_sym;
  int ptr;
  int size;
  int sym_name;
  if(verbose==0){
    return;
  }
  ptr=ri32(o + (4*obj_rel_text_o));
  size=ri32(o + (4*obj_rel_text_size_o));
  if(verbose){
    fputs("\n",stdout);
    fputs(name,stdout);
    fputs("\n",stdout);
  }
  for(i=0;i<size;i=i+8){
    r_info=ri32(ptr+i+r_info_o);
    /* FIXME use unsigned shift */
    r_sym=r_info>>8;
    if(verbose){
      fputs("r_info: 0x",stdout);
      fputs(int2str(r_sym,16,0),stdout);
      fputs("\n",stdout);
    }
    sym_name=ri32(o+ (4*obj_strtab_o))+
             ri32(ri32(o+(4*obj_symtab_o))+(16*r_sym));
    if(verbose){
      fputs("sym_name: ",stdout);
      fputs(sym_name,stdout);
      fputs("\n",stdout);
      hex_dump(ptr+i,8);
    }
  }
  if(verbose){
    fputs("\n",stdout);
  }
}

decode_elf(e, os){
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
  int bss;
  int strtab;
  int symtab;
  int rel_text;
  int rel_data;
  int text_mem;
  int data_mem;
  int bss_mem;
  int strtab_mem;
  int symtab_mem;
  int rel_text_mem;
  int rel_data_mem;
  int obj_struct;

  obj_struct=os;

  if(ri8(e+0)!=0x7F) { puts("magic 0");exit(1);}
  if(ri8(e+1)!='E') { puts("magic 1");exit(1);}
  if(ri8(e+2)!='L') { puts("magic 2");exit(1);}
  if(ri8(e+3)!='F') { puts("magic 3");exit(1);}
  if(verbose){puts("ELF magic ok");}
  e_shoff=ri32(e+0x20);
  e_shentsize=ri32(e+0x2E) & 0xFFFF;
  e_shnum=ri32(e+0x30) & 0xFFFF;
  e_shstrndx=ri32(e+0x32) & 0xFFFF;
  text=get_section_header(e,".text");
  data=get_section_header(e,".data");
  bss=get_section_header(e,".bss");
  strtab=get_section_header(e,".strtab");
  symtab=get_section_header(e,".symtab");
  rel_text=get_section_header(e,".rel.text");
  rel_data=get_section_header(e,".rel.data");
  if(verbose){
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
  }
  o=e_shoff;
  for(i=0;i<e_shnum;i=i+1){
    if(verbose){
      fputs("sh_name: ",stdout);
      fputs(int2str(ri32(e+o),16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_type: ",stdout);
    }
    sh_type=ri32(e+o+4);
    if(verbose){
      fputs(int2str(sh_type,16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_offset: ",stdout);
    }
    sh_offset=ri32(e+o+16);
    if(verbose){
      fputs(int2str(sh_offset,16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_size: ",stdout);
    }
    sh_size=ri32(e+o+20);
    if(verbose){
      fputs(int2str(sh_size,16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_entsize: ",stdout);
      fputs(int2str(ri32(e+o+36),16,0),stdout);
      fputs("\n",stdout);
    }
    if(sh_type==3){
      if(verbose){puts("SHT_STRTAB");}
    }
    if(i==e_shstrndx){
      if(verbose){puts(".shstrtab:");}
      o=e+sh_offset;
      for(j=0;j<e_shnum;j=j+1){
        sl=strlen(o);
        if(verbose){
          fputs(int2str(sl,10,0),stdout);
          fputs(" ",stdout);
          fputs(o,stdout);
        }
        o=o+sl+1;
        if(verbose){fputs("\n",stdout);}
      }
    }
    if(verbose){fputs("\n",stdout);}
    o=o+e_shentsize;
  }

  if(verbose){
    fputs(".text:\n",stdout);
    fputs("sh_name: 0x",stdout);
    fputs(int2str(ri32(text+sh_name_o),16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_offset: 0x",stdout);
  }
  sh_offset=ri32(text+sh_offset_o);
  if(verbose){
    fputs(int2str(sh_offset,16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_size: 0x",stdout);
  }
  sh_size=ri32(text+sh_size_o);
  if(verbose){
    fputs(int2str(sh_size,16,0),stdout);
    fputs("\n",stdout);
  }
  text_mem=malloc(sh_size);
  memcpy(text_mem,e+sh_offset,sh_size);
  if(verbose){
    fputs("text_mem address: 0x",stdout);
    fputs(int2str(text_mem,16,0),stdout);
    fputs("\n",stdout);
    hex_dump(text_mem,sh_size);
    fputs("\n",stdout);
  }
  wi32(obj_struct+(4*obj_text_o), text_mem);
  wi32(obj_struct+ (4*obj_text_size_o), sh_size);

  if(verbose){
    fputs(".data:\n",stdout);
    fputs("sh_name: 0x",stdout);
    fputs(int2str(ri32(data+sh_name_o),16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_offset: 0x",stdout);
  }
  sh_offset=ri32(data+sh_offset_o);
  if(verbose){
    fputs(int2str(sh_offset,16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_size: 0x",stdout);
  }
  sh_size=ri32(data+sh_size_o);
  if(verbose){
    fputs(int2str(sh_size,16,0),stdout);
    fputs("\n",stdout);
  }
  data_mem=malloc(sh_size);
  memcpy(data_mem,e+sh_offset,sh_size);
  if(verbose){
    fputs("data_mem address: 0x",stdout);
    fputs(int2str(data_mem,16,0),stdout);
    fputs("\n",stdout);
    hex_dump(data_mem,sh_size);
  }
  wi32(obj_struct + (4 * obj_data_o), data_mem);
  wi32(obj_struct + (4 * obj_data_size_o), sh_size);

  if(verbose){
    fputs(".strtab:\n",stdout);
    fputs("sh_name: 0x",stdout);
    fputs(int2str(ri32(strtab+sh_name_o),16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_offset: 0x",stdout);
  }
  sh_offset=ri32(strtab+sh_offset_o);
  if(verbose){
    fputs(int2str(sh_offset,16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_size: 0x",stdout);
  }
  sh_size=ri32(strtab+sh_size_o);
  if(verbose){
    fputs(int2str(sh_size,16,0),stdout);
    fputs("\n",stdout);
  }
  strtab_mem=malloc(sh_size);
  memcpy(strtab_mem,e+sh_offset,sh_size);
  if(verbose){
    fputs("strtab_mem address: 0x",stdout);
    fputs(int2str(strtab_mem,16,0),stdout);
    fputs("\n",stdout);
    hex_dump(strtab_mem,sh_size);
  }
  wi32(obj_struct + (4 * obj_strtab_o), strtab_mem);
  wi32(obj_struct + (4 * obj_strtab_size_o), sh_size);

  if(verbose){
    fputs(".symtab:\n",stdout);
    fputs("sh_name: 0x",stdout);
    fputs(int2str(ri32(symtab+sh_name_o),16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_offset: 0x",stdout);
  }
  sh_offset=ri32(symtab+sh_offset_o);
  if(verbose){
    fputs(int2str(sh_offset,16,0),stdout);
    fputs("\n",stdout);
    fputs("sh_size: 0x",stdout);
  }
  sh_size=ri32(symtab+sh_size_o);
  if(verbose){
    fputs(int2str(sh_size,16,0),stdout);
    fputs("\n",stdout);
  }
  sh_entsize=ri32(symtab+sh_entsize_o);
  if(verbose){
    fputs("sh_entsize: 0x",stdout);
    fputs(int2str(sh_entsize,16,0),stdout);
    fputs("\n",stdout);
  }
  symtab_mem=malloc(sh_size);
  memcpy(symtab_mem,e+sh_offset,sh_size);
  if(verbose){
    fputs("symtab_mem address: 0x",stdout);
    fputs(int2str(symtab_mem,16,0),stdout);
    fputs("\n",stdout);
    hex_dump(symtab_mem,sh_size);
  }
  wi32(obj_struct + (4 * obj_symtab_o), symtab_mem);
  wi32(obj_struct + (4 * obj_symtab_size_o), sh_size);

  if(rel_text!=0){
    if(verbose){
      fputs(".rel.text:\n",stdout);
      fputs("sh_name: 0x",stdout);
      fputs(int2str(ri32(rel_text+sh_name_o),16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_offset: 0x",stdout);
    }
    sh_offset=ri32(rel_text+sh_offset_o);
    if(verbose){
      fputs(int2str(sh_offset,16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_size: 0x",stdout);
    }
    sh_size=ri32(rel_text+sh_size_o);
    if(verbose){
      fputs(int2str(sh_size,16,0),stdout);
      fputs("\n",stdout);
    }
    sh_entsize=ri32(rel_text+sh_entsize_o);
    if(verbose){
      fputs("sh_entsize: 0x",stdout);
      fputs(int2str(sh_entsize,16,0),stdout);
      fputs("\n",stdout);
    }
    rel_text_mem=malloc(sh_size);
    memcpy(rel_text_mem,e+sh_offset,sh_size);
    if(verbose){
      fputs("rel_text_mem address: 0x",stdout);
      fputs(int2str(rel_text_mem,16,0),stdout);
      fputs("\n",stdout);
      hex_dump(rel_text_mem,sh_size);
    }
    wi32(obj_struct + (4 * obj_rel_text_o), rel_text_mem);
    wi32(obj_struct + (4 * obj_rel_text_size_o), sh_size);
    print_relocs(".rel.text", os);
  }

  if(rel_data!=0){
    if(verbose){
      fputs(".rel.data:\n",stdout);
      fputs("sh_name: 0x",stdout);
      fputs(int2str(ri32(rel_data+sh_name_o),16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_offset: 0x",stdout);
    }
    sh_offset=ri32(rel_data+sh_offset_o);
    if(verbose){
      fputs(int2str(sh_offset,16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_size: 0x",stdout);
    }
    sh_size=ri32(rel_data+sh_size_o);
    if(verbose){
      fputs(int2str(sh_size,16,0),stdout);
      fputs("\n",stdout);
    }
    sh_entsize=ri32(rel_data+sh_entsize_o);
    if(verbose){
      fputs("sh_entsize: 0x",stdout);
      fputs(int2str(sh_entsize,16,0),stdout);
      fputs("\n",stdout);
    }
    rel_data_mem=malloc(sh_size);
    memcpy(rel_data_mem,e+sh_offset,sh_size);
    if(verbose){
      fputs("rel_data_mem address: 0x",stdout);
      fputs(int2str(rel_data_mem,16,0),stdout);
      fputs("\n",stdout);
      hex_dump(rel_data_mem,sh_size);
    }
    wi32(obj_struct + (4 * obj_rel_data_o), rel_data_mem);
    wi32(obj_struct + (4 * obj_rel_data_size_o), sh_size);
  }

  if(bss!=0){
    if(verbose){
      fputs(".bss:\n",stdout);
      fputs("sh_name: 0x",stdout);
      fputs(int2str(ri32(bss+sh_name_o),16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_offset: 0x",stdout);
    }
    sh_offset=ri32(bss+sh_offset_o);
    if(verbose){
      fputs(int2str(sh_offset,16,0),stdout);
      fputs("\n",stdout);
      fputs("sh_size: 0x",stdout);
    }
    sh_size=ri32(bss+sh_size_o);
    if(verbose){
      fputs(int2str(sh_size,16,0),stdout);
      fputs("\n",stdout);
    }
    bss_mem=calloc(sh_size,1);
    if(verbose){
      fputs("bss_mem address: 0x",stdout);
      fputs(int2str(bss_mem,16,0),stdout);
      fputs("\n",stdout);
    }
    wi32(obj_struct + (4 * obj_bss_o), bss_mem);
    wi32(obj_struct + (4 * obj_bss_size_o), sh_size);
  }
  return os;
}

load_elf(name){
  int f;
  int l;
  int c;
  int i;
  int j;
  int v;
  int k;
  int e;
  int obj_struct;
  obj_struct=calloc(obj_struct_size,1);
  wi32(obj_struct + (4 * obj_name_o), name);
  wi32(obj_struct + (4 * obj_linked_o), 0);
  l=0;
  puts("loading elf file:");
  puts(name);
  f=fopen(name,"rb");
  while((c=fgetc(f)) >= 0){
    wi8(elf_buf + l, c);
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

int not_impl(void){
  puts("not impl");
  exit(1);
}

int mk_host_obj(void){
  int *obj;
  int *e;
  int n;
  n=0;
  e=calloc(exp_size*16,1);
  obj=calloc(obj_struct_size,1);
  obj[obj_name_o]="host.o";
  obj[obj_linked_o]=1;
  obj[obj_exports_o]=e;
  /*
    need to use accessors to get addresses of stdout etc since cc_x86 does not
    support &stdout etc
  */
  e[n+exp_name_o]="stdout";
  e[n+exp_address_o]=get_stdout();
  n=n+(exp_size>>2);
  e[n+exp_name_o]="stdin";
  e[n+exp_address_o]=get_stdin();
  n=n+(exp_size>>2);
  e[n+exp_name_o]="stderr";
  e[n+exp_address_o]=get_stderr();
  n=n+(exp_size>>2);
  e[n+exp_name_o]="fputs";
  e[n+exp_address_o]=fputs_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="calloc";
  e[n+exp_address_o]=calloc_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="exit";
  e[n+exp_address_o]=exit_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="fputc";
  e[n+exp_address_o]=fputc_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="free";
  e[n+exp_address_o]=free_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="malloc";
  e[n+exp_address_o]=malloc_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="realloc";
  e[n+exp_address_o]=realloc_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="open";
  e[n+exp_address_o]=open_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="close";
  e[n+exp_address_o]=close_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="read";
  e[n+exp_address_o]=read_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="fopen";
  e[n+exp_address_o]=fopen_tramp;
  n=n+(exp_size>>2);
  e[n+exp_name_o]="fclose";
  e[n+exp_address_o]=fclose_tramp;

  return obj;
}

int get_main(int o){
  int ms;
  ms=find_sym(o,"main");
  fputs("main address: 0x",stdout);
  fputs(int2str(ms,16,0),stdout);
  fputs("\n",stdout);
  return ms;
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
  int *exports;
  int n_exports;
  int *unds;
  int n_unds;

  n_exports=0;
  n_unds=0;
  /* FIXME this should not be 1024 it should be calculated */
  exports=calloc(exp_size*1024,1);
  unds=calloc(und_size*1024,1);

  symtab=obj[obj_symtab_o];
  symtab_size=obj[obj_symtab_size_o];
  entsize=16;
  if(verbose){
    puts("gen_und_exports (export table and undefined symbol table)");
  }
  obj[obj_exports_o] = exports;
  obj[obj_und_o] = unds;
  for(i=0;i<obj[obj_symtab_size_o];i=i+entsize){
    sym=i+symtab;
    hex_dump(sym,entsize);
    st_name=ri32(sym+st_name_o);
    st_name_str=obj[obj_strtab_o]+st_name;
    st_value=ri32(sym+st_value_o);
    st_info=ri8(sym+st_info_o);
    st_type=st_info & 0xF;
    st_bind=st_info>>4;
    st_shndx=ri32(sym+st_shndx_o) & 0xFFFF;
    if(verbose){
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
    }

    if((st_type==STT_OBJECT) | (st_type==STT_FUNC)){
      if(verbose){puts("OBJECT or FUNCTION");}
      if(st_shndx==0){
        if(verbose){puts("UND");}
        unds[(n_unds*2)+und_name_o]=st_name_str;
        unds[(n_unds*2)+und_val_o]=sym+st_value_o;
/* dummy test */
/*        wi32(unds[(n_unds*2)+und_val_o],0x12345678); */
        n_unds=n_unds+1;
        if(n_unds>1024) {
          puts("too many unds");
          exit(1);
        }
      } else {
        /* patch physical address into symtab */
        st_value=obj[st_shndx]+st_value;
        wi32(sym+st_value_o,st_value);
        if(st_bind==ST_GLOBAL) {
          if(verbose){puts("export");}
          exports[(n_exports*2)+exp_name_o]=st_name_str;
          exports[(n_exports*2)+exp_address_o]=st_value;
          n_exports=n_exports+1;
          if(n_exports>1024) {
            puts("too many exports");
            exit(1);
          }
        }
      }
    }
  }
  if(verbose){
    puts("exports:");
    hex_dump(exports,n_exports*8);
    puts("unds:");
    hex_dump(unds,n_unds*8);
  }
}

int find_sym(int os, char *name){
  int *objs;
  int *obj;
  int *exports;
  int m;
  int n;
  int e;
  int r;
  n=0;
  objs=os;
  if(verbose){
    fputs("find_sym: ",stdout);
    fputs(name,stdout);
    fputs("\n",stdout);
  }
  while((obj=objs[n])!=0){
    if(verbose){
      fputs("looking in: ",stdout);
      fputs(obj[obj_name_o],stdout);
      fputs("\n",stdout);
    }
    exports=obj[obj_exports_o];
    if(exports!=0){
      if(verbose){puts("we have some exports");}
      m=0;
      while((e=exports[(2*m)+exp_name_o])!=0){
        if(verbose){puts(e);}
        if(strcmp(e,name)==0){
          if(verbose){puts("found");}
          return(exports[(2*m)+exp_address_o]);
        }
        m=m+1;
      }
    }
    n=n+1;
  }
  r=dlsym(0,name);
  if(r != 0) {
    printf("found %s via dlsym\n", name);
    return r;
  } else {
    fputs("couldn't find: ",stdout);
    fputs(name,stdout);
    fputs("\n",stdout);
    return 0;
  }
}

int resolve_und(int os){
  int *objs;
  int *obj;
  int *unds;
  int m;
  int n;
  int u;
  int addr;
  n=0;
  objs=os;
  if(verbose){puts("resolve_und");}
  while((obj=objs[n])!=0){
    if(verbose){
      fputs("resolving_und in: ",stdout);
      fputs(obj[obj_name_o],stdout);
      fputs("\n",stdout);
    }
    unds=obj[obj_und_o];
    if(unds!=0){
      if(verbose){puts("we have some unds:");}
      m=0;
      while((u=unds[(2*m)+und_name_o])!=0){
        if(verbose){puts(u);}
        addr=find_sym(os,u);
        if(addr==0){
          puts("sym not found");
          exit(1);
        } else {
          if(verbose){puts("writing address of und sym");}
          wi32(unds[(2*m)+und_val_o],addr);
        }
        m=m+1;
      }
    } else {
      if(verbose){puts("no unds in this obj");}
    }
    n=n+1;
  }
}

int dump_exports(int o){
  int *obj;
  obj=o;
  int *exports;
  int name;
  int n;
  puts("==========");
  puts("dump_exports:");
  fputs("obj: ",stdout);
  fputs(obj[obj_name_o],stdout);
  fputs("\n",stdout);
  exports=obj[obj_exports_o];
  n=0;
  while((name=exports[(2*n)+exp_name_o])!=0){
    fputs("name: ",stdout);
    fputs(name,stdout);
    fputs(" address: 0x",stdout);
    fputs(int2str(exports[(2*n)+exp_address_o],16,0),stdout);
    fputs("\n",stdout);
    n=n+1;
  }
}

int dump_unds(int o) {
  int *obj;
  obj=o;
  int *unds;
  int name;
  int n;
  puts("==========");
  puts("dump_unds");
  fputs("obj: ",stdout);
  fputs(obj[obj_name_o],stdout);
  fputs("\n",stdout);
  unds=obj[obj_und_o];
  n=0;
  while((name=unds[(2*n)+und_name_o])!=0){
    fputs("name: ",stdout);
    fputs(name,stdout);
    fputs(" address: 0x",stdout);
    fputs(int2str(ri32(unds[(2*n)+und_val_o]),16,0),stdout);
    fputs("\n",stdout);
    n=n+1;
  }
}

int relocate_section(int o, int name, int rels, int size, int p){
  int *obj;
  int entsize;
  int sym_entsize;
  int i;
  int r_offset;
  int r_info;
  int r_type;
  int r_sym;
  int r;
  int symtab;
  int strtab;
  int val;
  int sym;
  int sym_name;
  int loc;

  obj=o;
  entsize=8;
  sym_entsize=16;
  symtab=obj[obj_symtab_o];
  strtab=obj[obj_strtab_o];
  if(verbose){
    fputs("relocating: ",stdout);
    fputs(name, stdout);
    fputs("  in: ", stdout);
    fputs(obj[obj_name_o], stdout);
    fputs("\n",stdout);
  }
  if(size==0){
    if(verbose){puts("no relocations");}
  } else {
    if(verbose){puts("processing relocations");}
    for(i=0;i<size;i=i+entsize){
      r=rels+i;
      if(verbose){puts("reloc");}
      hex_dump(r,8);
      r_offset=ri32(r+r_offset_o);
      r_info=ri32(r+r_info_o);
      r_type=r_info & 0xFF;
      /* FIXME is this right for unsiged right shift? */
      r_sym=(r_info >>8 ) & 0xFFFFFF;
      sym=symtab+(sym_entsize*r_sym);
      val=ri32(sym+st_value_o);
      sym_name=strtab+ri32(sym+st_name_o);
      if(verbose){
        fputs("offset: ",stdout);
        fputs(int2str(r_offset,16,0),stdout);
        fputs("\n",stdout);
        fputs("info: ",stdout);
        fputs(int2str(r_info,16,0),stdout);
        fputs("\n",stdout);
        fputs("sym num: ",stdout);
        fputs(int2str(r_sym,16,0),stdout);
        fputs("\n",stdout);
        fputs("val: 0x",stdout);
        fputs(int2str(val,16,0),stdout);
        fputs("\n",stdout);
        fputs("name: ",stdout);
          fputs(sym_name,stdout);
        fputs("\n",stdout);
        fputs("type: ",stdout);
        fputs(int2str(r_type,16,0), stdout);
        fputs(" ",stdout);
      }
      loc=p+r_offset;
      if(r_type==R_386_32){
        if(verbose){fputs("R_386_32", stdout);}
        wi32(loc,ri32(loc)+val);
      } else if (r_type==R_386_PC32){
        if(verbose){fputs("R_386_PC32", stdout);}
        wi32(loc,val-loc-4);
      } else {
        fputs("unsupported relocation type", stdout);
        exit(1);
      }
      if(verbose){fputs("\n",stdout);}
    }
  }
}

int relocate(int o) {
  int *obj;
  obj=o;
  if(verbose){
    fputs("relocate: ",stdout);
    fputs(obj[obj_name_o],stdout);
    fputs("\n",stdout);
  }
  relocate_section(obj, ".rel.text", obj[obj_rel_text_o],
                                     obj[obj_rel_text_size_o],
                                     obj[obj_text_o]);
  relocate_section(obj, ".rel.data", obj[obj_rel_data_o],
                                     obj[obj_rel_data_size_o],
                                     obj[obj_data_o]);
  obj[obj_linked_o]=1;
}

int link(int o){
  int *objs;
  int *obj;
  int name;
  int i;
  objs=o;
  i=0;
  if(verbose){
    puts("============================");
    puts("linking");
  }
  while(obj=objs[i]){
    name=obj[obj_name_o];
    if(verbose){puts(name);}
    if(obj[obj_linked_o]!=0){
      if(verbose){fputs("already linked\n",stdout);}
    } else {
      if(verbose){fputs("linking\n",stdout);}
      gen_und_exports(obj);
      if(verbose){
        dump_symtab(obj);
        dump_exports(obj);
        dump_unds(obj);
      }
    }
    if(verbose){puts("");}
    i=i+1;
  }

  resolve_und(o);
  i=0;

  if(verbose){
    puts("============================");
    puts("relocation");
  }
  while(obj=objs[i]){
    if(obj[obj_linked_o]!=0){
      if(verbose){
        fputs("already relocated: ",stdout);
        fputs(obj[obj_name_o],stdout);
        fputs("\n",stdout);
      }
    } else {
      relocate(obj);
    }
    i=i+1;
  }
  if(verbose){puts("============================");}
}

int main(int argc, char **argv)
{
  FUNCTION t;
  int optind;
  int *objs;
  int *objs_files;
  int i;
  int j;
  int cur;

  init_c();
  init_globals();
  init_offsets();

  verbose=0;

  /* enough for 8 objs */
  objs=calloc(36,1);
  /* filenames of input objects */
  objs_files=calloc(36,1);

  puts("elf loader starting");

  optind = 1;

  if(strcmp(argv[optind],"-v") == 0){
    verbose=1;
    optind = optind + 1;
    puts("verbose mode");
  }

  puts("running elf files");
  j=0;
  for(i=optind;i<argc;i=i+1){
    cur=argv[i];
    if(strcmp(cur, "-l") == 0){
      fputs("load elf file: ",stdout);
      i=i+1;
      objs_files[j]=argv[i];
      j=j+1;
      fputs(argv[i],stdout);
      fputs("\n",stdout);
      optind=i;
    } else {
      break;
    }
  }
  objs[0]=mk_host_obj();
  if(objs_files[0] == 0){
    objs[1]=load_elf("libc_boot.o");
    objs[2]=load_elf("tcc.o");
  } else {
    i=0;
    while((cur=objs_files[i]) !=0){
      fputs("loading: ",stdout);
      fputs(cur,stdout);
      fputs("\n",stdout);
      objs[i+1]=load_elf(cur);
      i=i+1;
    }
  }
  link(objs);
  fputs("optind: ",stdout);
  fputs(int2str(optind,10,0),stdout);
  fputs("\n",stdout);
  t=get_main(objs);
  puts("============================");
  puts("calling main from elf file:");
  return call_wrap(t, argc - optind, argv + (p_size*optind));
}
