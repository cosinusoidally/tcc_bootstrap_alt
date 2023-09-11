load("tcc.js");

wi8_orig=wi8;

wi8=function(o,v){
  if(o===0x00043c60+0x000cccfd-0x000cce70-17){
    print("dbg:");
//    backtrace();
debugger;
  }
  wi8_orig(o,v);
}

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("1dde969efcf3272803cd47d7d43ced768567b6a32450446536aafff248833dd8",true);
