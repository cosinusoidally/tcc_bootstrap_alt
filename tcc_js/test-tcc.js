load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("ca389d28afb9804dc7d24e4ab7c342fb0d21e2db9b987f4f1243325c34043aa9",true);
