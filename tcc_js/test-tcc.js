load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("28faf69dd65d70559b7b092e022db7cde5d5c1a34ac14f7e5dedba507178207c",true);
