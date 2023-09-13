load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("8a2fc23ebe1f8bac7c4839d51e939476d492a15cc1918a455d8dd922730ed5c2",true);
