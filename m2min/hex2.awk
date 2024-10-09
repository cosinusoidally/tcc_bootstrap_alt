# output a byte
function ob(b){
#  printf("%c",b) > out_name;
  out_data[offset]=b;
  offset=offset+1;
}

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

function process_line(x \
, t \
, l \
, i \
, h \
, lo \
, s \
, ln \
) {
  l=split(x, t, "");
  if(t[1]=="%"){
#    print "rel_32";
    for(i=2;i<=l;i=i+1){
      s[i-1]=t[i];
    }
    ln=join(s,"");
    rel_32[offset]=ln;
    ob(69);ob(70);ob(71);ob(10);
    return;
  }
  if(t[1]=="&"){
#    print "abs_32";
    for(i=2;i<=l;i=i+1){
      s[i-1]=t[i];
    }
    ln=join(s,"");
    abs_32[offset]=ln;
    ob(65);ob(66);ob(67);ob(10);
    return;
  }
  if(t[1]==":"){
    for(i=2;i<=l;i=i+1){
      s[i-1]=t[i];
    }
    ln=join(s,"");
#    print "label " ln " " offset;
    labels[ln]=offset;
    return;
  }
  for(i=1;i<=l;i=i+2){
    hi=t[i];
    lo=t[i+1];
#    printf("%s",(hi lo)) > out_name;
    ob(hex_to_byte(hi lo));
  }
#  printf("\n") > out_name;
}

function hex_to_byte(s \
, t \
){
  split(s,t,"");
  return (16*hexv[toupper(t[1])])+hexv[toupper(t[2])];
}


function init( \
i \
){
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

  for(i in hexc){
    hexv[hexc[i]]=i;
  }

  # elf base address
  base=134512724-84;

  # number of bytes we currently are into the output file
  offset=0;
}

function process_relocs( \
v \
, name \
, t){
  for(i in abs_32){
    v=labels[abs_32[i]]+base;
    print "patching abs_32: " i " to label: " abs_32[i] " v: " v;
    out_data[i]= v % 256;
    v=int(v/256);
    out_data[i+1]= v % 256;
    v=int(v/256);
    out_data[i+2]= v % 256;
    v=int(v/256);
    out_data[i+3]= v % 256;
  }

  for(i in rel_32){
    name=rel_32[i];
    v=labels[name]-i-4;
    if(split(name,t,">")>1) {
      v=labels[t[1]]-labels[t[2]];
    }
    print "patching rel_32: " i " to label: " name " v: " v;
    if(v<0){
      v=v+4294967296;
    }
    out_data[i]= v % 256;
    v=int(v/256);
    out_data[i+1]= v % 256;
    v=int(v/256);
    out_data[i+2]= v % 256;
    v=int(v/256);
    out_data[i+3]= v % 256;
  }

}

function write_data( \
i){
#  use_to_bin="bash";
  cmd=use_to_bin " ./to_bin.sh " out_name;
  for(i=0;i<offset;i=i+1){
    if(use_to_bin) {
# busybox awk needs to use this method as it ignores LC_ALL=C
      printf("%s\n", signed_char_to_hex(out_data[i])) | cmd;
    } else {
      printf("%c",out_data[i]) > out_name;
    }
  }
  if(use_to_bin) {
    close(cmd);
  }
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

BEGIN {
  init();
  print "hex2 awk";
  in_name= ARGV[1];
  out_name= ARGV[2];
  print "in_name: " in_name;
  print "out_name: " out_name;
#  print "# output file" > out_name;
  while((getline < in_name)) {
    process_line($0);
  }
#  for(i in labels){
#    print "label: " i ":" labels[i];
#  }
  process_relocs();
  write_data();
  print("end offset: " offset);
  exit;
}
