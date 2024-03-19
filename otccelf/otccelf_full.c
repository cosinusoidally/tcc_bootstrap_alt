int tok, tokc, tokl, ch, vars, rsym, prog, ind, loc, glo, file, sym_stk, dstk,dptr,dch,last_id, data, text, data_offset;

int ALLOC_SIZE;

int ELF_BASE;
int PHDR_OFFSET, INTERP_OFFSET, INTERP_SIZE, DYNAMIC_OFFSET, DYNAMIC_SIZE;
int ELFSTART_SIZE, STARTUP_SIZE, DYNSTR_BASE;

int TOK_STR_SIZE;
int TOK_IDENT, TOK_INT, TOK_IF, TOK_ELSE, TOK_WHILE, TOK_BREAK, TOK_RETURN;
int TOK_FOR, TOK_DEFINE, TOK_MAIN, TOK_DUMMY, TOK_NUM, LOCAL, SYM_FORWARD;
int SYM_DEFINE, TAG_TOK, TAG_MACRO;

pdef(t){
  *(char*) dstk++ = t;
}

inp (){
  if(dptr){
    ch=*(char*) dptr++;
    if( ch == TAG_MACRO){
      dptr=0;
      ch=dch;
    }
  }
    else ch=fgetc(file);
}

isid (){
  return isalnum(ch) | ch == '_';
}

getq (){
  if( ch == '\\'){
    inp ();
    if( ch == 'n') {
      ch = '\n';
    }
  }
}

next(){
  int t, s,h;
  while( isspace(ch) | ch == '#'){
    if( ch == '#'){
      inp ();
      next();
      if( tok == TOK_DEFINE){
        next();
        pdef(TAG_TOK);
        *(int*) tok=1;
        *(int*)(tok+4)=dstk;
      }
      while( ch != '\n'){
        pdef(ch);
        inp ();
      }
      pdef(ch);
      pdef(TAG_MACRO);
    }
    inp ();
  }
  tokl=0;
  tok=ch;
  if( isid ()){
    pdef(TAG_TOK);
    last_id=dstk;
    while( isid ()){
      pdef(ch);
      inp ();
    }
    if( isdigit(tok)){
      tokc = strtol(last_id,0,0);
      tok = TOK_NUM;
    }
    else{
      *(char*) dstk = TAG_TOK;
      tok = strstr(sym_stk, last_id - 1) - sym_stk;
      *(char*) dstk=0;
      tok=tok * 8 + TOK_IDENT;
      if( tok>TOK_DEFINE){
        tok=vars+tok;
        if( *(int*) tok == SYM_DEFINE){
          dptr=*(int*)(tok+4);
          dch=ch;
          inp ();
          next();
        }
      }
    }
  }
  else{
    inp ();
    if( tok == '\''){
      tok = TOK_NUM;
      getq ();
      tokc=ch;
      inp ();
      inp ();
    }
    else if( tok == '/' & ch == '*'){
      inp ();
      while( ch){
        while( ch != '*') {
          inp ();
        }
        inp ();
        if( ch == '/') {
          ch=0;
        }
      }
      inp ();
      next();
    }
    else{
      t="++#m--%am*@R<^1c/@%[_[H3c%@%[_[H3c+@.B#d-@%:_^BKd<<Z/03e>>`/03e<=0f>=/f<@.f>@1f==&g!='g&&k||#l&@.BCh^@.BSi|@.B+j~@/%Yd!@&d*@b";
      while( s=*(char*) t++){
        h=*(char*) t++;
        tokc=0;
        while((tokl = *(char*) t++ - 'b')<0) {
          tokc = tokc * 64 + tokl + 64;
        }
        if( s == tok & (h == ch | h == '@')){
          if( h == ch){
            inp ();
            tok = TOK_DUMMY;
          }
          break;
        }
      }
    }
  }
}

o( d){
  while( d&&d!=-1){
    *(char*) ind++=d;
    d=d>>8;
  }
}

put32(a,d){
  *(char*) a++=d;
  *(char*) a++=d>>8;
  *(char*) a++=d>>16;
  *(char*) a++=d>>24;
}

