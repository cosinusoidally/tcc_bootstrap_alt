load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("35b75cc4b072bbbbd81b93bb15f8c45f04747c9aaded72872771f14e3d32ada1",true);
