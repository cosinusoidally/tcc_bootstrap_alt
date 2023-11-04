int stdout;

int call_wrap(FUNCTION t, int a, int b){
  puts("call_wrap dummy stub");
  exit(1);
  return 1;
}

int dlsym(){
  puts("dlsym unimplemented");
  exit(1);
}

int puts(char *a) {
  fputs(a,stdout);
  fputs("\n",stdout);
}

int fread(int ptr,int size, int nitems, int stream) {
  puts("fread not impl");
/*  exit(1); */
}

int memcpy(int a, int b, int c) {
  puts("memcpy not impl");
  exit(1);
}
