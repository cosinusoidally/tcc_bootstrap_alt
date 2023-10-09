load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test10.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("2bc371385f2e55f7bb1b037f2cadb009345e399d04040bec9b2d78959451c6ee");
