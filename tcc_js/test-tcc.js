load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("dfe1e93cc39173942f9cb0728b577a22dfecb1c4b89d7214892117a7ac4b6c6d",true);
