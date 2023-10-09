print("loading support code");

var NULL=0;

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
  hd(prog,ind-prog);
  backtrace();
  throw "error not impl";
}

function v_alloca(x){
  esp=esp-x;
  print("alloca "+x+" at:"+to_hex(esp));
  return esp;
}

var malloc_base=4;

function malloc(x){
  var r=malloc_base;
// align to 4 bytes
  if(x!==((x>>>2)<<2)){
    x=4+(x>>>2) <<2;
  }
  malloc_base=malloc_base+x;
  if(malloc_base>(heap_size-stack_size)){
    throw "oom malloc";
  }
  return r;
}

function v_realloc(x,size){
print("realloc x:"+x+" size:"+size);
  var r=malloc(size);
  for(var i=0;i<size;i++){
    wi8(r+i,ri8(x+i));
  };
  return r;
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

function wi32(o,v){
  for(var i=0;i<4;i++){
    wi8(o+i,v);
    v=v>>>8;
  }
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

function ri32(o,dummy){
  var r=0;
  for(var i=0;i<4;i++){
     r=r|(ri8(o+i,dummy)<<(i*8));
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

function v_memcpy(dest,src,count){
  for(var i=0;i<count;i++){
    wi8(dest+i,ri8(src+i));
  }
}

function v_strlen(s){
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

function v_memset(x,v,size){
  for(var i=0;i<size;i++){
    wi8(x+i,v);
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
    argv=v_alloca(4);
    wi32(argv,mk_c_string(s.pop()));
  }
  return {argc:argc,argv:argv};
}

function mk_char(c){
  return c.charCodeAt(0);
}

f_files={};
vfs={};

function v_fopen(f,mode){
// FIXME ljw non-dummy impl
  var filename=mk_js_string(f);
  mode=mk_js_string(mode);
  print("fopen: filename: "+filename+" mode: "+mode);
// FIXME hack hack
  var file_num=malloc(4);
  file_o={};
  file_o.filename=filename;
  file_o.mode=mode;
  file_o.o=0;
  f_files[file_num]=file_o;
  if(mode==="r"){
    try {
      file_o.data=read(filename,"binary");
    } catch (e){
      v_fclose(file_num);
      return 0;
    }
  } else if(mode==="wb"){
    file_o.data=[];
    vfs[filename]=file_o.data;
  } else {
    print("unsupported fopen file mode");
err();
  }
  return file_num;
}

function v_fclose(f){
  delete f_files[f];
  return 0;
}

function v_getc_unlocked(file){
  file_o=f_files[file];
//  print(JSON.stringify(file_o));
  var c=file_o.data[file_o.o++];
  // eof
  if(c===undefined){
    return -1;
  }
  return c;
}

function v_fwrite(ptr, size, nmemb, stream){
  var f=f_files[stream];
  for(var i=0;i<size*nmemb;i++){
    f.data.push(ri8(ptr++)&0xFF);
  }
}

function v_memcmp(s1,s2,n){
//print("memcmp "+to_hex(s1)+" "+to_hex(s2)+" "+n);
  var r=0;
  for(var i=0;i<n;i++){
    if(ri8(s1+i)!==ri8(s2+i)){
// FIXME ljw not quite right
      r=1;
    }
  }
  return r;
}

function v_free(x){
  return 0;
}

function check(s,compare_file) {
  f=vfs["tcc_boot.o"];
  fp=malloc(f.length);
  for(var i=0;i<f.length;i++){
    wi8(i+fp,f[i]);
  }
  hd(fp,f.length);
  if(compare_file){
    print("comparing tcc_boot.o");
    var t=read("tcc_boot.o","binary");
    for(var i=0;i<t.length;i++) {
      if(ri8(fp+i)!==t[i]){
        print("diff: "+to_hex(fp+i)+ " i: "+to_hex(i));
      }
    }
  }
print(to_hex(ind));
  if(s){
    var sha=root.sha256(vfs["tcc_boot.o"]);
    print(sha);
    if(sha===s){
      print("OK");
    } else {
      print("BAD");
    }
  }
}

function v_strcpy(dest,src){
  var c;
  while((c=ri8(src++))!==0){
    wi8(dest++,c);
  }
}

function v_strrchr(s,c){
  var c1;
  while(c1=ri8(s++)){
    if(c1===c){
      return s-1;
    }
  }
  return 0;
}

function v_strcat(dest,src){
  var d=dest;
  var c;
  while(ri8(dest++));
  dest--;
  while(c=ri8(src++)){
    wi8(dest++,c);
  }
  wi8(dest,0);
  return d;
}

function v_strdup(src){
  var l=v_strlen(src);
  var dest=malloc(l+1);
  for(var i=0;i<l;i++){
    wi8(dest+i, ri8(src+i));
  }
  wi8(dest+l, 0);
  return dest;
}

load("sha256.js");

function decode_Sym(sym){
  s={};
  s.v=ri32(sym+Sym_v_o);
  s.t=ri32(sym+Sym_t_o);
  s.c=ri32(sym+Sym_c_o);
  s.next=ri32(sym+Sym_next_o);
  s.prev=ri32(sym+Sym_prev_o);
  s.hash_next=ri32(sym+Sym_hash_next_o);
  return s;
}

function urs(v,n){
// refactoring unsigned right shift into a function
//  return v >>> n;

// refactoring into a form that doesn't rely on non-sign extending right
// shifts since the mechanical C translation will only have signed int types.

  if(n===0) {
    return v;
  }
  var m=0x80000000;
  var m2=0x40000000;
  var t=v&m;
  v=v & ~m;
  v=v >> n;
  if(t){
    v=v | (m2 >> (n-1));
  }
  return v;
}

function init_globals(){
// dummy function in JS. This function needs to exist for the C code generated
// by js_to_c.js
}

function init_runtime(){
// dummy function in JS. This function needs to exist for the C code generated
}

// need wrapper function for main since tcc_main is the real function name
function main(argc,argv){
    return tcc_main(argc, argv);
}

// need this alias as when we convert to C we can't use malloc as a function
// name
v_malloc=malloc;
strlen=v_strlen;

function error(msg){
  print("error");
  print(msg);
  err();
}

function warning(msg){
  print("warning");
  print(msg);
  err();
}

function expect(msg){
  print("expect");
  print(msg);
  err();
}
