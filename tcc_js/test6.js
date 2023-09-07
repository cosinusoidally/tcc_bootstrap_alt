load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test6.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("b701d1be5c3563bac288f5849b76e8a18179d4b0190a8558e3e408f103a04890");
