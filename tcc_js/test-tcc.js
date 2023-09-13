load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("0339cb5d7cbd6980c1a61f298bd2801b92aa1a78c3e89ac647c4555857169535",true);
