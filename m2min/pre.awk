function append_in_line(l \
, chars \
, i \
, len \
){
  i=1;
  len=length(l);
  split(l, chars,"");
  if(len>=1){
    for(i=1;i<=len;i++){
#      printf("%s", chars[i]);
      in_data[in_off++]=chars[i];
    }
  }
  in_data[in_off++]="\n";
}

function load_in(pathname, flags, mode \
, myline \
) {
#  print("open name: " pathname);

  while((getline < pathname)) {
#    print "line";
    append_in_line($0);
  }
  in_len=in_off;
  in_off=0;
}

function getchar( \
eax) {
  if(in_off < in_len) {
    eax = 1;
    ch = in_data[in_off];
#      if(dbg) {
#        print("fgetc: "+String.fromCharCode(eax));
#      }
#      in_file[1]=in_file[1]+1;

#      print "fgetc f:" f " c: " eax " is: " charcode_to_str(eax);
    in_off=in_off+1;
    if(ch == "\n") {
#      print "fgetc newline: " (my_line_num++);
    }
  } else {
#      if(dbg) {
#        print("fgetc: EOF");
#      }
    eax = 0;
#    print "here2 " f;
  }
  return eax;
}


function process( \
c){
  while(c=getchar()){
    if(print_on){
    if(ch=="/"){
      getchar();
      if(ch=="*") {
        printf("slash_star");
        print_on=0;
      } else {
        print_on=1;
      }
    }
    if(print_on){
      printf("%s", ch);
    }
    } else {
      print_on=1;
    }
  }
}


BEGIN {
  print_on=1;
  load_in("./artifacts/tmp.c");
  process();
  exit 0;
}
