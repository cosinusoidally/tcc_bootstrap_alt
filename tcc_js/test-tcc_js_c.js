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

check("0f533aa55e3974126174c4e851dbbc380f1d9b71482bbcdd14fd82ff864884cb");
