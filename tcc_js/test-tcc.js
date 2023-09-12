load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("f504b91234e6c4d5cf3ff362f41e2b835f89ca8bfbf3cc5926c062d60015c012",true);
