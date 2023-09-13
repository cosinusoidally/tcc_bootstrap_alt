load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("a9c0817536dd0887c7d103578291abe85d9bb1a05c245a7cd2b107d84455fe64",true);
