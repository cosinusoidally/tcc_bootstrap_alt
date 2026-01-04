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
    var tmp2;
    tmp = read(x).split("\n");
    if(tmp[85] === "    var crypto = eval(\"require('crypto')\");") {
      tmp[85] = "var crypto;";
    } else {
      print("error line does not match: " +tmp2);
      throw "error";
    }
    if(tmp[86] === "    var Buffer = eval(\"require('buffer').Buffer\");") {
      tmp[86] = "var Buffer;";
    } else {
      print("error line does not match: " +tmp2);
      throw "error";
    }
//    print("got here");
//    print(tmp.join("\n"));
    eval.call(this, tmp.join("\n"));
    return;
  }
  load_(x);
}

load("build-tcc.js");
