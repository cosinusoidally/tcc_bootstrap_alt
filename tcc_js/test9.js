load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("d058ebf620c8409f84e76472448226e30742c1aaba0d02dfb11a6974151488e3");
