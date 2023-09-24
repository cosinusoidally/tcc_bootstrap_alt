load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test3.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("001c8c1d31beb97098e84ae50e24669f0d840a32ccfffccfe78f89c85a39a963");
