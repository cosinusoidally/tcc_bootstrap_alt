var dbg;

out=[];
out2=[];

print_old=print;

print=function(x){
  if(dbg) {
    out.push(x);
    if(out.length>100){
      out2=out;
      out=[];
    }
  }
}

print("hello world");

a=read("artifacts/M2_simple_asm-0.hex2");

a=a.split("\n");

labels={};

// patch points for relative offsets
relp = [];

rel_index=[];

// patch points for absolute addresses
absp = [];

abs_index = [];

hex_frags = [];

var heap_size=16*1024*1024;
var v_stack_size=256*1024;
var heap=new Array(heap_size/4);

var esp;
esp=heap_size-4;

var ebp = 0;
var edi = 0;
var eax = 0;
var ebx = 0;

var exec;

for(var i=0;i<heap_size/4;i++){
  heap[i]=0;
};

current_function = "*NOT_FUNCTION*";

// heap offset
ho = 0;

// heap metadata
md = [];

function wi8(o,v){
  if(v===undefined){
    print("wrong use of wi8");
    err();
  }
  var o1=o>>>2;
  var s=o&3;
  var v1=heap[o1];
  v1=v1&(~(0xff<<(s*8))) | ((v&0xff)<<(s*8));
  heap[o1]=v1;
};

function ri8(o,dummy){
  if(dummy!==undefined){
    print("wrong use of ri8");
    err();
  }
  var o1=o>>>2;
  var s=o&3;
  var v1=heap[o1];
  return (v1>>>(s*8)) &0xff;
};

function wi32(o, v) {
  wi8(o,v&0xFF);
  v=v>>8;
  wi8(o+1,v&0xFF);
  v=v>>8;
  wi8(o+2,v&0xFF);
  v=v>>8;
  wi8(o+3,v&0xFF);
}

function ri32(o) {
  return (ri8(o)&255)       | (ri8(o+1)&255)<<8 |
         (ri8(o+2)&255)<<16 | (ri8(o+3)&255)<<24;
}


to_hex = function(x){
  var y;
  var a=[];
  while(x){
    y=x&0xff;
    x=x>>>8;
    a.push(y);
  }
  var b=[];
  while(a.length>0){
    b.push(a.pop());
  }
  return "0x"+("00000000"+(b.map(function(z){return ("0000"+z.toString(16)).slice(-2)}).join(""))).slice(-8);
}

function append_hex(s) {
  var v;
  var m;
  print(s);
  s=s.split("");
  for(var i=0; i<s.length; i=i+2){
    v=[];
    v.push("0x");
    v.push(s[i]);
    v.push(s[i+1]);
    v=v.join("");
    print(v);
    wi8(ho,parseInt(v,16));
    m = {};
    m.function = current_function;
    m.line = current_line;
    md[ho] = m;

    ho=ho+1;
  }
}

for(var i=0;i<a.length;i++){
  var l;
  var l0;
  var name;
  l=a[i];
  l0=l[0];
  current_line = i;
  if(l0===":"){
    name = l.split(":")[1];
    metadata = {name: name, line: i, ho: ho};
    labels[name] = metadata;
    if(name.split("_")[0] === "FUNCTION") {
      current_function = name.split("FUNCTION_")[1];
    }
    if(name.split("_")[0] === "GLOBAL") {
      current_function = "*NON_FUNCTION: "+name;
    }
    if(name.split("_")[0] === "STRING") {
      current_function = "*NON_FUNCTION: "+name;
    }
  } else if(l0==="%"){
    name = l.split("%")[1];
    relp.push({name: name, line: i, ho: ho});
    rel_index[ho]=name;
    append_hex("DEADBEEF");
  } else if(l0==="&"){
    name = l.split("&")[1];
    absp.push({name: name, line: i, ho: ho});
    abs_index[ho]=name;
    append_hex("DEADBEEF");
  } else {
    if(l.length >0) {
      hex_frags.push({name: l, line: i, ho: ho});
      append_hex(l);
    }
  }
}

function link(x) {
  var c;
  var lo;
  for(var i=0;i<x.length;i++){
    c = x[i];
    lo = labels[c.name].ho;
    wi32(c.ho, lo);
  }
}

link(absp);

// hack we don't actually need relative offsets to intepret
link(relp);

eip=labels["FUNCTION_token_list_layout_init"].ho;

function push(r){
  esp = esp - 4;
  wi32(esp, r);
}

function pop(){
  var r;
  r = ri32(esp);
  esp = esp + 4;
  return r;
}

