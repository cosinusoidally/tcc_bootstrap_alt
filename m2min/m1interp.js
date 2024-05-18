print("hello world");
a=read("artifacts/M2_simple_asm-0.hex2");

a=a.split("\n");

labels={};

// patch points for relative offsets
relp = [];

// patch points for absolute addresses
absp = [];

hex_frags = [];

var heap_size=16*1024*1024;
var v_stack_size=256*1024;
var heap=new Array(heap_size/4);

for(var i=0;i<heap_size/4;i++){
  heap[i]=0;
};


// heap offset
ho = 0;

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
    ho=ho+1;
  }
}

for(var i=0;i<a.length;i++){
  var l;
  var l0;
  l=a[i];
  l0=l[0];
  if(l0===":"){
    labels[l.split(":")[1]] = {line: i, ho: ho};
  } else if(l0==="%"){
    relp.push({name: l.split("%")[1], line: i, ho: ho});
    append_hex("DEADBEEF");
  } else if(l0==="&"){
    absp.push({name: l.split("&")[1], line: i, ho: ho});
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

eip=labels["FUNCTION_main"].ho;

function run(){
  var t;
  while(1) {
  op=ri8(eip);
  print("op",op.toString(16));
  if(op=== 0x50){
    print("push_eax");
    eip = eip + 1;
  } else if(op=== 0x57){
    print("(");
    eip = eip + 1;
    t = ri32(eip) >>> 8;
    if(t !== 0x89e789) {
      print("unsupported opcode");
     break;
    }
    print("t "+ to_hex(t));
    eip = eip + 3;
  } else {
    print("unsupported opcode");
    break;
  }
  }
}


run();
