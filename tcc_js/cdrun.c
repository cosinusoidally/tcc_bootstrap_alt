/* cc_x86 has no preprocessor so will declare chdir and execve here */

#if 0

int chdir(char* path) {
        asm("lea_ebx,[esp+DWORD] %4"
            "mov_ebx,[ebx]"
            "mov_eax, %12"
            "int !0x80");
}

int execve(char* file_name, char** argv, char** envp) {
        asm("lea_ebx,[esp+DWORD] %12"
            "mov_ebx,[ebx]"
            "lea_ecx,[esp+DWORD] %8"
            "mov_ecx,[ecx]"
            "lea_edx,[esp+DWORD] %4"
            "mov_edx,[edx]"
            "mov_eax, %11"
            "int !0x80");
}

#endif

int main(int argc, int **argv){
  int argc_e;
  int **argv_e;
  char *tmp;
  int name;
  int target_dir;
  target_dir=argv[1];
  argc_e=argc-2;
  argv_e=argv;
  tmp=argv_e;
  tmp=tmp+(2*4);
  argv_e=tmp;
  name=argv_e[0];
  puts("cdrun");
  fputs("cdrun target_dir: ",stdout);
  fputs(target_dir,stdout);
  fputs("\n",stdout);
  fputs("cdrun name: ",stdout);
  fputs(name,stdout);
  fputs("\n",stdout);
  fputs("cdrun first_arg: ",stdout);
  fputs(argv_e[1],stdout);
  fputs("\n",stdout);
  chdir(target_dir);
  if(argv_e[argc_e]!=0){
    puts("last argv is not null");
    exit(1);
  }

  return execve(name,argv_e,0);
}
