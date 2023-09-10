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

check("b8eb66071146796a5873d2d979af5c4845c5c34e141b51eca2c48a714e0a2832",true);
