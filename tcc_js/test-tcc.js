load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("0b99729875157cf0007b39b70bfb816f4a59ad4849f6c87d7706d2fa35cabef8",true);
