load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test2.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check();
print(root.sha256(vfs["tcc_boot.o"]));
