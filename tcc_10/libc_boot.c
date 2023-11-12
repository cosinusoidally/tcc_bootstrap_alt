extern int stdout;

int puts(int x) {
  fputs(x, stdout);
  fputs("\n", stdout);
}

int strtod(void){
  puts("strtod not impl");
  exit(1);
}

int strlen(void){
  puts("strlen not impl");
  exit(1);
}

int memset(void){
  puts("memset not impl");
  exit(1);
}

int strcpy(void){
  puts("strcpy not impl");
  exit(1);
}

int strcmp(void){
  puts("strcmp not impl");
  exit(1);
}

int printf(int x){
  puts("printf not impl");
  exit(1);
}
