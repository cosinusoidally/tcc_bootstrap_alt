load("tcc.js");

/*
wi8_orig=wi8;

wi8=function(o,v){
  if(o===0x0004321e+0x000b5c8e-0x000b5d94){
    print("dbg:");
    backtrace();
  }
  wi8_orig(o,v);
}
*/

inp_orig=inp;

inp=function(){
  if(line_num===154){
    err();
  }
  return inp_orig();
}

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("fefa4408087df2adfea5cc3828a0f5c9e0cc896f8a9b0834f7cf2a4f9b9fb1f4",true);
