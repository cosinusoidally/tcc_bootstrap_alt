load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("a0515c8fb0be7d50b2aaaf0ffc784469c6d9ccd87474ac319deb2d5fefbab7ea",true);
