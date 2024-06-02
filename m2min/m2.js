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
  return (a << b) | 0;
}

function shr(a, b){
  a = a | 0;
  b = b | 0;
  return a >>> b;
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
  return a | b;
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
  print("open name:" +mk_js_string(name) + " flag: "+flag+" mode: "+mode);
  err();
}

function close(fd) {
  err();
}

var brk_ptr = brk_ptr=128*1024;

function brk(addr) {
  addr = addr |0;
  if(addr===0){
    return brk_ptr;
  } else {
    return addr;
  }
}

function exit(value) {
  err();
}

function mkc(c) {
  return c.charCodeAt(0) & 0xFF;
}

function mk_c_string(s){
  var r;
  r = malloc(s.length + 1);
  for(var i =0; i<s.length; i++){
    wi8(r + i, s.charCodeAt(i));
  }
  return r;
}

function mk_js_string(o){
  var c;
  var s = [];
  var i=0;
  while((c=ri8(o+i)) !==0){
    s.push(String.fromCharCode(c));
    i=i+1;
  }
  return s.join("");
}

mks = mk_c_string;


function mk_args(s){
  var argc;
  s=s.split(" ");
  argc = s.length;
  argv = malloc(argc * 4);
  for(var i = 0; i < argc ; i++){
    wi32(argv+(4*i), mk_c_string(s[i]));
  }
  return [argc, argv];
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
  argc_argv = mk_args("./artifacts/M2_simple_asm_m2.exe artifacts/M2_simple_asm_m2.c artifacts/out.M1")

  argv = argc_argv[1];
  argc = argc_argv[0];
  main(argc, argv);
} catch (e){
  print(e.stack);
  print(e.message);
}
