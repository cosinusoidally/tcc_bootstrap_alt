int e,C,J,m,T,U,K,v,P,i,ak,Q,D,V,al,Z,G,R,y;

L(a){
  *(char*) D++=a;
}

w (){
  if(V){
    m=*(char*) V++;
    if( m == 2){
      V=0;
      m=al;
    }
  }
    else m=fgetc(ak);
}

am (){
  return isalnum(m)|m == 95;
}

an (){
  if( m == 92){
    w ();
    if( m == 110)m=10;
  }
}

av(){
  int a,s,h;
  while( isspace(m)|m == 35){
    if( m == 35){
      w ();
      av();
      if( e == 536){
        av();
        L(32);
        *(int*) e=1;
        *(int*)(e+4)=D;
      }
      while( m!=10){
        L(m);
        w ();
      }
      L(m);
      L(2);
    }
    w ();
  }
  J=0;
  e=m;
  if( am ()){
    L(32);
    Z=D;
    while( am ()){
      L(m);
      w ();
    }
    if( isdigit(e)){
      C=strtol(Z,0,0);
      e=2;
    }
    else{
      *(char*) D=32;
      e=strstr(Q,Z-1)-Q;
      *(char*) D=0;
      e=e*8+256;
      if( e>536){
        e=T+e;
        if( *(int*) e == 1){
          V=*(int*)(e+4);
          al=m;
          w ();
          av();
        }
      }
    }
  }
  else{
    w ();
    if( e == 39){
      e=2;
      an ();
      C=m;
      w ();
      w ();
    }
    else if( e == 47&m == 42){
      w ();
      while( m){
        while( m!=42)w ();
        w ();
        if( m == 47)m=0;
      }
      w ();
      av();
    }
    else{
      a="++#m--%am*@R<^1c/@%[_[H3c%@%[_[H3c+@.B#d-@%:_^BKd<<Z/03e>>`/03e<=0f>=/f<@.f>@1f==&g!='g&&k||#l&@.BCh^@.BSi|@.B+j~@/%Yd!@&d*@b";
      while( s=*(char*) a++){
        h=*(char*) a++;
        C=0;
        while((J=*(char*) a++-98)<0) C=C*64+J+64;
        if( s == e&(h == m|h == 64)){
          if( h == m){
            w ();
            e=1;
          }
          break;
        }
      }
    }
  }
}

aw( d){
  while( d&&d!=-1){
    *(char*) v++=d;
    d=d>>8;
  }
}

E(a,d){
  *(char*) a++=d;
  *(char*) a++=d>>8;
  *(char*) a++=d>>16;
  *(char*) a++=d>>24;
}

ao(a){
  int d;
  return(*(char*) a&255)|(*(char*)(a+1)&255)<<8|(*(char*)(a+2)&255)<<16|(*(char*)(a+3)&255)<<24;
}

ap(a,z){
  int d;
  while( a){
    d=ao(a);
    if( *(char*)(a-1) == 5){
      if( z>=G&&z<i)E(a,z+y);
      else E(a,z-K+R+y);
    }
    else{
      E(a,z-a-4);
    }
    a=d;
  }
}

H(a){
  ap(a,v);
}

x(d,a){
  aw( d);
  E(v,a);
  a=v;
  v=v+4;
  return a;
}

M(a){
  x(184,a);
}

I(a){
  return x(233,a);
}

aa(s,a){
  aw( 1032325);
  return x(132+s,a);
}

aq(a){
  aw( 49465);
  M(0);
  aw( 15);
  aw( a+144);
  aw( 192);
}

W(s,a){
  int d;
  aw( s+131);
  d=*(int*) a;
  if( d&&d<512)x(133,d);
  else{
    a=a+4;
    *(int*) a=x(5,*(int*) a);
  }
}

