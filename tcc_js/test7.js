load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test7.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("7073473b99d21b03346503fad66e0e40b39bf0df1f7156a9b48814b269a67281");
