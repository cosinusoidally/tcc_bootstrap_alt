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
function add(a,b,c){
  print "add not impl"
  exit
}

function mul(a,b,c){
  a=or(a,0);
  b=or(b,0);
  return a*b;
}

function calloc(nmemb, size){
  print "calloc nmemb: " nmemb " size: " size;
  print "calloc not impl";
  exit
}

function init_support(a,b,c){
  stdin = 0;
  stdout = 0;
  stderr = 2;
  init_or_tt();
}

function wi8(a,b,c){
  print "wi8 not impl"
  exit
}
function mkc(a,b,c){
  print "mkc not impl"
  exit
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

function eq(a,b,c){
  print "eq not impl"
  exit
}

function do_bitwise(a, b, tt \
, ba \
, bb \
, i \
, r \
, v) {
  r=0;
#  print "a: " a;
#  print "b: " b;
  for(i=0;i<32;i++){
    r=r*0.5;
    ba=(a % 2);
    bb=(b % 2);
    v=tt[ba+bb];
#    print ba " " bb " " v;
    a=a-ba; a=a * 0.5;
    b=b-bb; b=b * 0.5;
    r=r+(v*2147483648);
  }
#  print "r: " r;
  return r;
}

function to_uint32(x) {
  if((x<0)){
    print "to_uint32 negative not supported";
    exit 1;
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
  or_tt[3]=1;
}

function or(a,b \
, r) {
  a=to_uint32(a);
  b=to_uint32(b);

  r=do_bitwise(a, b, or_tt);
#  print "r: " r;
  return r;
}

function and(a,b,c){
  print "and not impl"
  exit
}
function ri8(a,b,c){
  print "ri8 not impl"
  exit
}
function free(a,b,c){
  print "free not impl"
  exit
}
function lt(a,b,c){
  print "fgetc not impl"
  exit
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
