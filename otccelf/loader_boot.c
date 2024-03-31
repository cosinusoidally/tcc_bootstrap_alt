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

init_globals(){
  DATA_SIZE = (256*1024);
  TEXT_SIZE = (256*1024);
  RELOC_ADDR32 = 1; /* 32 bits relocation */
  RELOC_REL32 = 2; /* 32 bits relative relocation */
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

dlsym_wrap( h, sym){
  puts("dlsym lookup:");
  puts(sym);
  return dlsym(h,sym);
}

load_obj(){
  int f, text_len, data_len, reloc_len, global_reloc_len;
  int global_reloc_table_len, entrypoint, m0, m1, m2, m3, m4, i, t;
  int m; int l; int a; int n; int p;
  int goff; int off; int addr; int reloc_type; int ptr;
  puts("Loading object file");
  text_len=malloc(4);
  data_len=malloc(4);
  reloc_len=malloc(4);
  global_reloc_len=malloc(4);
  global_reloc_table_len=malloc(4);
  entrypoint=malloc(4);
  m0=3735928320;
  m1=3735928321;
  m2=3735928322;
  m3=3735928323;
  m4=3735928324;
  t=malloc(4);
  f = fopen("tcc_boot.o", "rb");
  fread(entrypoint,1,4,f);
  fread(text_len,1,4,f);
  fread(data_len,1,4,f);
  fread(reloc_len,1,4,f);
  fread(global_reloc_len,1,4,f);
  fread(global_reloc_table_len,1,4,f);
  fread(t,1,4,f);
  if(ri32(t) != m0){
    puts("sync m0");
    exit(1);
  }
  global_relocs_table_base=malloc(ri32(global_reloc_table_len));
  global_relocs_table=global_relocs_table_base;
  fread(global_relocs_table_base,1,ri32(global_reloc_table_len),f);
  prog_rel=malloc(ri32(text_len));
  data_rel=malloc(ri32(data_len));

  fread(t,1,4,f);
  if(ri32(t) != m1){
    puts("sync m1");
    exit(1);
  }
  relocs_base=malloc(ri32(reloc_len));
  fread(relocs_base,1,ri32(reloc_len),f);
  fread(t,1,4,f);
  if(ri32(t) != m2){
    puts("sync m2");
    exit(1);
  }

  fread(data_rel,1,ri32(data_len),f);

  fread(t,1,4,f);
  if(ri32(t) != m3){
    puts("sync m3");
    exit(1);
  }

  global_relocs_base=malloc(ri32(global_reloc_len));
  fread(global_relocs_base,1,ri32(global_reloc_len),f);

  fread(t,1,4,f);
  if(ri32(t) != m4){
    puts("sync m4");
    exit(1);
  }

  fread(prog_rel,1,ri32(text_len),f);
  glo = malloc(DATA_SIZE);
  glo_base=glo;
  memset(glo, 0, DATA_SIZE);
  prog = malloc(TEXT_SIZE);
  ind = prog;
  memcpy(prog, prog_rel, ri32(text_len));
  memcpy(glo_base, data_rel, ri32(data_len));
  fclose(f);

  m=global_relocs_table_base+ri32(global_reloc_table_len);
  for(i=0;i<ri32(reloc_len);i=i+12){
    if(relocs_base+i+8==0){
      p=prog;
    } else {
      p=glo;
    }
    wi32(prog+ri32(relocs_base+i),ri32(relocs_base+i+4)+p);
  }
  goff=0;
  while(global_relocs_table<m){
    l=strlen(global_relocs_table);
    a=dlsym_wrap(0,global_relocs_table);
    global_relocs_table = global_relocs_table + l + 1;
    n=ri32(global_relocs_table);
    global_relocs_table = global_relocs_table+4;
    for(i=0;i<n;i=i+1){
      off=ri32(global_relocs_base+goff+4);
      addr = (off+prog);
      ptr = addr;
      reloc_type=ri32(global_relocs_base+goff);
      if(reloc_type == RELOC_ADDR32) {
        wi32(ptr, a);
      }
      if(reloc_type == RELOC_REL32) {
        wi32(ptr, a - addr - 4);
      }
      goff=goff+8;
    }
  }
  return prog+(ri32(entrypoint));
}

main(argc, argv)
{
    int t;
    int r;
    int optind;
    int loader;
    int p_size;
    p_size=4;

    init_globals();

    puts("loader starting");

    optind = 1;
    loader = 0;

    while (1) {
        r = ri32(argv + (4*optind));
        if (ri8(r) != '-')
            break;
        optind = optind + 1;
        if(ri8(r+1) == 'R') {
            t=load_obj();
            loader=1;
        } else {
            puts("option error");
            exit(1);
        }
    }

  puts("running loader");
/*  return call_wrap(t, argc - optind, argv + (p_size*optind)); */
  return ((int (*)())(t))(argc - optind, argv + (p_size*optind));
}
