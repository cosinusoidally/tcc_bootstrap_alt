BEGIN {
  init_runtime();
  mk_args("./artifacts/M2_simple_asm_m2.exe test.c artifacts/out.M1");
  print "argc: " argc;
  print "argv: " argv;
  main(argc, argv);
}
