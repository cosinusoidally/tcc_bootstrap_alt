load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("6e447578d6dfffce5c77eba3aa08f7a3e3597ec983459417c689d935c5340f67",true);