get32(a){
  int d;
  return(*(char*) a&255)|(*(char*)(a+1)&255)<<8|(*(char*)(a+2)&255)<<16|(*(char*)(a+3)&255)<<24;
}

gsym1(a,z){
  int d;
  while( a){
    d=get32(a);
    if( *(char*)(a-1) == 5){
      if( z>=data&&z<glo)
        put32(a,z+data_offset);
      else
        put32(a,z-prog+text+data_offset);
    }
    else{
      put32(a,z-a-4);
    }
    a=d;
  }
}

gsym(a){
  gsym1(a,ind);
}

oad(d,a){
  o( d);
  put32(ind,a);
  a=ind;
  ind=ind+4;
  return a;
}

li(a){
  oad(184,a);
}

gjmp(a){
  return oad(233,a);
}

gtst(s,a){
  o( 1032325);
  return oad(132+s,a);
}

gcmp(a){
  o( 49465);
  li(0);
  o( 15);
  o( a+144);
  o( 192);
}

gmov(s,a){
  int d;
  o( s+131);
  d=*(int*) a;
  if( d && d < LOCAL) {
    oad(133,d);
  } else {
    a=a+4;
    *(int*) a=oad(5,*(int*) a);
  }
}

unary(s){
  int d,a,h,F;
  d=1;
  if( tok == '\"'){
    li(glo+data_offset);
    while( ch!='\"'){
      getq ();
      *(char*) glo++=ch;
      inp ();
    }
    *(char*) glo=0;
    glo=glo+4&-4;
    inp ();
    next();
  } else{
    F=tokl;
    h=tokc;
    a=tok;
    next();
    if( a == TOK_NUM){
      li(h);
    }
    else if( F == 2){
      unary(0);
      oad(185,0);
      if( a == '!') {
        gcmp(h);
      } else {
        o( h);
      }
    }
    else if( a == '('){
      expr ();
      next();
    }
    else if( a == '*'){
      next();
      a=tok;
      next();
      next();
      if( tok == '*'){
        next();
        next();
        next();
        next();
        a=0;
      }
      next();
      unary(0);
      if( tok == '='){
        next();
        o( 80);
        expr ();
        o( 89);
        o( 392+(a == TOK_INT));
      } else if( a){
        if( a == TOK_INT) {
          o( 139);
        } else {
          o( 48655);
        }
        ind++;
      }
    } else if( a == '&'){
      gmov(10,tok);
      next();
    }
    else{
      d=0;
      if( tok == '=' & s){
        next();
        expr ();
        gmov(6,a);
      } else if( tok != '('){
        gmov(8,a);
        if( tokl == 11){
          gmov(0,a);
          o( tokc);
          next();
        }
      }
    }
  }
  if( tok == '('){
    if( d)o( 80);
    h=oad(60545,0);
    next();
    s=0;
    while( tok != ')'){
      expr ();
      oad(2393225,s);
      if( tok == ',') {
        next();
      }
      s=s+4;
    }
    put32(h,s);
    next();
    if( d){
      oad(2397439,s);
      s=s+4;
    }
    else{
      a=a+4;
      *(int*) a=oad(232,*(int*) a);
    }
    if( s) oad(50305,s);
  }
}

sum(s){
  int a,d,h;
  if( s--== 1) unary(1);
  else{
    sum(s);
    h=0;
    while( s == tokl){
      d=tok;
      a=tokc;
      next();
      if( s>8){
        h=gtst(a,h);
        sum(s);
      }
      else{
        o( 80);
        sum(s);
        o( 89);
        if( s == 4|s == 5){
          gcmp(a);
        }
        else{
          o( a);
          if( d == '%') {
            o( 146);
          }
        }
      }
    }
    if( h&&s>8){
      h=gtst(a,h);
      li(a^1);
      gjmp(5);
      gsym(h);
      li(a);
    }
  }
}

expr (){
  sum(11);
}

test_expr (){
  expr ();
  return gtst(0,0);
}

