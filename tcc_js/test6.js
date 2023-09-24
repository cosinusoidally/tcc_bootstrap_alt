load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test6.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("93ea793ce87176be6989b7b45d818663a7e6d271017680ea7259ad86167e9fc5");
