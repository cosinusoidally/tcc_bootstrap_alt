load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("1917de0b1d93a9a8fa4865674119bb4e95e4b2fb0c85024a7cfa2c16f6fc24cf");
