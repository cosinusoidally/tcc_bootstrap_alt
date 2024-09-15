# output a byte
function ob(b){
  printf("%c",b) > out_name;
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
    printf("%c%c%c%c",65,66,67,10) > out_name;
    abs_32[offset]=ln;
    offset=offset+4;
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
    printf("%c",hex_to_byte(hi lo)) > out_name;
    offset=offset+1;
  }
#  printf("\n") > out_name;
}

function hex_to_byte(s \
, t \
){
  split(s,t,"");
  return (16*hexv[t[1]])+hexv[t[2]];
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
  base=134512724;

  # number of bytes we currently are into the output file
  offset=0;
}

function process_relocs(){

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
  print("end offset: " offset);
  exit;
}
