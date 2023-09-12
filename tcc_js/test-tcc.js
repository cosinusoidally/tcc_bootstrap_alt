load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("68a6c696c97770a9bab293992cd4750f9960829d17c998677fb98517467f8c8e",true);
