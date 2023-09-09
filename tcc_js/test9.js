load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("094c8e81b31f362a613c6f963511e5d08a1a4d78ba4fadbc7f01ee794621c5d8");
