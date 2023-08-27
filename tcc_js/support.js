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
//  leave();
//  hd(0,512);
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

function realloc(x,size){
print("realloc x:"+x+" size:"+size);
  var r=malloc(size);
  for(var i=0;i<size;i++){
    wi8(r+i,ri8(x+i));
  };
  return r;
}

function wi8(o,v){
  var o1=o>>>2;
  var s=o&3;
  var v1=heap[o1];
  v1=v1&(~(0xff<<(s*8))) | ((v&0xff)<<(s*8));
  heap[o1]=v1;
};

function wi32(o,v){
  for(var i=0;i<4;i++){
    wi8(o+i,v);
    v=v>>>8;
  }
};

function ri8(o){
  var o1=o>>>2;
  var s=o&3;
  var v1=heap[o1];
  return (v1>>>(s*8)) &0xff;
};

function ri32(o){
  var r=0;
  for(var i=0;i<4;i++){
     r=r|(ri8(o+i)<<(i*8));
  };
  return r;
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

heap[0]=0xdeadbeef;
heap[1]=0xdeadbeef;

wi32(2,0);

print("blah2 "+to_hex(ri32(0))+" "+to_hex(ri32(4)));

heap[0]=0x0;
heap[1]=0x0;

function to_printable(x){
  if(32 <=x  && x <=126){
    return String.fromCharCode(x);
  };
  return ".";
};

function hd(o,n){
  var d=[];
  var l=o+n;
  while(o<l){
    d.push(to_hex(o));
    d.push(": ");
    var r=[];
    for(var i=0;i<16;i++){
      r.push(ri8(o+i));
    };
    for(var i=0;i<r.length;i=i+2){
      d.push(("0000"+(r[i].toString(16))).slice(-2));
      d.push(("0000"+(r[i+1].toString(16))).slice(-2));
      d.push(" ");
    };
    o=o+16;
    d.push("  ");
    d.push(r.map(to_printable).join(""));
    d.push("\n");
  };
  print(d.join(""));
}
hd(0,16);

function mk_c_string(s){
  var o=malloc(s.length+1);
  var a=s.split("");
  a=a.map(function(x){return x.charCodeAt(0)});
  a.push(0);
//  print("o "+o+" len "+a.length);
  for(var i=0;i<a.length;i++){
    wi8(o+i,a[i]);
  };
  return o;
}

function mk_js_string_len(o,l){
  var s=[];
  for(var i=0;i<l;i++){
    s.push(ri8(o+i));
  };
  return s.map(function(x){return String.fromCharCode(x)}).join("");
}

function enter(){
// backtrace();
  print("pre-enter esp: "+to_hex(esp)+" ebp: "+to_hex(ebp));
  esp=esp-4;
  wi32(esp,ebp);
  ebp=esp;
  print("post-enter esp: "+to_hex(esp)+" ebp: "+to_hex(ebp));
}

function leave(x){
// backtrace();
  esp=ebp;
  ebp=ri32(esp);
  esp=esp+4;
  print("leave esp: "+to_hex(esp)+" ebp: "+to_hex(ebp));
  return x;
}

function memcpy(dest,src,count){
  for(var i=0;i<count;i++){
    wi8(dest,ri8(src));
  }
}

function strlen(s){
  var l=0;
  while(ri8(s++)){
    l=l+1;
  };
  print("strlen: "+l);
  return l;
}

function unsigned(x){
  return x>>>0;
}

function memset(x,v,size){
  for(var i=0;i<size;i++){
    ri8(x+i,v);
  }
}

function mk_argc_argv(s){
  var argc;
  var argv;
  print("error");
  s=s.split(" ");
  argc=s.length;
  print(JSON.stringify(s));
  while(s.length>0){
    argv=alloca(4);
    wi32(argv,mk_c_string(s.pop()));
  }
  return {argc:argc,argv:argv};
}

function mk_char(c){
  return c.charCodeAt(0);
}
