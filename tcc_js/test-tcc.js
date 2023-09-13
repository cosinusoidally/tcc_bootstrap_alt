load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("2f7a6f64b0aeeede06ecb9822075682e4b3f29341897da5fb262ccedcf0c64c3",true);
