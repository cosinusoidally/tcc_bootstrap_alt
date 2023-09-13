load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("6bb46a28a2aeaa6b433ea1b4b14ecd7a961a3bddb517bee86240b605e384699a",true);
