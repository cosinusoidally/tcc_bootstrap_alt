function process_line(x \
, t \
, t1 \
, t2) {
  print x >> out_name;
}

BEGIN {
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
