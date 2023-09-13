load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("d3a2d0695cbdc23dffb4634a1393633b624b3f5ce250417c09fd98d872a96b20",true);
