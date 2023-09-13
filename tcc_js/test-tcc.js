load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("5a68200a686c378b396497e3ec930fe4bc6ed1d8df7a89791dab7fab0d587544",true);
