print("loading prims");
lp=function(){
  var c;
  var n;
  load("prims.js");
  prims_index=[];
  for(var i=0;i<prims.length;i++){
    prims_index[i]=prim_ops[prims[i]];
  }
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
  } else if((flags === 577 ) && (mode === 384)){
    if(out_file === undefined) {
      out_file = [];
      eax = out_file_num;
    } else {
      print("out_file already loaded");
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

  if(dbg) {
    print("gt(" +a+", "+b+")");
  }
  eax = (a > b) | 0;
  op_ret();
}

prim_ops["gte"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("gte(" +a+", "+b+")");
  }
  eax = (a >= b) | 0;

  op_ret();
}

prim_ops["fgetc"]=function(){
  var a = get_arg(0);

  if(dbg) {
    print("fgetc(" +a+")");
  }
  if(a === in_file_num) {
    if(in_file[1] < in_file[0].length) {
      eax = in_file[0][in_file[1]];
      if(dbg) {
        print("fgetc: "+String.fromCharCode(eax));
      }
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

  if(dbg) {
    print("neq(" +a+", "+b+")");
  }
  eax = (a != b) | 0;

  op_ret();
}

prim_ops["shr"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("shr(" +a+", "+b+")");
  }
  eax = a >>> b;

  op_ret();
}

prim_ops["close"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  print("close("+a+")");
  eax = 0;

  op_ret();
}

function dl(){
  for(var i=0;i<out2.length;i++){
    print_old(out2[i]);
  }

  for(var i=0;i<out.length;i++){
    print_old(out[i]);
  }
}

prim_ops["mod"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("mod(" +a+", "+b+")");
  }
  eax = a % b;

  op_ret();
}

prim_ops["div"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("div(" +a+", "+b+")");
  }
  eax = Math.floor(a / b);

  op_ret();
}

prim_ops["fputc"]=function(){
  var c = get_arg(0);
  var stream = get_arg(1);

  if(dbg) {
    print("fputc(" +c+", "+stream+")");
  }
  out_file.push(c);

  op_ret();
}

prim_ops["lte"]=function(){
  var a = get_arg(0);
  var b = get_arg(1);

  if(dbg) {
    print("lte(" +a+", "+b+")");
  }
  eax = (a <= b) | 0;

  op_ret();
}

