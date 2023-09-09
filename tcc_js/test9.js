load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("0d5157d85038efe5e4ba350e4622cc0faeca03be9a8be91add6d171b724759c8");
