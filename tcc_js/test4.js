load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test4.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check();
