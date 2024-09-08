BEGIN {
  mk_args("./artifacts/M2_simple_asm_m2.exe dummy.c artifacts/out.M1");
  argc = argc_argv[0];
  argv = argc_argv[1];
  main(argc, argv);
}
