load("tcc.js");

wi8_orig=wi8;

unsigned=function(){
// we don't want to allow the use of unsigned ints
err();
}

// dummy main call:
argc_argv=mk_argc_argv("tcc -r /tmp/tcc_js_tmp/tcc_js.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("cf8cfb258fad1a529c037cd2cb400ed98ce7826cc4bfecae177991df4f97c360");
