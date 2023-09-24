load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("0ee8d6110aeface2e916bd91075bc0ff535e61af6ad2b8215073ad74fefdb8fc",true);
