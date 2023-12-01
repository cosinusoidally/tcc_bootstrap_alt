#if 0

int chdir(int d){
  puts("dummy chdir");
}

int execve(int name, int argv, int envp){
  puts("dummy execve");
  return 0;
}

#endif

int main(int argc, int **argv){
  int argc_e;
  int **argv_e;
  int name;
  argc_e=argc-2;
  argv_e=argv;
  argv_e=argv_e+2;
  name=argv_e[0];
  puts("cdrun");
  puts(name);
  puts(argv_e[1]);
  chdir(argv[1]);
  if(argv_e[argc_e]!=0){
    puts("last argv is not null");
    exit(1);
  }

  return execve(name,argv_e,0);
}
