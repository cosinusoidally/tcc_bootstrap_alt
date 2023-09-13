load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("458dc142bfe9f2af1dd84b9fbcf2a624fbf995de2cf8a3a96507c8ebe5b1898d",true);
