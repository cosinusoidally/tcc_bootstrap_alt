function process_line(x \
, t1 \
, t2) {
  if(match(x,/^DEFINE/)) {
    return "";
  }
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
  exit;
}
