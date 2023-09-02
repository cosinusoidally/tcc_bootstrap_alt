load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("f864413ede02dd85806407450bc53d741f2dee88a4cec80103c0679a7cb9b1c7");
