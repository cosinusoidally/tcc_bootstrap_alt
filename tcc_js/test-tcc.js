load("tcc.js");

wi8_orig=wi8;

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test-tcc.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("90ffb74ec7902ea8f695d0223968fd5e05a412c48b5e283dfcccc71bbde40702",true);
