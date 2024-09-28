BEGIN {
  init_runtime();
  if(!my_fname) {
    my_fname = "test.c"
  }
  mk_args("./artifacts/M2_simple_asm_m2.exe " my_fname " artifacts/M2_simple_asm_m2.M1");
  print "argc: " argc;
  print "argv: " argv;
  main(argc, argv);
  print("or_count: " or_count);
  print("and_count: " and_count);
  exit;
}
