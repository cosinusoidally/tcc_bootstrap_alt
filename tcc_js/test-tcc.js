load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("82571735445ade4ab503680f1ac2446d07455cc60ed0f0fa8e7d445f2e9078cc",true);
