load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("c679d7e58e460fef72e88d049d3b0963e429aa10a3f53edfb98e10164af17fe2",true);
