load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("f454152c0490de66d54476adfbc165144a73a2414721f56e5f549c0c4a5558da");
