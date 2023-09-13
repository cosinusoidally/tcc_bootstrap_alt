load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("8a5191c788601ec8f48535a2b670025a03f0d1ec4ca877e7e0e9ab0728240ce7",true);
