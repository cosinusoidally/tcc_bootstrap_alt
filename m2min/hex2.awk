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

# it would be good if there were a nicer way of doing this
bt[0]="\x0";
bt[1]="\x1";
bt[2]="\x2";
bt[3]="\x3";
bt[4]="\x4";
bt[5]="\x5";
bt[6]="\x6";
bt[7]="\x7";
bt[8]="\x8";
bt[9]="\x9";
bt[10]="\xa";
bt[11]="\xb";
bt[12]="\xc";
bt[13]="\xd";
bt[14]="\xe";
bt[15]="\xf";
bt[16]="\x10";
bt[17]="\x11";
bt[18]="\x12";
bt[19]="\x13";
bt[20]="\x14";
bt[21]="\x15";
bt[22]="\x16";
bt[23]="\x17";
bt[24]="\x18";
bt[25]="\x19";
bt[26]="\x1a";
bt[27]="\x1b";
bt[28]="\x1c";
bt[29]="\x1d";
bt[30]="\x1e";
bt[31]="\x1f";
bt[32]="\x20";
bt[33]="\x21";
bt[34]="\x22";
bt[35]="\x23";
bt[36]="\x24";
bt[37]="\x25";
bt[38]="\x26";
bt[39]="\x27";
bt[40]="\x28";
bt[41]="\x29";
bt[42]="\x2a";
bt[43]="\x2b";
bt[44]="\x2c";
bt[45]="\x2d";
bt[46]="\x2e";
bt[47]="\x2f";
bt[48]="\x30";
bt[49]="\x31";
bt[50]="\x32";
bt[51]="\x33";
bt[52]="\x34";
bt[53]="\x35";
bt[54]="\x36";
bt[55]="\x37";
bt[56]="\x38";
bt[57]="\x39";
bt[58]="\x3a";
bt[59]="\x3b";
bt[60]="\x3c";
bt[61]="\x3d";
bt[62]="\x3e";
bt[63]="\x3f";
bt[64]="\x40";
bt[65]="\x41";
bt[66]="\x42";
bt[67]="\x43";
bt[68]="\x44";
bt[69]="\x45";
bt[70]="\x46";
bt[71]="\x47";
bt[72]="\x48";
bt[73]="\x49";
bt[74]="\x4a";
bt[75]="\x4b";
bt[76]="\x4c";
bt[77]="\x4d";
bt[78]="\x4e";
bt[79]="\x4f";
bt[80]="\x50";
bt[81]="\x51";
bt[82]="\x52";
bt[83]="\x53";
bt[84]="\x54";
bt[85]="\x55";
bt[86]="\x56";
bt[87]="\x57";
bt[88]="\x58";
bt[89]="\x59";
bt[90]="\x5a";
bt[91]="\x5b";
bt[92]="\x5c";
bt[93]="\x5d";
bt[94]="\x5e";
bt[95]="\x5f";
bt[96]="\x60";
bt[97]="\x61";
bt[98]="\x62";
bt[99]="\x63";
bt[100]="\x64";
bt[101]="\x65";
bt[102]="\x66";
bt[103]="\x67";
bt[104]="\x68";
bt[105]="\x69";
bt[106]="\x6a";
bt[107]="\x6b";
bt[108]="\x6c";
bt[109]="\x6d";
bt[110]="\x6e";
bt[111]="\x6f";
bt[112]="\x70";
bt[113]="\x71";
bt[114]="\x72";
bt[115]="\x73";
bt[116]="\x74";
bt[117]="\x75";
bt[118]="\x76";
bt[119]="\x77";
bt[120]="\x78";
bt[121]="\x79";
bt[122]="\x7a";
bt[123]="\x7b";
bt[124]="\x7c";
bt[125]="\x7d";
bt[126]="\x7e";
bt[127]="\x7f";
bt[128]="\x80";
bt[129]="\x81";
bt[130]="\x82";
bt[131]="\x83";
bt[132]="\x84";
bt[133]="\x85";
bt[134]="\x86";
bt[135]="\x87";
bt[136]="\x88";
bt[137]="\x89";
bt[138]="\x8a";
bt[139]="\x8b";
bt[140]="\x8c";
bt[141]="\x8d";
bt[142]="\x8e";
bt[143]="\x8f";
bt[144]="\x90";
bt[145]="\x91";
bt[146]="\x92";
bt[147]="\x93";
bt[148]="\x94";
bt[149]="\x95";
bt[150]="\x96";
bt[151]="\x97";
bt[152]="\x98";
bt[153]="\x99";
bt[154]="\x9a";
bt[155]="\x9b";
bt[156]="\x9c";
bt[157]="\x9d";
bt[158]="\x9e";
bt[159]="\x9f";
bt[160]="\xa0";
bt[161]="\xa1";
bt[162]="\xa2";
bt[163]="\xa3";
bt[164]="\xa4";
bt[165]="\xa5";
bt[166]="\xa6";
bt[167]="\xa7";
bt[168]="\xa8";
bt[169]="\xa9";
bt[170]="\xaa";
bt[171]="\xab";
bt[172]="\xac";
bt[173]="\xad";
bt[174]="\xae";
bt[175]="\xaf";
bt[176]="\xb0";
bt[177]="\xb1";
bt[178]="\xb2";
bt[179]="\xb3";
bt[180]="\xb4";
bt[181]="\xb5";
bt[182]="\xb6";
bt[183]="\xb7";
bt[184]="\xb8";
bt[185]="\xb9";
bt[186]="\xba";
bt[187]="\xbb";
bt[188]="\xbc";
bt[189]="\xbd";
bt[190]="\xbe";
bt[191]="\xbf";
bt[192]="\xc0";
bt[193]="\xc1";
bt[194]="\xc2";
bt[195]="\xc3";
bt[196]="\xc4";
bt[197]="\xc5";
bt[198]="\xc6";
bt[199]="\xc7";
bt[200]="\xc8";
bt[201]="\xc9";
bt[202]="\xca";
bt[203]="\xcb";
bt[204]="\xcc";
bt[205]="\xcd";
bt[206]="\xce";
bt[207]="\xcf";
bt[208]="\xd0";
bt[209]="\xd1";
bt[210]="\xd2";
bt[211]="\xd3";
bt[212]="\xd4";
bt[213]="\xd5";
bt[214]="\xd6";
bt[215]="\xd7";
bt[216]="\xd8";
bt[217]="\xd9";
bt[218]="\xda";
bt[219]="\xdb";
bt[220]="\xdc";
bt[221]="\xdd";
bt[222]="\xde";
bt[223]="\xdf";
bt[224]="\xe0";
bt[225]="\xe1";
bt[226]="\xe2";
bt[227]="\xe3";
bt[228]="\xe4";
bt[229]="\xe5";
bt[230]="\xe6";
bt[231]="\xe7";
bt[232]="\xe8";
bt[233]="\xe9";
bt[234]="\xea";
bt[235]="\xeb";
bt[236]="\xec";
bt[237]="\xed";
bt[238]="\xee";
bt[239]="\xef";
bt[240]="\xf0";
bt[241]="\xf1";
bt[242]="\xf2";
bt[243]="\xf3";
bt[244]="\xf4";
bt[245]="\xf5";
bt[246]="\xf6";
bt[247]="\xf7";
bt[248]="\xf8";
bt[249]="\xf9";
bt[250]="\xfa";
bt[251]="\xfb";
bt[252]="\xfc";
bt[253]="\xfd";
bt[254]="\xfe";
bt[255]="\xff";
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
  for(i=0;i<offset;i=i+1){
    if(use_bt) {
      printf(bt[out_data[i]]) > out_name;
    } else {
      printf("%c",out_data[i]) > out_name;
    }
  }
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