ab(s){
  int d,a,h,F;
  d=1;
  if( e == 34){
    M(i+y);
    while( m!=34){
      an ();
      *(char*) i++=m;
      w ();
    }
    *(char*) i=0;
    i=i+4&-4;
    w ();
    av();
  }
  else{
    F=J;
    h=C;
    a=e;
    av();
    if( a == 2){
      M(h);
    }
    else if( F == 2){
      ab(0);
      x(185,0);
      if( a == 33)aq(h);
      else aw( h);
    }
    else if( a == 40){
      B ();
      av();
    }
    else if( a == 42){
      av();
      a=e;
      av();
      av();
      if( e == 42){
        av();
        av();
        av();
        av();
        a=0;
      }
      av();
      ab(0);
      if( e == 61){
        av();
        aw( 80);
        B ();
        aw( 89);
        aw( 392+(a == 256));
      }
      else if( a){
        if( a == 256)aw( 139);
        else aw( 48655);
        v++;
      }
    }
    else if( a == 38){
      W(10,e);
      av();
    }
    else{
      d=0;
      if( e == 61&s){
        av();
        B ();
        W(6,a);
      }
      else if( e!=40){
        W(8,a);
        if( J == 11){
          W(0,a);
          aw( C);
          av();
        }
      }
    }
  }
  if( e == 40){
    if( d)aw( 80);
    h=x(60545,0);
    av();
    s=0;
    while( e!=41){
      B ();
      x(2393225,s);
      if( e == 44)av();
      s=s+4;
    }
    E(h,s);
    av();
    if( d){
      x(2397439,s);
      s=s+4;
    }
    else{
      a=a+4;
      *(int*) a=x(232,*(int*) a);
    }
    if( s) x(50305,s);
  }
}

X(s){
  int a,d,h;
  if( s--== 1) ab(1);
  else{
    X(s);
    h=0;
    while( s == J){
      d=e;
      a=C;
      av();
      if( s>8){
        h=aa(a,h);
        X(s);
      }
      else{
        aw( 80);
        X(s);
        aw( 89);
        if( s == 4|s == 5){
          aq(a);
        }
        else{
          aw( a);
          if( d == 37)aw( 146);
        }
      }
    }
    if( h&&s>8){
      h=aa(a,h);
      M(a^1);
      I(5);
      H(h);
      M(a);
    }
  }
}

B (){
  X(11);
}

ac (){
  B ();
  return aa(0,0);
}

S(s){
  int h,d,a;
  if( e == 288){
    av();
    av();
    h=ac ();
    av();
    S(s);
    if( e == 312){
      av();
      d=I(0);
      H(h);
      S(s);
      H(d);
    }
    else{
      H(h);
    }
  }
  else if( e == 352|e == 504){
    a=e;
    av();
    av();
    if( a == 352){
      d=v;
      h=ac ();
    }
    else{
      if( e!=59)B ();
      av();
      d=v;
      h=0;
      if( e!=59)h=ac ();
      av();
      if( e!=41){
        a=I(0);
        B ();
        I(d-v-5);
        H(a);
        d=a+4;
      }
    }
    av();
    S(&h);
    I(d-v-5);
    H(h);
  }
  else if( e == 123){
    av();
    ar(1);
    while( e!=125)S(s);
    av();
  }
  else{
    if( e == 448){
      av();
      if( e!=59)B ();
      U=I(U);
    }
    else if( e == 400){
      av();
      *(int*) s=I(*(int*) s);
    }
    else if( e!=59)B ();
    av();
  }
}

ar(s){
  int h;
  while( e == 256|e!=-1&!s){
    if( e == 256){
      av();
      while( e!=59){
        if( s){
          P=P+4;
          *(int*) e=-P;
        }
        else{
          *(int*) e=i;
          i=i+4;
        }
        av();
        if( e == 44)av();
      }
      av();
    }
    else{
      *(int*) e=v;
      av();
      av();
      h=8;
      while( e!=41){
        *(int*) e=h;
        h=h+4;
        av();
        if( e == 44)av();
      }
      av();
      U=P=0;
      aw( 15042901);
      h=x(60545,0);
      S(0);
      H(U);
      aw( 50121);
      E(h,P);
    }
  }
}

