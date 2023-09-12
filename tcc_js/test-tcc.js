load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("09083f40dd20b386a3110702f719d36819fbe69f3ecb7abdccad00ba2dc5d344",true);
