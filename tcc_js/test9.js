load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("64a86d0c530196923bf31dafaf5564c91d33c82d841cfd9efe8e8b55610b37c8");
