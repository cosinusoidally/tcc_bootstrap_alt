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
int st_value_o;
int st_info_o;
int st_shndx_o;
int STT_OBJECT;
int STT_FUNC;
int ST_GLOBAL;
int r_offset_o;
int R_386_32;
int R_386_PC32;
int p_size;
int verbose;
int libm;

puts_num(x){
  printf("%u\n",x);
}

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

#if 0
int2str(a, b, c){
  puts("int2str unimpl");
  return "blah";
}

/* HACK otccelf will ignore the if an define call_wrap */

stub2(a,b){
  return a+b;
}

stub() {
  return &stub2;
}

call_wrap(t, a, b){
  int f;
  printf("call_wrap called %x %d\n",t, a);
  f=stub();
  wi8(f,0xB8);
  wi32(f+1,t);
  wi8(f+5,0xFF);
  wi8(f+6,0xE0);
  printf("stub2 %x\n",f);
/*  while(my_break){} */
  return stub2(a, b);
/*  return(*(int(*)())*(int*)(t))(a, b) */
/*  exit(0); */
}

#endif

hex_dump(e, l){
}

dump_symtab(o){
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
  o=e+e_shoff+(e_shstrndx*e_shentsize);
  sh_offset=ri32(o+16);
  o=e_shoff;
  for(i=0;i<e_shnum;i=i+1){
    sh_name=ri32(e+o);
    sh_name_str=e+sh_offset+sh_name;
    if(strcmp(str,sh_name_str) == 0){
      return e+o;
    }
    o=o+e_shentsize;
  }
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
  o=e_shoff;
  for(i=0;i<e_shnum;i=i+1){
    sh_type=ri32(e+o+4);
    sh_offset=ri32(e+o+16);
    sh_size=ri32(e+o+20);
    if(i==e_shstrndx){
      o=e+sh_offset;
      for(j=0;j<e_shnum;j=j+1){
        sl=strlen(o);
        o=o+sl+1;
      }
    }
    o=o+e_shentsize;
  }

  sh_offset=ri32(text+sh_offset_o);
  sh_size=ri32(text+sh_size_o);
  text_mem=malloc(sh_size);
  memcpy(text_mem,e+sh_offset,sh_size);
  wi32(obj_struct+(4*obj_text_o), text_mem);
  wi32(obj_struct+ (4*obj_text_size_o), sh_size);

  sh_offset=ri32(data+sh_offset_o);
  sh_size=ri32(data+sh_size_o);
  data_mem=malloc(sh_size);
  memcpy(data_mem,e+sh_offset,sh_size);
  wi32(obj_struct + (4 * obj_data_o), data_mem);
  wi32(obj_struct + (4 * obj_data_size_o), sh_size);

  sh_offset=ri32(strtab+sh_offset_o);
  sh_size=ri32(strtab+sh_size_o);
  strtab_mem=malloc(sh_size);
  memcpy(strtab_mem,e+sh_offset,sh_size);
  wi32(obj_struct + (4 * obj_strtab_o), strtab_mem);
  wi32(obj_struct + (4 * obj_strtab_size_o), sh_size);

  sh_offset=ri32(symtab+sh_offset_o);
  sh_size=ri32(symtab+sh_size_o);
  sh_entsize=ri32(symtab+sh_entsize_o);
  symtab_mem=malloc(sh_size);
  memcpy(symtab_mem,e+sh_offset,sh_size);
  wi32(obj_struct + (4 * obj_symtab_o), symtab_mem);
  wi32(obj_struct + (4 * obj_symtab_size_o), sh_size);

  if(rel_text!=0){
    sh_offset=ri32(rel_text+sh_offset_o);
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

not_impl(){
  puts("not impl");
  exit(1);
}

mk_host_obj(){
  int obj;
  int e;
  int n;
  n=0;
  e=calloc(exp_size*16,1);
  obj=calloc(obj_struct_size,1);
  wi32(obj + (4 *obj_name_o), "host.o");
  wi32(obj + (4 *obj_linked_o), 1);
  wi32(obj + (4 * obj_exports_o), e);
  /*
    need to use accessors to get addresses of stdout etc since cc_x86 does not
    support &stdout etc
  */
/* HACK disable all these since not needed with otccelf version */
/*
  e[n+exp_name_o]="fopen";
  e[n+exp_address_o]=fopen_tramp;
  n=n+(exp_size>>2);
*/
  return obj;
}

get_main(o){
  int ms;
  ms=find_sym(o,"main");
  fputs("main address: 0x",stdout);
  fputs(int2str(ms,16,0),stdout);
  fputs("\n",stdout);
  return ms;
}

gen_und_exports(o){
  int obj;
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
  int exports;
  int n_exports;
  int unds;
  int n_unds;

  obj = o;

  n_exports=0;
  n_unds=0;
  /* FIXME this should not be 1024 it should be calculated */
  exports=calloc(exp_size*1024,1);
  unds=calloc(und_size*1024,1);

  symtab = ri32(obj + (4 * obj_symtab_o));
  symtab_size = ri32(obj + (4 * obj_symtab_size_o));
  entsize=16;
  if(verbose){
    puts("gen_und_exports (export table and undefined symbol table)");
  }
  wi32(obj + (4 * obj_exports_o), exports);
  wi32(obj + (4 * obj_und_o), unds);
  for(i=0;i< ri32(obj + (4 * obj_symtab_size_o));i=i+entsize){
    sym=i+symtab;
    hex_dump(sym,entsize);
    st_name=ri32(sym+st_name_o);
    st_name_str = ri32(obj + (4 * obj_strtab_o)) + st_name;
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
        wi32(unds + (4 * ((n_unds*2)+und_name_o)), st_name_str);
        wi32(unds + (4 * ((n_unds*2)+und_val_o)), sym+st_value_o);
/* dummy test */
/*        wi32(unds[(n_unds*2)+und_val_o],0x12345678); */
        n_unds=n_unds+1;
        if(n_unds>1024) {
          puts("too many unds");
          exit(1);
        }
      } else {
        /* patch physical address into symtab */
        st_value = ri32(obj + (4 * st_shndx)) + st_value;
        wi32(sym+st_value_o,st_value);
        if(st_bind==ST_GLOBAL) {
          if(verbose){puts("export");}
          wi32(exports + (4 * ((n_exports*2)+exp_name_o)), st_name_str);
          wi32(exports + (4 * ((n_exports*2)+exp_address_o)), st_value);
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
    hex_dump(exports, n_exports*8);
    puts("unds:");
    hex_dump(unds, n_unds*8);
  }
}

find_sym(os, name){
  int objs;
  int obj;
  int exports;
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
  while((obj = ri32(objs + (4 * n)))!=0){
    if(verbose){
      fputs("looking in: ",stdout);
      fputs(ri32(obj + (4 * obj_name_o)),stdout);
      fputs("\n",stdout);
    }
    exports = ri32(obj + (4 * obj_exports_o));
    if(exports != 0){
      if(verbose){puts("we have some exports");}
      m=0;
      while((e = ri32(exports + (4 * ((2*m)+exp_name_o)))) != 0){
        if(verbose){puts(e);}
        if(strcmp(e,name)==0){
          if(verbose){puts("found");}
          return(ri32(exports + (4 * ((2*m)+exp_address_o))));
        }
        m=m+1;
      }
    }
    n=n+1;
  }
  r=dlsym(0,name);
  if(r == 0){
    /* try finding the symbol in libm */
    r = dlsym(libm, name);
  }
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

resolve_und(os){
  int objs;
  int obj;
  int unds;
  int m;
  int n;
  int u;
  int addr;

  n=0;
  objs=os;

  if(verbose){puts("resolve_und");}
  while((obj = ri32(objs + (4 * n))) != 0){
    if(verbose){
      fputs("resolving_und in: ",stdout);
      fputs(ri32(obj + (4 * obj_name_o)),stdout);
      fputs("\n",stdout);
    }
    unds = ri32(obj + (4 *obj_und_o));
    if(unds != 0){
      if(verbose){puts("we have some unds:");}
      m=0;
      while((u = ri32(unds + (4 * ((2*m)+und_name_o)))) != 0){
        if(verbose){puts(u);}
        addr=find_sym(os,u);
        if(addr==0){
          puts("sym not found");
          exit(1);
        } else {
          if(verbose){puts("writing address of und sym");}
          wi32(ri32(unds + (4 * ((2*m)+und_val_o))), addr);
        }
        m=m+1;
      }
    } else {
      if(verbose){puts("no unds in this obj");}
    }
    n=n+1;
  }
}

dump_exports(o){
/*
  int obj;
  int exports;
  int name;
  int n;
  obj = o;
  puts("==========");
  puts("dump_exports:");
  fputs("obj: ",stdout);
  fputs(ri32(obj + (4 * obj_name_o)),stdout);
  fputs("\n",stdout);
  exports = ri32(obj + (4 * obj_exports_o));
  n=0;
  while((name = ri32(exports + (4 * ((2*n)+exp_name_o)))) != 0){
    fputs("name: ",stdout);
    fputs(name,stdout);
    fputs(" address: 0x",stdout);
    fputs(int2str(ri32(exports + (4 * ((2*n)+exp_address_o))),16,0),stdout);
    fputs("\n",stdout);
    n=n+1;
  }
*/
}

dump_unds(o) {
  int obj;
  int unds;
  int name;
  int n;
  obj=o;
  puts("==========");
  puts("dump_unds");
  fputs("obj: ",stdout);
  fputs(ri32(obj + (4 * obj_name_o)),stdout);
  fputs("\n",stdout);
  unds = ri32(obj + (4 * obj_und_o));
  n=0;
  while((name = ri32(unds +  (4 * ((2*n)+und_name_o)))) != 0){
    fputs("name: ",stdout);
    fputs(name,stdout);
    fputs(" address: 0x",stdout);
    fputs(int2str(ri32(ri32(unds + (4 * ((2*n)+und_val_o)))),16,0),stdout);
    fputs("\n",stdout);
    n=n+1;
  }
}

relocate_section(o, name, rels, size, p){
  int obj;
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

  obj = o;
  entsize=8;
  sym_entsize=16;
  symtab = ri32(obj + (4 * obj_symtab_o));
  strtab = ri32(obj + (4 * obj_strtab_o));
  if(verbose){
    fputs("relocating: ",stdout);
    fputs(name, stdout);
    fputs("  in: ", stdout);
    fputs(ri32(obj + (4 * obj_name_o)), stdout);
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

relocate(o) {
  int obj;
  obj = o;
  if(verbose){
    fputs("relocate: ",stdout);
    fputs(ri32(obj + (4 * obj_name_o)),stdout);
    fputs("\n",stdout);
  }
  relocate_section(obj, ".rel.text", ri32(obj + (4 * obj_rel_text_o)),
                                     ri32(obj + (4 * obj_rel_text_size_o)),
                                     ri32(obj + (4 * obj_text_o)));
  relocate_section(obj, ".rel.data", ri32(obj + (4 * obj_rel_data_o)),
                                     ri32(obj + (4 * obj_rel_data_size_o)),
                                     ri32(obj + (4 * obj_data_o)));
  wi32(obj + (4 * obj_linked_o), 1);
}

link(o){
  int objs;
  int obj;
  int name;
  int i;

  objs = o;
  i=0;
  if(verbose){
    puts("============================");
    puts("linking");
  }
  while(obj = ri32(objs + (4 * i))){
    name = ri32(obj + (4 * obj_name_o));
    if(verbose){puts(name);}
    if(ri32(obj + (4 * obj_linked_o)) != 0){
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
  while(obj = ri32(objs + (4 * i))){
    if(ri32(obj + (4 * obj_linked_o)) != 0){
      if(verbose){
        fputs("already relocated: ",stdout);
        fputs(ri32(obj + (4 * obj_name_o)),stdout);
        fputs("\n",stdout);
      }
    } else {
      relocate(obj);
    }
    i=i+1;
  }
  if(verbose){puts("============================");}
}

main(argc, argv)
{
  int t;
  int optind;
  int objs;
  int objs_files;
  int i;
  int j;
  int cur;

  init_globals();
  init_offsets();

  verbose=0;

  libm = dlopen("libm.so.6", 2); /* RTLD_NOW */

  /* enough for 8 objs */
  objs = calloc(36,1);
  /* filenames of input objects */
  objs_files = calloc(36,1);

  puts("elf loader starting");

  optind = 1;

  if(strcmp(ri32(argv + (4 * optind)),"-v") == 0){
    verbose=1;
    optind = optind + 1;
    puts("verbose mode");
  }

  puts("running elf files");
  j=0;
  for(i=optind;i<argc;i=i+1){
    cur= ri32(argv + (4 *i));
    if(strcmp(cur, "-l") == 0){
      fputs("load elf file: ",stdout);
      i=i+1;
      wi32(objs_files + (4 * j), ri32(argv + (4 * i)));
      j=j+1;
      fputs(ri32(argv + (4 * i)),stdout);
      fputs("\n",stdout);
      optind=i;
    } else {
      break;
    }
  }
  wi32(objs, mk_host_obj());
  if(ri32(objs_files) == 0){
    wi32(objs + (4 * 1), load_elf("libc_boot.o"));
    wi32(objs + (4 * 2), load_elf("tcc.o"));
  } else {
    i=0;
    while((cur = ri32(objs_files + (4 * i))) !=0){
      fputs("loading: ",stdout);
      fputs(cur,stdout);
      fputs("\n",stdout);
      wi32(objs + (4 * (i+1)), load_elf(cur));
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
  return call_wrap(t, argc - optind, argv + (4*optind));
}
