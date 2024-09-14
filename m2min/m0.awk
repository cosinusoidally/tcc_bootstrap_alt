function process_line(x \
, t \
, t1 \
, t2) {
  if(match(x,/^DEFINE/)) {
    split(x, t1, " ");
    defines[t1[2]]=t1[3];
    return "";
  }
  l=split(x, t1, "");
  for(i=1;i<=l;i++){
    t=t1[i];
    if(in_string) {
      printf("%s",t1[i]) > out_name;
      if(t1[i]=="\"") {
        in_string=0;
      }
    } else {
      if((t1[i]==";")) {
        return;
      } else if((t1[i]==" ") || (t1[i]=="\t")) {
        # skip whitespace
      } else if(t1[i]=="\"") {
        in_string=1;
        printf("\n") > out_name;
      } else {
        printf("%s",t1[i]) > out_name;
      }
    }
  }
  printf("\n") > out_name;
#  print x > out_name;
  return l;
}

BEGIN {
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