block(s){
  int h,d,a;
  if( tok == TOK_IF){
    next();
    next();
    h=test_expr ();
    next();
    block(s);
    if( tok == TOK_ELSE){
      next();
      d=gjmp(0);
      gsym(h);
      block(s);
      gsym(d);
    }
    else{
      gsym(h);
    }
  }
  else if( tok == TOK_WHILE | tok == TOK_FOR){
    a=tok;
    next();
    next();
    if( a == TOK_WHILE){
      d=ind;
      h=test_expr ();
    }
    else{
      if( tok != ';') {
        expr ();
      }
      next();
      d=ind;
      h=0;
      if( tok != ';') {
        h=test_expr ();
      }
      next();
      if( tok != ')'){
        a=gjmp(0);
        expr ();
        gjmp(d-ind-5);
        gsym(a);
        d=a+4;
      }
    }
    next();
    block(&h);
    gjmp(d-ind-5);
    gsym(h);
  }
  else if( tok == '{'){
    next();
    decl(1);
    while( tok != '}') {
      block(s);
    }
    next();
  }
  else{
    if( tok == TOK_RETURN){
      next();
      if( tok != ';') {
        expr ();
      }
      rsym=gjmp(rsym);
    } else if( tok == TOK_BREAK){
      next();
      *(int*) s=gjmp(*(int*) s);
    } else if( tok != ';') {
      expr ();
    }
    next();
  }
}

decl(s){
  int h;
  while( tok == TOK_INT | tok != -1 & !s){
    if( tok == TOK_INT){
      next();
      while( tok != ';'){
        if( s){
          loc=loc+4;
          *(int*) tok=-loc;
        }
        else{
          *(int*) tok=glo;
          glo=glo+4;
        }
        next();
        if( tok == ',') {
          next();
        }
      }
      next();
    }
    else{
      *(int*) tok=ind;
      next();
      next();
      h=8;
      while( tok != ')'){
        *(int*) tok=h;
        h=h+4;
        next();
        if( tok == ',') {
          next();
        }
      }
      next();
      rsym=loc=0;
      o( 15042901);
      h=oad(60545,0);
      block(0);
      gsym(rsym);
      o( 50121);
      put32(h,loc);
    }
  }
}

gle32( d){
  put32(glo,d);
  glo=glo+4;
}

gphdr1(d,a){
  gle32( d);
  d = d + ELF_BASE;
  gle32( d);
  gle32( d);
  gle32( a);
  gle32( a);
}

elf_reloc(s){
  int a,h,d,N,z,F;
  N=0;
  a=sym_stk;
  while( 1){
    a++;
    h=a;
    while( *(char*) a != TAG_TOK && a < dstk) {
      a++;
    }
    if( a == dstk)break;
    tok = vars + (h - sym_stk) * 8 + TOK_IDENT - 8;
    z=*(int*) tok;
    d=*(int*)(tok+4);
    if( d&&z!=1){
      if(!z){
        if(!s){
          memcpy(glo,h,a-h);
          glo=glo+a-h+1;
        }
        else if( s == 1){
          gle32( N + DYNSTR_BASE);
          gle32( 0);
          gle32( 0);
          gle32( 16);
          N=N+a-h+1;
        }
        else{
          N++;
          while( d){
            h=get32(d);
            F=*(char*)(d-1)!=5;
            put32(d,-F*4);
            gle32( d-prog+text+data_offset);
            gle32( N*256+F+1);
            d=h;
          }
        }
      }
      else if(!s){
        gsym1(d,z);
      }
    }
  }
}

