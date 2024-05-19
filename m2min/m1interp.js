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


function to_hex(x){
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

print(to_hex(ri32(labels["FUNCTION_skip"].ho+1)));
print(to_hex(ri32(labels["WHILE_match_3"].ho+9)));

eip=labels["FUNCTION_token_list_layout_init"].ho;

function op_push_eax(){
    print("push_eax");
    eip = eip + 1;
}

function op_oparen(){
  var t;
  print("(");
  eip = eip + 1;
  t = ri32(eip) & 0xFFFFFF;
//    print("t "+ to_hex(t));
  if(t !== 0xE78955) {
    print("unsupported opcode");
    throw "op_oparen";
  }
  eip = eip + 3;
}

function run(){
  var t;
  while(1) {
  op=ri8(eip);
  print("op",op.toString(16));
  if(op=== 0x50){
    op_push_eax();
  } else if(op=== 0x57){
    op_oparen();
  } else if(op=== 0x89){
    print("do_call");
    eip = eip + 1;
    t = ri32(eip) & 0xFFFF;
//    print("t "+ to_hex(t));
    if(t !== 0xE8FD) {
      print("unsupported opcode");
      break;
    }
    eip = eip + 2;
    t = ri32(eip);
    print("t "+ to_hex(t));
    print("%" + rel_index[eip]);
    eip = eip + 4;
  } else if(op=== 0x5D){
    print(")");
    eip = eip + 1;
    t = ri8(eip);
//    print("t "+ to_hex(t));
    if(t !== 0x5F) {
      print("unsupported opcode");
      break;
    }
    eip = eip + 1;
  } else if(op=== 0xB8){
    print("mov_eax,");
    eip = eip + 1;
    t = ri32(eip);
    name = abs_index[eip];
    if(name) {
      print("&" + name);
    } else {
      print("%" + t + " # assuming constant");
    }
    eip = eip + 4;
  } else if(op=== 0x81){
    print("add_esp,");
    eip = eip + 1;
    t = ri8(eip);
    if(t !== 0xC4){
      print("unsupported opcode");
      break;
    }
    eip = eip + 1;
    t = ri32(eip);
    print("%" + t);
    eip = eip + 4;
  } else if(op=== 0xB8){
    eip = eip + 1;
    t = ri32(eip);
    print("&" + abs_index[eip]);
    eip = eip + 4;
  } else if(op=== 0x8B){
    print("load");
    eip = eip + 1;
    t = ri8(eip);
    if(t !== 0){
      print("unsupported opcode");
      break;
    }
    eip = eip + 1;
  } else if(op=== 0x5B){
    print("store");
    eip = eip + 1;
    t = ri32(eip) & 0xFFFF;
    if(t !== 0x0389) {
      print("unsupported opcode");
      break;
    }
    eip = eip + 2;
  } else if(op=== 0x8D){
    print("local");
    eip = eip + 1;
    t = ri8(eip);
    if(t !== 0x85) {
      print("unsupported opcode");
      break;
    }
    eip = eip + 1;
    t = ri32(eip);
    print("local: " + t);
    eip = eip + 4;
  } else if(op=== 0xC3){
    print("ret");
    eip = eip + 1;
  } else if(op=== 0x85){
    print("jump_false");
    eip = eip + 1;
    t = ri32(eip) & 0xFFFFFF;
    if(t !== 0x840FC0) {
      print("unsupported opcode");
      break;
    }
    eip = eip + 3;
    t = ri32(eip);
    print("%" + rel_index[eip]);
    eip = eip + 4;
  } else if(op=== 0xE9){
    print("jump");
    eip = eip + 1;
    t = ri32(eip);
    print("%" + rel_index[eip]);
    eip = eip + 4;
  } else if(op=== 0xCC){
    print("int_03");
    t = md[eip].function;
    while(t === md[eip].function) {
      eip = eip + 1;
    }
    print(md[eip].function);
  } else {
    print("unsupported opcode");
    break;
  }
  }
}

int_03 = 0xCC;

prims = ["add", "sub", "mul", "shl", "shr", "lt", "gt", "lte", "gte", "and",
         "or", "ri8", "eq", "neq", "xor", "wi8", "div", "mod", "fgetc", "fputc",
         "open", "close", "brk", "exit"];

prims.forEach(function(x){
  wi8(labels["FUNCTION_"+x].ho, int_03);
});

run();

print(md[eip].function);
