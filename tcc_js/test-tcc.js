load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("348cb5c2ec5ce7478de8791ba1d886f5387ed9144b118b99a1f6251e3fb31661",true);
