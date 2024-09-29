function append_in_line(l \
, chars \
, i \
, len \
){
  i=1;
  # replace single quotes with double
  gsub(/\('/,"(\"", l);
  gsub(/'\)/,"\")", l);
  gsub(/\"\"\"/,"\"\\\"\"", l);
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
    in_off=in_off+1;
  } else {
    eax = 0;
  }
  return eax;
}

function consume_whitespace(){
  while((ch==" ") || (ch =="\t")) {
    printf("%s", ch);
    getchar();
  }
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


function symbol( \
s \
, i \
, tmp){
  i=1;
  while(match(ch,/[a-zA-Z0-9_]/)){
#    printf("%s",ch);
    tmp[i]=ch;
    i=i+1;
    getchar();
  }
  in_off--;
  s=join(tmp,"");
  if((s=="match") || (s=="next") || (s=="sub") || (s=="close") || \
     (s=="func") || (s=="or") || (s=="and")) {
    printf("_",s);
  }
  printf("%s",s);
}

function process( \
c){
  while(c=getchar()){
    if(print_on){
    consume_whitespace();
    if(ch=="/"){
      getchar();
      if(ch=="*") {
#        printf("slash_star");
        print_on=0;
      } else {
        print_on=1;
        printf("/");
      }
    }
    if(print_on){
      if(c==-1) {
        exit;
      } else if(match(ch,/[a-zA-Z0-9_]/)){
        symbol();
      } else {
        printf("%s", ch);
      }
    }
    } else {
      if(ch=="*"){
        getchar();
        if(ch=="/") {
#          printf("slash_star");
          print_on=1;
        } else {
          print_on=0;
        }
      }
    }
  }
}


BEGIN {
  print_on=1;
  load_in("./artifacts/tmp.c");
  process();
  exit 0;
}