function op_push_eax(){
  print("push_eax");
  if(exec){
    push(eax);
  }
  eip = eip + 1;
}

function op_oparen(){
  var t;
  print("(");
  t = ri32(eip+1) & 0xFFFFFF;
//    print("t "+ to_hex(t));
  if(t !== 0xE78955) {
    print("unsupported opcode");
    throw "op_oparen";
  }
  if(exec) {
    push(edi);
    push(ebp);
    edi = esp;
  }
  eip = eip + 4;
}

function op_do_call(){
  var t;
  print("do_call");
  eip = eip;
  t = ri32(eip + 1) & 0xFFFF;
//    print("t "+ to_hex(t));
  if(t !== 0xE8FD) {
    print("unsupported opcode");
    throw "op_do_call";
  }
  t = ri32(eip + 3);
  if(dbg) {
    print("t "+ to_hex(t));
    print("%" + rel_index[eip+3]);
  }
  if(exec) {
    ebp = edi;
    push(eip + 7);
    // HACK we are using absolute addresses
    eip = t;
  } else {
    eip = eip + 7;
  }
}

function op_cparen(){
  var t;
  print(")");
  t = ri8(eip + 1);
//    print("t "+ to_hex(t));
  if(t !== 0x5F) {
    print("unsupported opcode");
    throw "op_cparen";
  }
  if(exec) {
    ebp = pop();
    edi = pop();
  }
  eip = eip + 2;
}

function op_mov_eax(){
  var t;
  print("mov_eax,");
  t = ri32(eip + 1);
  if(dbg) {
    name = abs_index[eip + 1];
    if(name) {
      print("&" + name);
    } else {
      print("%" + t + " # assuming constant");
    }
  }
  if(exec) {
    eax = t;
  }
  eip = eip + 5;
}

function op_add_esp(){
  var t;
  print("add_esp,");
  t = ri8(eip + 1);
  if(t !== 0xC4){
    print("unsupported opcode");
    throw "op_add_esp";
  }
  t = ri32(eip + 2);
  if(dbg) {
    print("%" + t);
  }
  if(exec) {
    esp = esp + t
  }
  eip = eip + 6;
}

function op_load(){
  var t;
  print("load");
  t = ri8(eip + 1);
  if(t !== 0){
    print("unsupported opcode");
    throw "op_load";
  }
  if(exec) {
    eax = ri32(eax);
  }
  eip = eip + 2;
}

function op_store(){
  var t;
  print("store");
  t = ri32(eip + 1) & 0xFFFF;
  if(t !== 0x0389) {
    print("unsupported opcode");
    throw "op_store";
  }
  if(exec) {
    ebx = pop();
    wi32(ebx, eax);
  }
  eip = eip + 3;
}

function op_local(){
  var t;
  print("local");
  t = ri8(eip + 1);
  if(t !== 0x85) {
    print("unsupported opcode");
    throw "op_local";
  }
  t = ri32(eip + 2);
  if(dbg){
    print("local: " + t);
  }
  if(exec) {
    eax = ebp + t;
  }
  eip = eip + 6;
}

function op_ret(){
  var t;
  print("ret");
  if(exec) {
    t = pop();
    if(dbg) {
      print("return address:" + t);
    }
    eip = t;
  } else {
    eip = eip + 1;
  }
}

function op_jump_false(){
  var t;
  print("jump_false");
  t = ri32(eip + 1) & 0xFFFFFF;
  if(t !== 0x840FC0) {
    print("unsupported opcode");
    throw "op_jump_false";
  }
  t = ri32(eip + 4);
  if(dbg) {
    print("%" + rel_index[eip]);
  }
  if(exec) {
    if(eax & eax) {
      eip = eip + 8;
    } else {
      eip = t;
    }
  } else {
    eip = eip + 8;
  }
}

function op_jump(){
  var t;
  print("jump");
  t = ri32(eip + 1);
  if(dbg) {
    print("%" + rel_index[eip]);
  }
  if(exec) {
    eip = t;
  } else {
    eip = eip + 5;
  }
}

function op_int_03(){
  var t;
  if(dbg) {
    t = md[eip].function;
    print("int_03 : " + t);
  }
  if(exec) {
    prims_index[ri8(eip+1)]();
  } else {
    while(t === md[eip].function) {
      eip = eip + 1;
    }
    if(dbg) {
      print(md[eip].function);
    }
  }
}

function dump_stack(){
  print("stack:");
  for(var i=esp;i< heap_size;i=i+4){
    print(to_hex(ri32(i)));
  }
  print("end stack");
}

