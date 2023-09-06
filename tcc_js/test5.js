load("tcc.js");

wi8_orig=wi8;

wi8=function(o,v){
if(o==glo_base+2){
print("dbg:");
backtrace();
}
return wi8_orig(o,v);
};

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test5.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("37fc0173874e232b69ee2da750d81be0f610ac9d14b9477fa36e0c418c6f31f8");
