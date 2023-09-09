load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("d5ff5f95b22dde694bfd803d27346e0b44e2b73b259ac10750dfa2e878c1f53f");
