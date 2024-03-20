var v_esp, v_ebp, v_stack_size, v_stack;
var heap_size=16*1024*1024;
var v_stack_size=256*1024;
var heap=new Array(heap_size/4);
for(var i=0;i<heap_size/4;i++){
  heap[i]=0;
};

var v_esp=heap_size-4;
var v_ebp=v_esp;

var malloc_base=4;

function malloc(x){
  var r=malloc_base;
// align to 4 bytes
  if(x!==((x>>>2)<<2)){
    x=4+(x>>>2) <<2;
  }
  malloc_base=malloc_base+x;
  if(malloc_base>(heap_size-v_stack_size)){
    throw "oom malloc";
  }
  return r;
}

function calloc(x){
  var r=malloc(x);
  for(var i=0;i<x;i++){
    wi8(r+x,0);
  }
  return r;
}

var string_cache={};

function mk_c_string(s){
  if(string_cache[s]){
    return string_cache[s];
  }
  var o=malloc(s.length+1);
  var a=s.split("");
  a=a.map(function(x){return x.charCodeAt(0)});
  a.push(0);
//  print("o "+o+" len "+a.length);
  for(var i=0;i<a.length;i++){
    wi8(o+i,a[i]);
  };
  string_cache[s]=o;
  return o;
}

function mk_js_string_len(o,l){
  var s=[];
  for(var i=0;i<l;i++){
    s.push(ri8(o+i));
  };
  return s.map(function(x){return String.fromCharCode(x)}).join("");
}

function mk_js_string(o){
  var s=[];
  var l=v_strlen(o);
  for(var i=0;i<l;i++){
    s.push(ri8(o+i));
  };
  return s.map(function(x){return String.fromCharCode(x)}).join("");
}

puts=print;

function printf(s) {
  print("printf: "+s);
}

function init_c(){
/* dummy */
}

function mk_char(c){
  return c.charCodeAt(0);
}

function mk_argc_argv(s){
  var argc;
  var argv;
  s=s.split(" ");
  argc=s.length;
  print(JSON.stringify(s));
  while(s.length>0){
    argv=v_alloca(4);
    wi32(argv,mk_c_string(s.pop()));
  }
  return {argc:argc,argv:argv};
}

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

function v_strcpy(dest,src){
  var c;
  while((c=ri8(src++))!==0){
    wi8(dest++,c);
  }
}

strcpy=v_strcpy;
