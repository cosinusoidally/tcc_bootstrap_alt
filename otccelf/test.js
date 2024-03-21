print("Test JS version of otcelf");

load("support.js");
load("support_common.js");
load("otccelf_full.js");

argc_argv=mk_argc_argv("otccelf ex1.c otccelf_js_js.exe");
print(JSON.stringify(argc_argv));
 main(argc_argv.argc,argc_argv.argv);
// main(0,0);

e="f99cbf13a24ff16e1732756dea7ad36b3f073d628c8fd4bead71579cf7d543af";
sha=root.sha256(vfs["otccelf_js_js.exe"]);
print(sha);
print(e==sha);
check(e,true);
