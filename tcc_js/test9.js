load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("e05897351ce5d40fb4687a0e87cd33f0f94e8523f1edd8a5b4a419cf9c55efa7");
