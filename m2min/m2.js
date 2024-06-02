load("simple_support_js_m2.js");
load("M2_simple_asm.js");

function add(a, b){
/*	return a + b; */
  err();
}

function sub(a, b){
  a = a | 0;
  b = b | 0;
/*	return a - b; */
  err();
}

function mul(a, b){
/*	return a * b; */
  err();
}

function shl(a, b){
/*	return a << b; */
  err();
}

function shr(a, b){
/*	return a >> b; */
  err();
}

function lt(a, b){
/*	return a < b; */
  err();
}

function gt(a, b){
/*	return a > b; */
  err();
}

function lte(a, b){
/*	return a <= b; */
  err();
}

function gte(a, b){
/*	return a >= b; */
  err();
}

function and(a, b){
/*	return a & b; */
  err();
}

function or(a, b){
/*	return a | b; */
  err();
}

function ri8(o) {
/*
  char *h = 0;
  return h[o] & 0xFF;
*/
  err();
}

function eq(a, b){
/*	return a == b; */
  err();
}

function neq(a, b){
/*	return a != b; */
  err();
}

function xor(a, b){
/*	return a ^ b; */
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

function div(a, b){
/*	return a / b; */
  err();
}

function mod(a, b){
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

main();
