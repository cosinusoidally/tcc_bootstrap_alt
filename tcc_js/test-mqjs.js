var f_files;
var vfs;
var v_malloc;
var strlen;
var print_orig;
var to_hex_orig;
var argc_argv;
var s;
var file_o;

var load_ = load;

load = function(x) {
  if(x == "sha256.js") {
    return;
  }
  load_(x);
}

load("build-tcc.js");
