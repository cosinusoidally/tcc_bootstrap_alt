load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("8b703b8612addcbe0fa8352889a9266bfad58548fe72d116b594b4b30ed6e8f0",true);
