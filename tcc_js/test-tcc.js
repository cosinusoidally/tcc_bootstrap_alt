load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("d1dc2185ee3337541fe29c0aeb46962f67e03acdbac58764a0c9fdda0097fc97",true);
