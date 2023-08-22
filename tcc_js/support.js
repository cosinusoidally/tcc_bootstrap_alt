print("loading support code");

var heap_size=16*1024*1024;
var stack_size=256*1024;
var heap=new Array(heap_size/4);
for(var i=0;i<heap_size/4;i++){
  heap[i]=0;
};

var esp=heap_size-4;
var ebp=esp;

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
print(to_hex(heap_size));
print(to_hex(esp));

function puts(x){
  print(x);
}

function err(){
  backtrace();
  throw "error not impl";
}

function alloca(x){
  esp=esp-x;
  print("alloca "+x+" at:"+to_hex(esp));
  return esp;
}

var malloc_base=4;

function malloc(x){
  var r=malloc_base;
  malloc_base=malloc_base+x;
  if(malloc_base>(heap_size-stack_size)){
    throw "oom malloc";
  }
  return r;
}

function wi8(o,v){
  var o1=o>>>2;
  var s=o&3;
  var v1=heap[o1];
  v1=v1&(~(0xff<<(o*8))) | ((v&0xff)<<(o*8));
  heap[o1]=v1;
};

function wi32(o,v){
  err();
};

heap[0]=0xdeadbeef;
print("blah "+to_hex(heap[0]));
wi8(0,1);
print("blah "+to_hex(heap[0]));
wi8(1,1);
print("blah "+to_hex(heap[0]));
wi8(2,1);
print("blah "+to_hex(heap[0]));
wi8(3,1);
print("blah "+to_hex(heap[0]));
