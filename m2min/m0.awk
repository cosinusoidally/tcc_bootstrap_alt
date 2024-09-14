function process_line(x \
, t1 \
, t2) {
  if(match(x,/^DEFINE/)) {
    split(x, t1, " ");
    defines[t1[2]]=t1[3];
    return "";
  }
  split(x, t1, "");
  return x;
}

BEGIN {
  print "M0 awk";
  in_name= ARGV[1];
  out_name= ARGV[2];
  print "in_name: " in_name;
  print "out_name: " out_name;
  print "# output file" > out_name;
  while((getline < in_name)) {
    $0=process_line($0);
    print $0 > out_name;
  }
  for(i in defines) {
#    print("DEFINE: " i " as " defines[i]);
  }
  exit;
}
