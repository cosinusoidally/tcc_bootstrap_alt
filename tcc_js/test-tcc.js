load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("8d90f3945a213c3ff1c38cc92c7c750aace9302bb590fb46b63142b0e056d9b6",true);
