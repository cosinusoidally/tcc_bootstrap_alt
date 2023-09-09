load("tcc.js");

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test9.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("1ccdfe49f1f64cab854c743bbfab31d834fd20b445ee9178c740ebda8b6659c0");
