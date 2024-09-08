/^function/ {
  split($0,foo,")");
  print foo[1];
  next;
}

/^\tvar/ {
  decl = 0;
  sub(/^.*var /, "", $0);
  split($0, bar, ";")
  print ", " bar[1];
  next;
}

BEGIN {
  decl = 1
}

{
  if(decl == 1) {
    print;
  } else {
    print "){";
    print
    decl = 1
  }
}
