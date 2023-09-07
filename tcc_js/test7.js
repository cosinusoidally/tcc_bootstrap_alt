load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test7.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("27bbd1c545da7da9d6fd9f5547d89796332ad4d19e1b487fdb0d42ddd115831d");
