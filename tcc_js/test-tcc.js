load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("86fb292f1951ca03d606e0dd4c52271c66c040144b57fb85e17cbfc87ec48424",true);
