load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("5d4f3a0ce9916793f6fcbeee8ff0b446676c282c13a5f6faefcacfcfa57007f3",true);
