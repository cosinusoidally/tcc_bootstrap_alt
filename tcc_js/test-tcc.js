load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("2414a9bb1d83a1885449c0f2afe4b328dd940ce85abaec508c0ecad1c8212ed9",true);
