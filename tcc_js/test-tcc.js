load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-dlsym_wrap.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("78f8497990a829e99b18baf1430e871a8702602904bfd2901f13f7a5ac093cfe",true);
