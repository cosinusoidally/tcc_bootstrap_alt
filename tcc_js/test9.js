load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("868b3079705709cc1518c6347664b6226bafb9bb6192dbc3d69dba3a6e7dd5a5");