elf_out(c){
  int glo_saved, dynstr, dynstr_size, dynsym, hash, rel, n, t, text_size;
  text=glo;
  text_size=ind-prog;
  ind=prog;
  o( 5264472);
  t = *(int*)(vars + TOK_MAIN);
  oad(232,t-ind-5);
  o( 50057);
  li(1);
  o( 32973);
  glo=glo+text_size;
  dynstr=glo;
  glo++;
  glo=strcpy(glo,"libc.so.6")+10;
  glo=strcpy(glo,"libdl.so.2")+11;
  elf_reloc(0);
  dynstr_size=glo-dynstr;
  glo=(glo+3)&-4;
  dynsym=glo;
  gle32( 0);
  gle32( 0);
  gle32( 0);
  gle32( 0);
  elf_reloc(1);
  hash=glo;
  n=(glo-dynsym)/16;
  gle32( 1);
  gle32( n);
  gle32( 1);
  gle32( 0);
  t=2;
  while( t < n) {
    gle32( t++);
  }
  gle32( 0);
  rel=glo;
  elf_reloc(2);
  memcpy(text,prog,text_size);
  glo_saved=glo;
  glo=data;
  gle32( 1179403647);
  gle32( 65793);
  gle32( 0);
  gle32( 0);
  gle32( 196610);
  gle32( 1);
  gle32( text+data_offset);
  gle32( PHDR_OFFSET);
  gle32( 0);
  gle32( 0);
  gle32( 2097204);
  gle32( 3);
  gle32( 3);
  gphdr1(INTERP_OFFSET, INTERP_SIZE);
  gle32( 4);
  gle32( 1);
  gle32( 1);
  gphdr1(0,glo_saved-data);
  gle32( 7);
  gle32( 4096);
  gle32( 2);
  gphdr1(DYNAMIC_OFFSET, DYNAMIC_SIZE);
  gle32( 6);
  gle32( 4);
  glo=strcpy(glo,"/lib/ld-linux.so.2")+20;
  gle32( 1);
  gle32( 1);
  gle32( 1);
  gle32( 11);
  gle32( 4);
  gle32( hash+data_offset);
  gle32( 6);
  gle32( dynsym+data_offset);
  gle32( 5);
  gle32( dynstr+data_offset);
  gle32( 10);
  gle32( dynstr_size);
  gle32( 11);
  gle32( 16);
  gle32( 17);
  gle32( rel+data_offset);
  gle32( 18);
  gle32( glo_saved-rel);
  gle32( 19);
  gle32( 8);
  gle32( 0);
  gle32( 0);
  t=fopen(c,"w");
  fwrite(data,1,glo_saved-data,t);
  fclose(t);
}

init_globals(){
  ALLOC_SIZE = 99999;

  ELF_BASE = 0x08048000;
  PHDR_OFFSET = 0x30;

  INTERP_OFFSET = 0x90;
  INTERP_SIZE =  0x13;

  DYNAMIC_OFFSET = (INTERP_OFFSET + INTERP_SIZE + 1);
  DYNAMIC_SIZE =  (11*8);

  ELFSTART_SIZE = (DYNAMIC_OFFSET + DYNAMIC_SIZE);
  STARTUP_SIZE =  17;

  DYNSTR_BASE  =  22;

  TOK_STR_SIZE = 48;
  TOK_IDENT  =  0x100;
  TOK_INT    =  0x100;
  TOK_IF     =  0x120;
  TOK_ELSE   =  0x138;
  TOK_WHILE  =  0x160;
  TOK_BREAK  =  0x190;
  TOK_RETURN =  0x1c0;
  TOK_FOR    =  0x1f8;
  TOK_DEFINE =  0x218;
  TOK_MAIN   =  0x250;

  TOK_DUMMY  = 1;
  TOK_NUM    = 2;

  LOCAL = 0x200;

  SYM_FORWARD = 0;
  SYM_DEFINE  = 1;

  TAG_TOK   =  ' ';
  TAG_MACRO =  2;
}


main(n,t){
  init_globals();
  if( n<3){
    printf("usage: otccelf file.c outfile\n");
    return 0;
  }
  dstk=strcpy(sym_stk = calloc(1, ALLOC_SIZE),
              " int if else while break return for define main ") + TOK_STR_SIZE;
  glo = data = calloc(1, ALLOC_SIZE);
  ind = prog = calloc(1, ALLOC_SIZE);
  vars = calloc(1, ALLOC_SIZE);

  t = t + 4;
  file=fopen(*(int*)t, "r");

  data_offset = ELF_BASE - data;
  glo = glo + ELFSTART_SIZE;
  ind = ind + STARTUP_SIZE;

  inp();
  next();
  decl(0);
  t = t + 4;
  elf_out(*(int*)t);
  return 0;
}
