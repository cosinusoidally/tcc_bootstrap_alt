load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("7f74804008cc6c3db2fc4d5d7890c988fa472440e63695c4bc5732c6fa0a9a87",true);
