load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("7bec083cb81e05b5abea2996bd4ab5cac26fc67df4bf7d6b77ed692f75bed877",true);
