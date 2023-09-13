load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("c95cc6d930461b039ab99138b93f1577b01a2fb8d0a2785fbbbfc163b0d98642",true);
