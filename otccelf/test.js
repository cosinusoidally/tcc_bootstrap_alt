print("Test JS version of otcelf");

load("support.js");
load("support_common.js");
load("otccelf_full.js");

argc_argv=mk_argc_argv("otccelf artifacts/otccelf_js.c otccelf_js_js.exe");
print(JSON.stringify(argc_argv));
 main(argc_argv.argc,argc_argv.argv);
// main(0,0);

e="b2b9028c8501dafdd792e72489c3d23e2cb05ab7805e215f3afd898ef8f15a10"
sha=root.sha256(vfs["otccelf_js_js.exe"]);
print(sha);
print(e==sha);
check(e,true);
