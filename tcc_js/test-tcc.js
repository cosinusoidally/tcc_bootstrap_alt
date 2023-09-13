load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("ace1d64d1d5f54c95ebfedff1d1978761e4643bd3be0d61b8ab9b3dd8020b8f8",true);
