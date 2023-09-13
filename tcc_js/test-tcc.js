load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("37d0b4b9980a4a99ae38d1212bfa2052bee14c7b5222fdc7b5afbe03969f181d",true);
