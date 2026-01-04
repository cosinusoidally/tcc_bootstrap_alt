var f_files;
var vfs;
var v_malloc;
var strlen;
var print_orig;
var to_hex_orig;
var argc_argv;
var s;
var file_o;
var f;
var fp;

var read_ = read;

read=function(x,y){
  if(arguments.length>1){
    if(y==="binary"){
      var b=read_(x);
      b=b.split("");
      b=b.map(function(x) {return x.charCodeAt(0);});
      return b;
    };
  };
  return read_(x);
};

var load_ = load;

load = function(x) {
  if(x == "sha256.js") {
    var tmp;
    tmp = read(x).split("\n");
    print(tmp[86]);
    eval.call(this, tmp.join("\n"));
    return;
  }
  load_(x);
}

load("build-tcc.js");
