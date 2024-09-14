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


function signed_char_to_hex(s \
, h \
, l ){
  if(s<0) {
   s=256+s;
  }
  l=(s % 16);
  h=s-l;
  h=h / 16;
  return hexc[h] hexc[l];
}

function signed_int_to_hex(s) {
  return "signed int: " s;
}

function get_tok(a,s,f \
, i \
, t){
  if(a[s]=="!") {
    for(i=s+1;i<=f;i++){
      t[i-s]=a[i];
    }
    r = signed_char_to_hex(join(t,""));
    return r;
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


function process_line(x \
, t \
, t1 \
, t2) {
  tok_start=0;
  if(match(x,/^DEFINE/)) {
    split(x, t1, " ");
    defines[t1[2]]=t1[3];
    return "";
  }
  l=split(x, t1, "");
  for(i=1;i<=l;i++){
    t=t1[i];
    if(in_string) {
      if(t1[i]=="\"") {
        in_string=0;
        printf("\n*STRING_END*\n") > out_name;
      } else {
        printf("%s",t1[i]) > out_name;
      }
    } else {
      in_tok=0;
      if((t1[i]==";")) {
        return;
      } else if((t1[i]==" ") || (t1[i]=="\t")) {
        # skip whitespace
      } else if(t1[i]=="\"") {
        in_string=1;
        printf(" *STRING_START*\n") > out_name;
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
}

BEGIN {
  init_tables();
  print "M0 awk";
  in_name= ARGV[1];
  out_name= ARGV[2];
  print "in_name: " in_name;
  print "out_name: " out_name;
  print "# output file" > out_name;
  while((getline < in_name)) {
    process_line($0);
  }
  for(i in defines) {
#    print("DEFINE: " i " as " defines[i]);
  }
  exit;
}
