function join(a,j \
, i \
, res \
) {
  i=1;
  if(j!="") {
    print "can only join with the empty string"
    exit 1;
  }
  while(a[i]!=""){
    # FIXME this is quadratic probably not an issue but not sure if can fix in
    # awk
    res = res a[i];
    i=i+1;
  }
#  print "joined to: " res;
  return res;
}

function charcode_to_str(a \
, c){
  c = charcode_to_str_arr[a];
  if(c!="") {
    return c;
  }
  print "charcode_to_str invalid: "+a;
  exit 1;
}

function init_mkc( \
i, \
c, \
t) {
  for(i=0;i<256;i++){
    t=sprintf("%c",i);
    mkc_table[t]=i;
  }
  mkc_table["\'"]=39;
  for(i in mkc_table){
    c=mkc_table[i];
    charcode_to_str_arr[c] = i;
  }
}

function mkc(a \
, c) {
  c=mkc_table[a];
  if(c==""){
    print("mkc char not defined: "a);
    exit 1;
  }
#  print("mkc: " a " charcode: " c);
  return c;
}

function signed_char_to_hex(s \
, h \
, l \
, len \
, t){
# handle hex values first
  len=split(s, t, "x");
  if(len>1){
    return t[2];
  }
# non hex handling
  if(s<0) {
   s=256+s;
  }
  l=(s % 16);
  h=s-l;
  h=h / 16;
  return hexc[h] hexc[l];
}

function signed_int_to_hex(s \
, t \
, i \
, t2 \
, t3 \
, d1 \
, d2 \
, d3 \
, d4 \
, l \
) {
  t=s;
  if(split(s,t2,"x")>1) {
    d1=t2[2];
    l=split(t2[2],t3,"");
    if(l<8){
      for(i=1;i<=l;i++){
        t3[8-l+i]=t3[i];
      }
      for(i=1;i<=8-l;i++){
        t3[i]="0";
      }
    }
    d1= t3[7] t3[8] t3[5] t3[6] t3[3] t3[4] t3[1] t3[2];
  } else {
    if(s < 0){
      s=s+4294967296;
    }
    d1=signed_char_to_hex(s % 256); s=int(s/256);
    d2=signed_char_to_hex(s % 256); s=int(s/256);
    d3=signed_char_to_hex(s % 256); s=int(s/256);
    d4=signed_char_to_hex(s % 256); s=int(s/256);
  }
  return d1 d2 d3 d4;
}

function get_tok(a,s,f \
, i \
, t){
  if(a[s]=="!") {
    for(i=s+1;i<=f;i++){
      t[i-s]=a[i];
    }
    if(match(a[s+1],/[0-9-]/)) {
      r = signed_char_to_hex(join(t,""));
      return r;
    }
  }
  if(a[s]=="%") {
    if(match(a[s+1], /[0-9\-]/)) {
      for(i=s+1;i<=f;i++){
        t[i-s]=a[i];
      }
      r = signed_int_to_hex(join(t,""));
      return r;
    }
  }
  for(i=s;i<=f;i++){
    t[i-s+1]=a[i];
  }
  r= join(t,"");
#  print "TOKEN: " r;
  if(r in defines) {
    return defines[r];
  }
  return r;
}

function tmp_str_reset( \
i){
  for(i in tmp_str){
    delete tmp_str[i];
  }
  str_off=1;
}

function string_append(c){
  tmp_str[str_off++]=c;
}

function tmp_str_to_hex( \
i \
, t ){
  for(i=1;i<str_off;i++) {
    t[i]=signed_char_to_hex(mkc(tmp_str[i]));
  }
  t[i]="00";
  return join(t,"");
}

function process_line(x \
, t \
, t1 \
, t2) {
  tok_start=0;
  if(match(x,/^[ \t]*DEFINE/)) {
    split(x, t1, " ");
    defines[t1[2]]=t1[3];
    return "";
  }
  l=split(x, t1, "");
  l++;
  t1[l]="\n";
  for(i=1;i<=l;i++){
    t=t1[i];
    if(in_quote) {
      if(t1[i]=="'") {
        in_quote=0;
        printf("\n") > out_name;
      } else {
        printf("%s",t1[i]) > out_name;
      }
    } else if(in_string) {
      if(t1[i]=="\"") {
        in_string=0;
        printf("%s\n", tmp_str_to_hex()) > out_name;
#        printf("*STRING_END*\n") > out_name;
      } else {
        string_append(t1[i]);
#        printf("%s",t1[i]) > out_name;
      }
    } else {
      in_tok=0;
      if((t1[i]==";") || (t1[i]=="#")) {
        return;
      } else if((t1[i]==" ") || (t1[i]=="\t") || (t1[i]=="\n")) {
        # skip whitespace
      } else if(t1[i]=="\"") {
        in_string=1;
        tmp_str_reset();
#        printf(" *STRING_START*\n") > out_name;
      } else if(t1[i]=="'") {
        in_quote=1;
      } else {
        in_tok=1;
        if(!tok_start) {
          tok_start=i;
        }
#        printf("%s",t1[i]) > out_name;
      }
      if((in_tok==0) && tok_start) {
        t2=get_tok(t1,tok_start,i-1);
        printf("%s", t2) > out_name;
        printf("\n") > out_name;
        tok_start=0;
      }
    }
  }
  if((in_tok==1) && tok_start) {
    t2=get_tok(t1,tok_start,i-1);
    printf("%s", t2) > out_name;
    printf("\n") > out_name;
    tok_start=0;
  }
#  print x > out_name;
  return l;
}

function init_tables(){
  hexc[0]="0";
  hexc[1]="1";
  hexc[2]="2";
  hexc[3]="3";
  hexc[4]="4";
  hexc[5]="5";
  hexc[6]="6";
  hexc[7]="7";
  hexc[8]="8";
  hexc[9]="9";
  hexc[10]="A";
  hexc[11]="B";
  hexc[12]="C";
  hexc[13]="D";
  hexc[14]="E";
  hexc[15]="F";

  init_mkc();
}

BEGIN {
  init_tables();
  print "M0 awk";
  in_name= ARGV[1];
  out_name= ARGV[2];
  print "in_name: " in_name;
  print "out_name: " out_name;
#  print "# output file" > out_name;
  while((getline < in_name)) {
    process_line($0);
  }
  for(i in defines) {
#    print("DEFINE: " i " as " defines[i]);
  }
  exit;
}
