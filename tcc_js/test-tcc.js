load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("c643b9c126a4a54ac9f2b4f9a09690cfe1327461d2dee9e08989093b0f6f3c3a",true);
