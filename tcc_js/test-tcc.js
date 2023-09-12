load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("ad99d2c18c63d7e4f9c9d44821b8a8fbb9d94e3346f30fd1b37dfc10b4f0ffcc",true);
