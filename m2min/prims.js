print("loading prims");
lp=function(){
  load("prims.js");
}

prim_ops["open"]=function(){
  var pathname = mk_js_string(get_arg(0));
  var flags = get_arg(1);
  var mode = get_arg(2);

  print("open(\"" +pathname+"\", "+flags+", "+mode+")");
  if((flags ===0 ) && (mode === 0)){
    if(in_file === undefined) {
      in_file=[read(pathname, "binary"), 0];
      eax = in_file_num;
    } else {
      print("in_file already loaded");
      throw "open";
    }
  } else {
    throw "open";
  }
  op_ret();
}
