function expect(a,b){
  if(a==b) {
    print("PASS expected: " b " got: " a);
    return 1;
  } else {
    print("FAIL expected: " b " got: " a);
    exit 1;
  }
}


BEGIN {
  init_support();
  print "starting tests";
  expect(or(1,2),3);
  expect(or(2147483647,0),2147483647);
  expect(or(43690,52428),61166);
  expect(eq(1,2),0);
  expect(eq(10,10),1);

  expect(lt(10,10),0);
  expect(lt(9,10),1);
  expect(lt(10,9),0);


  expect(heap[0]=="",1);
  expect(heap[0]==0,1);


  print("and tests:");
  expect(and(1,1),1);
  expect(and(1,2),0);
  expect(and(2147483647,0),0);
  expect(and(43690,52428),34952);

  expect(or(-1,2),-1);
  exit;
}
