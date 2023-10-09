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

check("d718b62b4d5730e97be2d1308a680fde8a930d3802055b992a499351100df7cb");
