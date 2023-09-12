load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("74c10bf1a3b8855510ee2977f4bb1da41d02bb3ea3a6edcb19b4d769376eb64f",true);
