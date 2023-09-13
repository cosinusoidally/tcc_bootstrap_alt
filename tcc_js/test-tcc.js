load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("1b3e358129f04cc2ccfb7a75cc4c550e7efd789e2c96ce2ceba186dd7318cba4",true);
