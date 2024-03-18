int tok, tokc, tokl, ch, vars, rsym, prog, ind, loc, glo, file, sym_stk, dstk,dptr,dch,last_id, data, text, data_offset;

pdef(a){
  *(char*) dstk++=a;
}

inp (){
  if(dptr){
    ch=*(char*) dptr++;
    if( ch == 2){
      dptr=0;
      ch=dch;
    }
  }
    else ch=fgetc(file);
}

isid (){
  return isalnum(ch)|ch == 95;
}

getq (){
  if( ch == 92){
    inp ();
    if( ch == 110)ch=10;
  }
}

next(){
  int a,s,h;
  while( isspace(ch)|ch == 35){
    if( ch == 35){
      inp ();
      next();
      if( tok == 536){
        next();
        pdef(32);
        *(int*) tok=1;
        *(int*)(tok+4)=dstk;
      }
      while( ch!=10){
        pdef(ch);
        inp ();
      }
      pdef(ch);
      pdef(2);
    }
    inp ();
  }
  tokl=0;
  tok=ch;
  if( isid ()){
    pdef(32);
    last_id=dstk;
    while( isid ()){
      pdef(ch);
      inp ();
    }
    if( isdigit(tok)){
      tokc=strtol(last_id,0,0);
      tok=2;
    }
    else{
      *(char*) dstk=32;
      tok=strstr(sym_stk,last_id-1)-sym_stk;
      *(char*) dstk=0;
      tok=tok*8+256;
      if( tok>536){
        tok=vars+tok;
        if( *(int*) tok == 1){
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
    if( tok == 39){
      tok=2;
      getq ();
      tokc=ch;
      inp ();
      inp ();
    }
    else if( tok == 47&ch == 42){
      inp ();
      while( ch){
        while( ch!=42)inp ();
        inp ();
        if( ch == 47)ch=0;
      }
      inp ();
      next();
    }
    else{
      a="++#m--%am*@R<^1c/@%[_[H3c%@%[_[H3c+@.B#d-@%:_^BKd<<Z/03e>>`/03e<=0f>=/f<@.f>@1f==&g!='g&&k||#l&@.BCh^@.BSi|@.B+j~@/%Yd!@&d*@b";
      while( s=*(char*) a++){
        h=*(char*) a++;
        tokc=0;
        while((tokl=*(char*) a++-98)<0) tokc=tokc*64+tokl+64;
        if( s == tok&(h == ch|h == 64)){
          if( h == ch){
            inp ();
            tok=1;
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
  if( d&&d<512)oad(133,d);
  else{
    a=a+4;
    *(int*) a=oad(5,*(int*) a);
  }
}

unary(s){
  int d,a,h,F;
  d=1;
  if( tok == 34){
    li(glo+data_offset);
    while( ch!=34){
      getq ();
      *(char*) glo++=ch;
      inp ();
    }
    *(char*) glo=0;
    glo=glo+4&-4;
    inp ();
    next();
  }
  else{
    F=tokl;
    h=tokc;
    a=tok;
    next();
    if( a == 2){
      li(h);
    }
    else if( F == 2){
      unary(0);
      oad(185,0);
      if( a == 33)gcmp(h);
      else o( h);
    }
    else if( a == 40){
      B ();
      next();
    }
    else if( a == 42){
      next();
      a=tok;
      next();
      next();
      if( tok == 42){
        next();
        next();
        next();
        next();
        a=0;
      }
      next();
      unary(0);
      if( tok == 61){
        next();
        o( 80);
        B ();
        o( 89);
        o( 392+(a == 256));
      }
      else if( a){
        if( a == 256)o( 139);
        else o( 48655);
        ind++;
      }
    }
    else if( a == 38){
      gmov(10,tok);
      next();
    }
    else{
      d=0;
      if( tok == 61&s){
        next();
        B ();
        gmov(6,a);
      }
      else if( tok!=40){
        gmov(8,a);
        if( tokl == 11){
          gmov(0,a);
          o( tokc);
          next();
        }
      }
    }
  }
  if( tok == 40){
    if( d)o( 80);
    h=oad(60545,0);
    next();
    s=0;
    while( tok!=41){
      B ();
      oad(2393225,s);
      if( tok == 44)next();
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
          if( d == 37)o( 146);
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

B (){
  sum(11);
}

ac (){
  B ();
  return gtst(0,0);
}

S(s){
  int h,d,a;
  if( tok == 288){
    next();
    next();
    h=ac ();
    next();
    S(s);
    if( tok == 312){
      next();
      d=gjmp(0);
      gsym(h);
      S(s);
      gsym(d);
    }
    else{
      gsym(h);
    }
  }
  else if( tok == 352|tok == 504){
    a=tok;
    next();
    next();
    if( a == 352){
      d=ind;
      h=ac ();
    }
    else{
      if( tok!=59)B ();
      next();
      d=ind;
      h=0;
      if( tok!=59)h=ac ();
      next();
      if( tok!=41){
        a=gjmp(0);
        B ();
        gjmp(d-ind-5);
        gsym(a);
        d=a+4;
      }
    }
    next();
    S(&h);
    gjmp(d-ind-5);
    gsym(h);
  }
  else if( tok == 123){
    next();
    decl(1);
    while( tok!=125)S(s);
    next();
  }
  else{
    if( tok == 448){
      next();
      if( tok!=59)B ();
      rsym=gjmp(rsym);
    }
    else if( tok == 400){
      next();
      *(int*) s=gjmp(*(int*) s);
    }
    else if( tok!=59)B ();
    next();
  }
}

decl(s){
  int h;
  while( tok == 256|tok!=-1&!s){
    if( tok == 256){
      next();
      while( tok!=59){
        if( s){
          loc=loc+4;
          *(int*) tok=-loc;
        }
        else{
          *(int*) tok=glo;
          glo=glo+4;
        }
        next();
        if( tok == 44)next();
      }
      next();
    }
    else{
      *(int*) tok=ind;
      next();
      next();
      h=8;
      while( tok!=41){
        *(int*) tok=h;
        h=h+4;
        next();
        if( tok == 44)next();
      }
      next();
      rsym=loc=0;
      o( 15042901);
      h=oad(60545,0);
      S(0);
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
  d=d+134512640;
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
    while( *(char*) a!=32&&a<dstk)a++;
    if( a == dstk)break;
    tok=vars+(h-sym_stk)*8+256-8;
    z=*(int*) tok;
    d=*(int*)(tok+4);
    if( d&&z!=1){
      if(!z){
        if(!s){
          memcpy(glo,h,a-h);
          glo=glo+a-h+1;
        }
        else if( s == 1){
          gle32( N+22);
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
  t=*(int*)(vars+592);
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
  while( t<n)gle32( t++);
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
  gle32( 48);
  gle32( 0);
  gle32( 0);
  gle32( 2097204);
  gle32( 3);
  gle32( 3);
  gphdr1(144,19);
  gle32( 4);
  gle32( 1);
  gle32( 1);
  gphdr1(0,glo_saved-data);
  gle32( 7);
  gle32( 4096);
  gle32( 2);
  gphdr1(164,88);
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

main(n,t){
  if( n<3){
    printf("usage: otccelf file.c outfile\n");
    return 0;
  }
  dstk=strcpy(sym_stk = calloc(1,99999),
              " int if else while break return for define main ")+48;
  glo = data = calloc(1,99999);
  ind = prog = calloc(1,99999);
  vars = calloc(1,99999);

  t = t + 4;
  file=fopen(*(int*)t, "r");

  data_offset=134512640-data;
  glo=glo+252;
  ind=ind+17;

  inp();
  next();
  decl(0);
  t = t + 4;
  elf_out(*(int*)t);
  return 0;
}
