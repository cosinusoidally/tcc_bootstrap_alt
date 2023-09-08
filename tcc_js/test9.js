load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("8c6c3e6be8d1061913e1b9d25668af834908ed0dc4ecc2dafad8f8d64928afa9");
