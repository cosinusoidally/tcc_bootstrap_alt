#include "loader_support_tcc.c"

int init_globals(void){
}

int load_elf(char *name){
  puts("loading elf file:");
  puts(name);

}

int main(int argc, char **argv)
{
  FUNCTION t;
  int optind;

  puts("elf loader starting");

  optind = 1;

  init_c();
  init_globals();

  puts("running elf files");
  load_elf("test.o");
  puts(argv[optind]);
  return 0;
/* can't call yet
  return call_wrap(t, argc - optind, argv + (p_size*optind));
*/
}
