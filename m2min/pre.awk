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



BEGIN {
  load_in("./artifacts/tmp.c");
  exit 0;
}
