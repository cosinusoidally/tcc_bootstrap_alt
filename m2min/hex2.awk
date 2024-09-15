function process_line(x \
, t \
, l \
, i \
, h \
, lo \
) {
  l=split(x, t, "");
  if(t[1]=="%"){
#    print "rel_32";
    printf("%c%c%c%c",69,70,71,10) > out_name;
    return;
  }
  if(t[1]=="&"){
#    print "abs_32";
    printf("%c%c%c%c",65,66,67,10) > out_name;
    return;
  }
  if(t[1]==":"){
#    print "label";
    return;
  }
  for(i=1;i<=l;i=i+2){
    hi=t[i];
    lo=t[i+1];
    printf("%s",hi) > out_name;
    printf("%s",lo) > out_name;
  }
  printf("\n") > out_name;
}

function hex_to_byte(s \
, h \
, l ){
  l=(s % 16);
  h=s-l;
  h=h / 16;
  return hexc[h] hexc[l];
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
  print "hex2 awk";
  in_name= ARGV[1];
  out_name= ARGV[2];
  print "in_name: " in_name;
  print "out_name: " out_name;
#  print "# output file" > out_name;
  while((getline < in_name)) {
    process_line($0);
  }
  exit;
}
