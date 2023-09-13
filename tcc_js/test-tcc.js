load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("1c9b0b4518cc6a042982e28c41ef6f1451ac4ed417f92f0e8a29eaf823909008",true);
