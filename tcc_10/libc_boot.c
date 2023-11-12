extern int stdout;

int puts(int x) {
  fputs(x, stdout);
  fputs("\n", stdout);
}

int printf(int x){
  puts("printf not impl");
  exit(1);
}

int malloc(int x){
  puts("malloc not impl");
  exit(1);
}
