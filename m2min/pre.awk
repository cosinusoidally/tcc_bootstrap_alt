function append_in_line(l \
, chars \
, i \
){
  i=1;
  split(l, chars,"");
  while(chars[i]!=""){
    in_data[in_off++]=chars[i];
    i=i+1;
  }
  in_data[in_off++]="\n";
}

function load_in(pathname, flags, mode \
, myline \
) {
  print("open name: " pathname);

  while((getline < pathname)) {
    append_in_line($0);
  }
  in_len=in_off;
  in_off=0;
}

function getchar( \
eax) {
  if(in_off < in_len) {
    eax = in_data[in_off];
#      if(dbg) {
#        print("fgetc: "+String.fromCharCode(eax));
#      }
#      in_file[1]=in_file[1]+1;

#      print "fgetc f:" f " c: " eax " is: " charcode_to_str(eax);
    in_off=in_off+1;
    if(eax == "\n") {
      print "fgetc newline: " (my_line_num++);
    }
  } else {
#      if(dbg) {
#        print("fgetc: EOF");
#      }
    eax = -1;
    print "here2 " f;
  }
  return eax;
}


function process( \
c){
  while(c=getchar()){
    print c;
  }
}


BEGIN {
  load_in("./artifacts/tmp.c");
  process();
  exit 0;
}
