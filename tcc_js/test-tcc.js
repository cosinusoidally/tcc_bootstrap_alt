load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("d3eb8b1add562a13b83d1584435f018969adb6a96d21eeab894c27b2c1eb53ec",true);
