load("../tcc_js/sha256.js");

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

function calloc(s,x){
  if(s!=1){
    print("calloc can't handle sizes other than 1");
    exit(1);
  }
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
  var c, p;
  p = dest;
  while((c=ri8(src++))!==0){
    wi8(dest++,c);
  }
  return p;
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

function fgetc(file){
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

function v_fclose(f){
  delete f_files[f];
  return 0;
}

function v_strlen(s){
  var l=0;
  while(ri8(s++)){
    l=l+1;
  };
/*  print("strlen: "+l); */
  return l;
}

function isspace(c){
  if((c==mk_char(' ')) || (c==mk_char('\n')) || (c==mk_char('\t'))){
    return 1;
  } else {
    return 0;
  }
}

function isalnum(c){
  var r, t;
  c=c&0xFF;
  t = (c|32) - mk_char('a');
  r = ((t < 26 ) && (t >=0)) || isdigit(c);
/*  print("isalnum:"+c+" "+r+ " "+String.fromCharCode(c)); */
  return r;
}

function isdigit(c){
  var r;
  c=c&0xFF;
/*  print("isdigit:"+c); */
  r = c - mk_char('0');
  return (r < 10) && (r >= 0);
}

function strncmp (a, b, size) {
  if (size == 0)
    return 0;

  while (ri8(a) != 0 && ri8(b) != 0 && ri8(a) == ri8(b) && size > 1)
    {
      size = size - 1;
      a = a + 1;
      b = b + 1;
    }

  return ri8(a) - ri8(b);
}

function strlen(s){
  var l;
  l=0;
  while(ri8(s)){
    s=s+1;
    l=l+1;
  }
  return l;
}

function strstr(haystack, needle){
/*
  puts("strstr");
  puts("haystack: "+haystack);
  puts("needle:"+needle);
*/
  var lh, ln, o, r;
  o=0;
  if((haystack == 0) || (needle == 0)){
    return 0;
  }
  if(ri8(needle) == 0){
    return haystack;
  }
  if(ri8(haystack) == 0 ){
    return 0;
  }
  lh=strlen(haystack);
  ln=strlen(needle);
  if(ln > lh) {
    return 0;
  }
  while(o<lh) {
/*    puts("o: "+o); */
    r = strncmp(needle, haystack + o, ln);
/*    puts("r: "+r); */
    if( r == 0) {
      return haystack + o;
    }
    o = o + 1;
  }
  return 0;
}

function strtol(nptr, endptr, base){
  var t, v;
  print("strtol: "+nptr+" "+endptr+" "+base);
  print("the string:" +mk_js_string(nptr));
  if((endptr !=0) || (base !=0)){
    puts("strtol not supported endptr/base");
    exit(1);
  }
  /* FIXME this might not be quite correct */
  t=mk_js_string(nptr);
  v=parseInt(t);
  print("strtol: input="+t+" output="+v);
  return v;
}

function dummy(){
  debugger;
}

function memcpy(dest, src, c) {
  var i, p;
  p = dest;
  for(i=0;i<c;i=i+1){
    wi8(dest, ri8(src));
    dest = dest + 1;
    src = src + 1;
  }
  return p;
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

function check(s,compare_file) {
  f=vfs["otccelf_js_js.exe"];
  fp=malloc(f.length);
  for(var i=0;i<f.length;i++){
    wi8(i+fp,f[i]);
  }
  hd(fp,f.length);
  if(compare_file){
    print("comparing out.exe");
    var t=read("artifacts/otccelf_js3.exe","binary");
    for(var i=0;i<t.length;i++) {
      if(ri8(fp+i)!==t[i]){
        print("diff: "+to_hex(fp+i)+ " i: "+to_hex(i));
      }
    }
  }
print(to_hex(ind));
  if(s){
    var sha=root.sha256(vfs["otccelf_js_js.exe"]);
    print(sha);
    if(sha===s){
      print("OK");
    } else {
      print("BAD");
    }
  }
}


strcpy=v_strcpy;
fopen=v_fopen;
fwrite=v_fwrite;
fclose=v_fclose;
