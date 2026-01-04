var f_files;
var vfs;

var load_ = load;

load = function(x) {
  if(x == "sha256.js") {
    return;
  }
  load_(x);
}

load("build-tcc.js");
