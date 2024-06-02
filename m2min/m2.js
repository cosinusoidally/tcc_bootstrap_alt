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
/*	return a & b; */
  err();
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

function ri8(o) {
/*
  char *h = 0;
  return h[o] & 0xFF;
*/
  err();
}

function wi8(o,v) {
/*
  char *h = 0;
  h[o]=v;
  return;
*/
  err();
}


main();
