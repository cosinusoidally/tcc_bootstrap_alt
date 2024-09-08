function expect(a,b){
  if(a==b) {
    return 1;
  } else {
    print("expected: " b " got: " a);
    exit 1;
  }
}


BEGIN {
  print "starting tests";
  print expect(or(1,2),3);
  print or(-1,2);
  exit;
}
