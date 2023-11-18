#include "elf_loader_support_tcc.c"

int init_globals(void){
}

int load_elf(char *name){
  int f;
  int l;
  l=0;
  puts("loading elf file:");
  puts(name);
  f=fopen(name,"rb");
  while(fgetc(f) >= 0){
    l=l+1;
  }
  puts("file length");
  puts_num(l);
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
  load_elf("elf_test.o");
  puts(argv[optind]);
  return 0;
/* can't call yet
  return call_wrap(t, argc - optind, argv + (p_size*optind));
*/
}
