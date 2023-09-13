load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("341fe80b72e2cf0ace8f968edb36abdd3d22a96064e958b4f18e28c4b13ea9d9",true);
