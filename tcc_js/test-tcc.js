load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("8b785e92a95b28078ad4bd87238491a41b730f63e2cbdca0ec26944f4706c66e",true);
