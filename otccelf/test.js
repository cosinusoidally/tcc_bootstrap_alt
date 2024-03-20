print("Test JS version of otcelf");

load("support.js");
load("support_common.js");
load("otccelf_full.js");

argc_argv=mk_argc_argv("otccelf artifacts/otccelf_js.c otccelf_js_js.exe");
print(JSON.stringify(argc_argv));
main(argc_argv.argc,argc_argv.argv);
