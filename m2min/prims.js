print("loading prims");
lp=function(){
  load("prims.js");
}

prim_ops["open"]=function(){
  var pathname = get_arg(0);
  var flags = get_arg(1);
  var mode = get_arg(2);

  print("open(\"" +mk_js_string(pathname)+"\", "+flags+", "+mode+")");
  throw "open";
  op_ret();
}
