load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("be9f75b7b3d852ee2d5b1743c52ca502f354fe227ac5d354dcb87a29af23e076",true);
