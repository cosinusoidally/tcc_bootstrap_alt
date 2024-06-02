load("simple_support_js_m2.js");
load("M2_simple_asm.js");

function add(a, b){
  a = a | 0;
  b = b | 0;
  return a + b;
}

function sub(a, b){
  a = a | 0;
  b = b | 0;
  return a - b;
}

function mul(a, b){
  a = a | 0;
  b = b | 0;
  return a * b;
}

function shl(a, b){
  a = a | 0;
  b = b | 0;
/*	return a << b; */
  err();
}

function shr(a, b){
  a = a | 0;
  b = b | 0;
/*	return a >> b; */
  err();
}

function lt(a, b){
  a = a | 0;
  b = b | 0;
  return (a < b) | 0;
}

function gt(a, b){
  a = a | 0;
  b = b | 0;
/*	return a > b; */
  err();
}

function lte(a, b){
  a = a | 0;
  b = b | 0;
/*	return a <= b; */
  err();
}

function gte(a, b){
  a = a | 0;
  b = b | 0;
/*	return a >= b; */
  err();
}

function and(a, b){
  a = a | 0;
  b = b | 0;
  return a & b;
}

function or(a, b){
  a = a | 0;
  b = b | 0;
/*	return a | b; */
  err();
}

function eq(a, b){
  a = a | 0;
  b = b | 0;
/*	return a == b; */
  return (a == b) | 0;
  err();
}

function neq(a, b){
  a = a | 0;
  b = b | 0;
/*	return a != b; */
  err();
}

function xor(a, b){
  a = a | 0;
  b = b | 0;
/*	return a ^ b; */
  err();
}

function div(a, b){
  a = a | 0;
  b = b | 0;
/*	return a / b; */
  err();
}

function mod(a, b){
  a = a | 0;
  b = b | 0;
/*	return a % b; */
  err();
}

function fgetc(f) {
  err();
}

function fputc(s, f) {
  err();
}

function open(name, flag, mode) {
  err();
}

function close(fd) {
  err();
}

function brk(addr) {
}

function exit(value) {
}

function mkc(c) {
  return c.charCodeAt(0) & 0xFF;
}

var heap_size=16*1024*1024;
var heap=new Uint8Array(heap_size);

function wi8(o,v){
  if(v===undefined){
    print("wrong use of wi8");
    err();
  }
  heap[o]=v & 0xFF;
};

function ri8(o,dummy){
  if(dummy!==undefined){
    print("wrong use of ri8");
    err();
  }
  return heap[o] & 0xFF;
};


try {
  main();
} catch (e){
  print(e.stack);
}
