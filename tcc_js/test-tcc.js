load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("de94225d4b5e3e289bada1241efa04ff85b30e308fdcc241f7509bf5c76fdb1f",true);
