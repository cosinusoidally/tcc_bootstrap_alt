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

prim_ops["gt"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  print("gt(" +a+", "+b+")");
  eax = (a > b) | 0;
  op_ret();
}

prim_ops["fgetc"]=function(){
  var a = get_arg(0);

  print("fgetc(" +a+")");

  if(a === in_file_num) {
    if(in_file[1] < in_file[0].length) {
      eax = in_file[0][in_file[1]];
      print("fgetc: "+String.fromCharCode(eax));
      in_file[1]=in_file[1]+1;
    } else {
      print("fgetc: EOF");
      eax = -1;
    }
  } else {
    print("fgetc wrong file descriptor");
    throw "fgetc";
  }

  op_ret();
}

prim_ops["neq"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  print("neq(" +a+", "+b+")");
  eax = (a != b) | 0;

  op_ret();
}

prim_ops["shr"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  print("shr(" +a+", "+b+")");
  eax = a >>> b;

  op_ret();
}

