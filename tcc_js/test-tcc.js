load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("3a2c34b32cd2467d6488d9b70653ec854e5e2452df414dbb7df2a171673386a1",true);
