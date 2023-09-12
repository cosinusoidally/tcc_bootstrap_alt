load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("efd4e8f92cecca8e5f83ed16d7f5d31ee029f0acd32657b871775388726a6af3",true);
