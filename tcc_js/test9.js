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
// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("8e4b6259bd9d8dd86b508674ab7a3434b4be34e299deda4aed002240d6d4ddad",true);