ax( d){
  E(i,d);
  i=i+4;
}

ad(d,a){
  ax( d);
  d=d+134512640;
  ax( d);
  ax( d);
  ax( a);
  ax( a);
}

ae(s){
  int a,h,d,N,z,F;
  N=0;
  a=Q;
  while( 1){
    a++;
    h=a;
    while( *(char*) a!=32&&a<D)a++;
    if( a == D)break;
    e=T+(h-Q)*8+256-8;
    z=*(int*) e;
    d=*(int*)(e+4);
    if( d&&z!=1){
      if(!z){
        if(!s){
          memcpy(i,h,a-h);
          i=i+a-h+1;
        }
        else if( s == 1){
          ax( N+22);
          ax( 0);
          ax( 0);
          ax( 16);
          N=N+a-h+1;
        }
        else{
          N++;
          while( d){
            h=ao(d);
            F=*(char*)(d-1)!=5;
            E(d,-F*4);
            ax( d-K+R+y);
            ax( N*256+F+1);
            d=h;
          }
        }
      }
      else if(!s){
        ap(d,z);
      }
    }
  }
}

elf_out(c){
  int glo_saved, dynstr, dynstr_size, dynsym, hash,ah,d,a,ai;
  R=i;
  ai=v-K;
  v=K;
  aw( 5264472);
  a=*(int*)(T+592);
  x(232,a-v-5);
  aw( 50057);
  M(1);
  aw( 32973);
  i=i+ai;
  dynstr=i;
  i++;
  i=strcpy(i,"libc.so.6")+10;
  i=strcpy(i,"libdl.so.2")+11;
  ae(0);
  dynstr_size=i-dynstr;
  i=(i+3)&-4;
  dynsym=i;
  ax( 0);
  ax( 0);
  ax( 0);
  ax( 0);
  ae(1);
  hash=i;
  d=(i-dynsym)/16;
  ax( 1);
  ax( d);
  ax( 1);
  ax( 0);
  a=2;
  while( a<d)ax( a++);
  ax( 0);
  ah=i;
  ae(2);
  memcpy(R,K,ai);
  glo_saved=i;
  i=G;
  ax( 1179403647);
  ax( 65793);
  ax( 0);
  ax( 0);
  ax( 196610);
  ax( 1);
  ax( R+y);
  ax( 48);
  ax( 0);
  ax( 0);
  ax( 2097204);
  ax( 3);
  ax( 3);
  ad(144,19);
  ax( 4);
  ax( 1);
  ax( 1);
  ad(0,glo_saved-G);
  ax( 7);
  ax( 4096);
  ax( 2);
  ad(164,88);
  ax( 6);
  ax( 4);
  i=strcpy(i,"/lib/ld-linux.so.2")+20;
  ax( 1);
  ax( 1);
  ax( 1);
  ax( 11);
  ax( 4);
  ax( hash+y);
  ax( 6);
  ax( dynsym+y);
  ax( 5);
  ax( dynstr+y);
  ax( 10);
  ax( dynstr_size);
  ax( 11);
  ax( 16);
  ax( 17);
  ax( ah+y);
  ax( 18);
  ax( glo_saved-ah);
  ax( 19);
  ax( 8);
  ax( 0);
  ax( 0);
  a=fopen(c,"w");
  fwrite(G,1,glo_saved-G,a);
  fclose(a);
}

main(n,t){
  if( n<3){
    printf("usage: otccelf file.c outfile\n");
    return 0;
  }
  D=strcpy(Q =calloc(1,99999)," int if else while break return for define main ")+48;
  i=G =calloc(1,99999);
  v=K =calloc(1,99999);
  T =calloc(1,99999);
  t=t+4;
  ak=fopen(*(int*) t,"r");
  y=134512640-G;
  i=i+252;
  v=v+17;
  w ();
  av();
  ar(0);
  t=t+4;
  elf_out(*(int*) t);
  return 0;
}
