load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("b4e5e4a7f949bd4607e215b196e457d5e505d05f4f329c1249ab0d42256b16f8",true);
