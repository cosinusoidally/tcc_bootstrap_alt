var tok, tokc, tokl, ch, vars, rsym, prog, ind, loc, glo, file, sym_stk, dstk,dptr,dch,last_id, data, text, data_offset;

var ALLOC_SIZE;

var ELF_BASE;
var PHDR_OFFSET, INTERP_OFFSET, INTERP_SIZE, DYNAMIC_OFFSET, DYNAMIC_SIZE;
var ELFSTART_SIZE, STARTUP_SIZE, DYNSTR_BASE;

var TOK_STR_SIZE;
var TOK_IDENT, TOK_INT, TOK_IF, TOK_ELSE, TOK_WHILE, TOK_BREAK, TOK_RETURN;
var TOK_FOR, TOK_DEFINE, TOK_MAIN, TOK_DUMMY, TOK_NUM, LOCAL, SYM_FORWARD;
var SYM_DEFINE, TAG_TOK, TAG_MACRO;

function pdef(t){
  wi8(dstk, t);
  dstk = dstk + 1;
}

function inp (){
  if(dptr){
    ch = ri8(dptr);
    dptr = dptr + 1;
    if( ch == TAG_MACRO){
      dptr=0;
      ch=dch;
    }
  }
    else ch=fgetc(file);
}

function isid (){
  return isalnum(ch) | ch == '_';
}

function getq (){
  if( ch == '\\'){
    inp ();
    if( ch == 'n') {
      ch = '\n';
    }
  }
}

