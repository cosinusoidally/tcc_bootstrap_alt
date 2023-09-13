load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("3df7b4e36ed4ac69251798bc242b7d37da4e4a672a92af6fc22db49f5b719f33",true);
