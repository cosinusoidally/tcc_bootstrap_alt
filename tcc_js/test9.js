load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("24b3577a484610dac70219d108d3ec1007704ca6a31a8b6202bcb1241d0a201f");
