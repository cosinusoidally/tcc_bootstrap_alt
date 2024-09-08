BEGIN {
  init_runtime();
  if(!my_fname) {
    my_fname = "test.c"
  }
  mk_args("./artifacts/M2_simple_asm_m2.exe " my_fname " artifacts/out.M1");
  print "argc: " argc;
  print "argv: " argv;
  main(argc, argv);
  exit;
}
