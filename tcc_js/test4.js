load("tcc.js");

/*
wi8_orig=wi8;

wi8=function(o,v){
if(ri32(tokc)===-2080374784){
err();
}
return wi8_orig(o,v);
};
*/

// dummy main call:
argc_argv=mk_argc_argv("tcc -r test4.c");
print(JSON.stringify(argc_argv));
//err();
main(argc_argv.argc,argc_argv.argv);

check("cf167a1b44d14de150c059664f46cfb9007b5c51d1b4d715aaa7316817bac133");