function next(){
  var t, l, a;
  while( isspace(ch) | ch == '#'){
    if( ch == '#'){
      inp ();
      next();
      if( tok == TOK_DEFINE){
        next();
        pdef(TAG_TOK);
        wi32(tok, 1);
        wi32(tok+4, dstk);
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
    } else{
      wi8(dstk, TAG_TOK);
      tok = strstr(sym_stk, last_id - 1) - sym_stk;
      wi8(dstk, 0);
      tok=tok * 8 + TOK_IDENT;
      if( tok>TOK_DEFINE){
        tok=vars+tok;
        if( ri32(tok) == SYM_DEFINE){
          dptr = ri32(tok+4);
          dch=ch;
          inp ();
          next();
        }
      }
    }
  } else{
    inp ();
    if( tok == '\''){
      tok = TOK_NUM;
      getq ();
      tokc=ch;
      inp ();
      inp ();
    } else if( tok == '/' & ch == '*'){
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
    } else{
      t="++#m--%am*@R<^1c/@%[_[H3c%@%[_[H3c+@.B#d-@%:_^BKd<<Z/03e>>`/03e<=0f>=/f<@.f>@1f==&g!='g&&k||#l&@.BCh^@.BSi|@.B+j~@/%Yd!@&d*@b";
      while( l = ri8(t)){
        t = t + 1;
        a = ri8(t);
        t = t + 1;
        tokc=0;
        while((tokl = ri8(t) - 'b')<0) {
          t = t + 1;
          tokc = tokc * 64 + tokl + 64;
        }
        t = t + 1;
        if( l == tok & (a == ch | a == '@')){
          if( a == ch){
            inp ();
            tok = TOK_DUMMY;
          }
          break;
        }
      }
      t = t + 1;
    }
  }
}

function o( n){
  while( n && n!=-1){
    wi8(ind, n);
    ind = ind + 1;
    n = n >> 8;
  }
}

function put32(t, n){
  wi8(t, n);     t = t + 1;
  wi8(t,  n>>8); t = t + 1;
  wi8(t, n>>16); t = t + 1;
  wi8(t, n>>24); t = t + 1;
}

function get32(t){
  var n;
  return (ri8(t)&255)       | (ri8(t+1)&255)<<8 |
         (ri8(t+2)&255)<<16 | (ri8(t+3)&255)<<24;
}

function gsym1(t, b){
  var d;
  while( t){
    d=get32(t);
    if( ri8(t-1) == 5){
      if( b >= data && b < glo)
        put32(t, b + data_offset);
      else
        put32(t, b - prog + text + data_offset);
    }
    else{
      put32(t, b - t - 4);
    }
    t = d;
  }
}

function gsym(t){
  gsym1(t, ind);
}

function oad(n, t){
  o( n);
  put32(ind, t);
  t = ind;
  ind = ind + 4;
  return t;
}

function li(t){
  oad(184, t);
}

function gjmp(t){
  return oad(233, t);
}

function gtst(l, t){
  o( 1032325);
  return oad(132+l, t);
}

function gcmp(t){
  o( 49465);
  li(0);
  o( 15);
  o( t+144);
  o( 192);
}

function gmov(l, t){
  var d;
  o( l+131);
  d = ri32(t);
  if( d && d < LOCAL) {
    oad(133,d);
  } else {
    t = t + 4;
    wi32(t, oad(5,ri32(t)));
  }
}

function unary(l){
  var n, t, a, c;
  n=1;
  if( tok == '\"'){
    li(glo+data_offset);
    while( ch!='\"'){
      getq ();
      wi8(glo, ch);
      glo = glo + 1;
      inp ();
    }
    wi8(glo, 0);
    glo=glo+4&-4;
    inp ();
    next();
  } else {
    c=tokl;
    a=tokc;
    t=tok;
    next();
    if( t == TOK_NUM){
      li(a);
    }
    else if( c == 2){
      unary(0);
      oad(185,0);
      if( t == '!') {
        gcmp(a);
      } else {
        o( a);
      }
    }
    else if( t == '('){
      expr ();
      next();
    }
    else if( t == '*'){
      next();
      t=tok;
      next();
      next();
      if( tok == '*'){
        next();
        next();
        next();
        next();
        t=0;
      }
      next();
      unary(0);
      if( tok == '='){
        next();
        o( 80);
        expr ();
        o( 89);
        o( 392+(t == TOK_INT));
      } else if( t){
        if( t == TOK_INT) {
          o( 139);
        } else {
          o( 48655);
        }
        ind++;
      }
    } else if( t == '&'){
      gmov(10,tok);
      next();
    }
    else{
      n=0;
      if( tok == '=' & l){
        next();
        expr ();
        gmov(6,t);
      } else if( tok != '('){
        gmov(8,t);
        if( tokl == 11){
          gmov(0,t);
          o( tokc);
          next();
        }
      }
    }
  }
  if( tok == '('){
    if( n)o( 80);
    a=oad(60545,0);
    next();
    l=0;
    while( tok != ')'){
      expr ();
      oad(2393225,l);
      if( tok == ',') {
        next();
      }
      l=l+4;
    }
    put32(a,l);
    next();
    if( n){
      oad(2397439,l);
      l=l+4;
    }
    else{
      t=t+4;
      wi32(t, oad(232,ri32(t)));
    }
    if( l) oad(50305,l);
  }
}

function sum(l){
  var t, n, a;
  if( l--== 1) unary(1);
  else{
    sum(l);
    a=0;
    while( l == tokl){
      n=tok;
      t=tokc;
      next();
      if( l>8){
        a=gtst(t,a);
        sum(l);
      }
      else{
        o( 80);
        sum(l);
        o( 89);
        if( l == 4|l == 5){
          gcmp(t);
        }
        else{
          o( t);
          if( n == '%') {
            o( 146);
          }
        }
      }
    }
    if( a&&l>8){
      a=gtst(t,a);
      li(t^1);
      gjmp(5);
      gsym(a);
      li(t);
    }
  }
}

function expr (){
  sum(11);
}

function test_expr (){
  expr ();
  return gtst(0,0);
}

function block(l){
  var a , n, t;
  enter();
  if( tok == TOK_IF){
    next();
    next();
    a=test_expr ();
    next();
    block(l);
    if( tok == TOK_ELSE){
      next();
      n=gjmp(0);
      gsym(a);
      block(l);
      gsym(n);
    }
    else{
      gsym(a);
    }
  }
  else if( tok == TOK_WHILE | tok == TOK_FOR){
    t=tok;
    next();
    next();
    if( t == TOK_WHILE){
      n=ind;
      a=test_expr ();
    }
    else{
      if( tok != ';') {
        expr ();
      }
      next();
      n=ind;
      a=0;
      if( tok != ';') {
        a=test_expr ();
      }
      next();
      if( tok != ')'){
        t=gjmp(0);
        expr ();
        gjmp(n-ind-5);
        gsym(t);
        n=t+4;
      }
    }
    next();
    block(&a);
    gjmp(n-ind-5);
    gsym(a);
  }
  else if( tok == '{'){
    next();
    decl(1);
    while( tok != '}') {
      block(l);
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
      wi32(l, gjmp(ri32(l)));
    } else if( tok != ';') {
      expr ();
    }
    next();
  }
  leave(0);
}

function decl(l){
  var a;
  while( tok == TOK_INT | tok != -1 & !l){
    if( tok == TOK_INT){
      next();
      while( tok != ';'){
        if( l){
          loc=loc+4;
          wi32(tok, -loc);
        }
        else{
          wi32(tok, glo);
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
      wi32(tok, ind);
      next();
      next();
      a=8;
      while( tok != ')'){
        wi32(tok, a);
        a=a+4;
        next();
        if( tok == ',') {
          next();
        }
      }
      next();
      rsym=loc=0;
      o( 15042901);
      a=oad(60545,0);
      block(0);
      gsym(rsym);
      o( 50121);
      put32(a,loc);
    }
  }
}

function gle32( n){
  put32(glo,n);
  glo=glo+4;
}

function gphdr1(n,t){
  gle32( n);
  n = n + ELF_BASE;
  gle32( n);
  gle32( n);
  gle32( t);
  gle32( t);
}

function elf_reloc(l){
  var t,a,n,p,b,c;
  p=0;
  t=sym_stk;
  while( 1){
    t++;
    a=t;
    while( ri8(t) != TAG_TOK && t < dstk) {
      t++;
    }
    if( t == dstk)break;
    tok = vars + (a - sym_stk) * 8 + TOK_IDENT - 8;
    b = ri32(tok);
    n = ri32(tok+4);
    if( n&&b!=1){
      if(!b){
        if(!l){
          memcpy(glo,a,t-a);
          glo=glo+t-a+1;
        } else if( l == 1){
          gle32( p + DYNSTR_BASE);
          gle32( 0);
          gle32( 0);
          gle32( 16);
          p=p+t-a+1;
        } else{
          p++;
          while( n){
            a=get32(n);
            c = ri8(n-1)!=5;
            put32(n,-c*4);
            gle32( n-prog+text+data_offset);
            gle32( p*256+c+1);
            n=a;
          }
        }
      } else if(!l){
        gsym1(n,b);
      }
    }
  }
}

function elf_out(c){
  var glo_saved, dynstr, dynstr_size, dynsym, hash, rel, n, t, text_size;
  text=glo;
  text_size=ind-prog;
  ind=prog;
  o( 5264472);
  t = ri32(vars + TOK_MAIN);
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

function init_globals(){
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


function main(n,t){
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
  file=fopen(ri32(t), "r");

  data_offset = ELF_BASE - data;
  glo = glo + ELFSTART_SIZE;
  ind = ind + STARTUP_SIZE;

  inp();
  next();
  decl(0);
  t = t + 4;
  elf_out(ri32(t));
  return 0;
}