function run(){
  var t;
  while(1) {
  op=ri8(eip);
  if(dbg) {
    print("op",op.toString(16));
    print("line: " + md[eip].line);
  }
  if(exec){
//    dump_stack();
  }
  if(op=== 0x50){
    op_push_eax();
  } else if(op=== 0x57){
    op_oparen();
  } else if(op=== 0x89){
    op_do_call();
  } else if(op=== 0x5D){
    op_cparen();
  } else if(op=== 0xB8){
    op_mov_eax();
  } else if(op=== 0x81){
    op_add_esp();
  } else if(op=== 0x8B){
    op_load();
  } else if(op=== 0x5B){
    op_store();
  } else if(op=== 0x8D){
    op_local();
  } else if(op=== 0xC3){
    op_ret();
  } else if(op=== 0x85){
    op_jump_false();
  } else if(op=== 0xE9){
    op_jump();
  } else if(op=== 0xCC){
    op_int_03();
  } else {
    print("unsupported opcode");
    throw "unsupported opcode";
  }
  }
}

int_03 = 0xCC;

prims = ["add", "sub", "mul", "shl", "shr", "lt", "gt", "lte", "gte", "and",
         "or", "ri8", "eq", "neq", "xor", "wi8", "div", "mod", "fgetc", "fputc",
         "open", "close", "brk", "exit"];

prim_ops={};

function stub(x){
  return function(){
    print(x + " not impl");
    throw "error";
  }
}

prims.forEach(function(x,i){
  var o=labels["FUNCTION_"+x].ho;
  wi8(o, int_03);
  wi8(o+1, i);
  prim_ops[x] = stub(x);
});

function get_arg(x){
  return ri32(ebp-4*(x + 1));
}

prim_ops["add"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("add(" +a+", "+b+")");
  }
  eax = a + b;
  op_ret();
}

prim_ops["sub"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("sub(" +a+", "+b+")");
  }
  eax = a - b;
  op_ret();
}

prim_ops["mul"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("mul(" +a+", "+b+")");
  }
  eax = a * b;
  op_ret();
}

prim_ops["eq"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("eq(" +a+", "+b+")");
  }
  eax = (a === b) | 0;
  op_ret();
}

prim_ops["lt"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("lt(" +a+", "+b+")");
  }
  eax = (a < b) | 0;
  op_ret();
}

prim_ops["brk"]=function(){
  var a = get_arg(0);

  if(dbg) {
    print("brk("+a+")");
  }
  if(a===0){
    eax = brk_ptr;
  } else {
    eax = a;
  }
  op_ret();
}

prim_ops["wi8"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("wi8(" +a+", "+b+")");
  }
  wi8(a, b);
  op_ret();
}

prim_ops["ri8"]=function(){
  var a = get_arg(0);

  if(dbg) {
    print("ri8(" +a+")");
  }
  eax = ri8(a);
  op_ret();
}

prim_ops["and"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("and(" +a+", "+b+")");
  }
  eax = a & b;
  op_ret();
}

prim_ops["shl"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("shl(" +a+", "+b+")");
  }
  eax = a << b;

  op_ret();
}

prim_ops["or"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("or(" +a+", "+b+")");
  }
  eax = a | b;
  op_ret();
}

var in_file;
in_file_num = 5;

var out_file;
out_file_num = 6;

load("prims.js");

if(dbg) {
  try {
    run();
  } catch (e){
    print("error");
  }

  print(md[eip].function);
}

print("starting");
eip = labels["FUNCTION_main"].ho;
exec = true;

brk_ptr=128*1024;

function malloc(n){
  var r;
  r = brk_ptr;
  brk_ptr = brk_ptr + n;
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

function mk_c_string(s){
  var r;
  r = malloc(s.length + 1);
  for(var i =0; i<s.length; i++){
    wi8(r + i, s.charCodeAt(i));
  }
  return r;
}

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

argc_argv = mk_args("./artifacts/M2_simple_asm_m2.exe artifacts/M2_simple_asm_m2.c artifacts/out.M1")

argv = argc_argv[1];
argc = argc_argv[0];

ebp = esp;
push(argc);
push(argv);
push(0); // envp (unused)
push(labels["FUNCTION_exit"].ho);

eax = 0x1234567; // deliberate garbage


// re-load prims and re-populate prims_index
lp();

try {
  run();
} catch (e){
  print("error");
  print(e.stack);
}

// dl();

// pop of the last newline in the file
if(out_file[out_file.length-1]===0xa){
  out_file.pop();
}

file=out_file.map(function(x){
  return String.fromCharCode(x);
}).join("");

print_old(file);
