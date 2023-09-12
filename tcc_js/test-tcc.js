load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("6635f62e9163964b0e376232d7e6e2a4154d89669e1eefa25cc6e0832069f52e",true);
