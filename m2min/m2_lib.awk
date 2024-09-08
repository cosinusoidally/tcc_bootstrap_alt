function fopen(a,b,c){
  print "fclose not impl"
  exit
}

function fclose(a,b,c){
  print "fclose not impl"
  exit
}

function fputs(a,b,c){
  print "fputs not impl"
  exit
}

function mks(a,b,c){
  print "mks not impl"
  exit
}

function add(a,b) {
  a = or(a, 0);
  b = or(b, 0);
  return a + b;
}

function mul(a,b,c){
  a=or(a,0);
  b=or(b,0);
  return a*b;
}

function brk(addr) {
  addr = or(addr,0);
  if(addr==0){
    return brk_ptr;
  } else {
    brk_ptr = addr;
    return addr;
  }
}

function malloc(size \
, old_malloc) {
  if(eq(NULL, _brk_ptr)) {
    _brk_ptr = brk(0);
    _malloc_ptr = _brk_ptr;
  }

  if(lt(_brk_ptr, add(_malloc_ptr, size))) {
    _brk_ptr = brk(add(_malloc_ptr, size));
    if(eq(_sub(0,1), _brk_ptr)) return 0;
  }

  old_malloc = _malloc_ptr;
  _malloc_ptr = add(_malloc_ptr, size);
  return old_malloc;
}

function memset(ptr, value, num \
, s) {
  s = ptr;
  print "memset ptr: " ptr " value: " value " num: " num;
  while(lt(0, num)) {
#    print "num: " num;
    wi8(s, value);
    s = add(s, 1);
    num = _sub(num, 1);
  }
}

function calloc(nmemb, size \
, ret) {
  print "calloc nmemb: " nmemb " size: " size;
  ret = malloc(mul(nmemb, size));
  if(eq(NULL, ret)) {
    return NULL;
  }
  memset(ret, 0, mul(nmemb, size));
  return ret;
}

function init_support(a,b,c){
  stdin = 0;
  stdout = 0;
  stderr = 2;
  init_or_tt();
  init_and_tt();
  init_mkc();
  brk_ptr = 128*1024;
}

function wi8(o,v){
  if(v==""){
    print("wrong use of wi8");
    exit 1;
  }
  heap[o] = and(v, 255);
}

function init_mkc( \
i, \
t) {
  print("init_mkc");
  for(i=0;i<256;i++){
    t=sprintf("%c",i);
    mkc_table[t]=i;
  }
  mkc_table["\'"]=39;
}

function mkc(a \
, c) {
  c=mkc_table[a];
  if(c==""){
    print("mkc char not defined: "a);
    exit 1;
  }
  print("mkc: " a " charcode: " c);
  return c;
}

function _sub(a,b){
  a = or(a, 0);
  b = or(b, 0);
  return a - b;
}

function neq(a,b,c){
  print "neq not impl"
  exit
}

function eq(a, b){
  a = or(a, 0);
  b = or(b, 0);
  return or((a == b), 0);
}

function to_int32(x){
  if(x>2147483647) {
    x=x-4294967296;
  }
  return x;
}

function do_bitwise(a, b, tt \
, ba \
, bb \
, i \
, r \
, t \
, v) {
  r=0;
#  print "a: " a;
#  print "b: " b;
  for(i=0;i<32;i++){
    r=r*0.5;
    ba=(a % 2);
    bb=(b % 2);
    t = ba+bb;
#    print "t: " t;
    v=tt[t];
#    print ba " " bb " " v;
    a=a-ba; a=a * 0.5;
    b=b-bb; b=b * 0.5;
    r=r+(v*2147483648);
  }
#  print "r: " r;
  return to_int32(r);
}

function to_uint32(x) {
#  print("to_uint32 x: " x);
  if((x<0)){
    if(x<-2147483648) {
      print("to_uint32 less than -2147483648 not supported");
      exit 1;
    }
    x=4294967296+x;
#    print("to_uint32 x: " or(x,0));
  }
  if(x>4294967295) {
    print "to_uint32 too big";
  }
  return x;
}

function init_or_tt(){
  or_tt[0]=0;
  or_tt[1]=1;
  or_tt[2]=1;
}

function or(a,b \
, r) {
  a=to_uint32(a);
  b=to_uint32(b);

  r=do_bitwise(a, b, or_tt);
#  print "r: " r;
  return r;
}

function init_and_tt(){
  and_tt[0]=0;
  and_tt[1]=0;
  and_tt[2]=1;
}

function and(a,b \
, r) {
  a=to_uint32(a);
  b=to_uint32(b);

  r=do_bitwise(a, b, and_tt);
#  print "r: " r;
  return r;
}

function ri8(o, dummy){
  if(dummy!=""){
    print("wrong use of ri8");
    exit 1;
  }
  return and(heap[o], 255);
}

function free(a,b,c){
  print "free not impl"
  exit
}

function lt(a,b) {
  a = or(a, 0);
  b = or(b, 0);
  return or((a < b), 0);
}

function gte(a,b,c){
  print "gte not impl"
  exit
}
function div(a,b,c){
  print "div not impl"
  exit
}
function fgetc(a,b,c){
  print "fgetc not impl"
  exit
}
function mod(a,b,c){
  print "mod not impl"
  exit
}
function shl(a,b,c){
  print "shl not impl"
  exit
}
function shr(a,b,c){
  print "shr not impl"
  exit
}
function gt(a,b,c){
  print "gt not impl"
  exit
}
