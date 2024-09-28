/^function/ {
  locals = 0;
  split($0,foo,")");
  print foo[1] " \\";
  split(foo[1],foo2,"(");
  if(foo2[2] =="") {
    print "_dummy_arg \\";
  }
  next;
}

/^\tvar/ {
  sub(/^.*var /, "", $0);
  split($0, bar, ";")
  print ", " bar[1] " \\";
  next;
}

BEGIN {
  locals = 1
}

{
  if(locals == 1) {
    print;
  } else {
    print ") {";
    print
    locals = 1
  }
}
