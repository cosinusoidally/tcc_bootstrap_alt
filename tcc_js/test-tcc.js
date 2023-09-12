load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("21e39a37632feaf595fea038f2bb0f6f0a5583e50ca26499c9f03f0ce19ead74",true);
