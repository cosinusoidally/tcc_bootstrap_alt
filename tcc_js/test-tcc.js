load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("e05fc5331e139847c8f22cf9ee5c6fcb0e00428f7534a0fc671d15886f3ac1f9",true);
