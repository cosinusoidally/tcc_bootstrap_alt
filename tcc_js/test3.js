load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test2.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("fe1352ca0cdd38d869d3dd4475fa0085eeaeedb6dda05b6df80c22a01191483d");
