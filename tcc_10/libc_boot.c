extern int stdout;

int puts(int x) {
  fputs(x, stdout);
  fputs("\n", stdout);
}

int strtod(void){
  puts("strtod not impl");
  exit(1);
}

int strlen(int p){
//  puts("strlen not impl");
  int l=0;
  char *s;
  s=(char *)p;
  while(s[0]){
    s=s+1;
    l=l+1;
  }
  return l;
}

int memset(int xi, int v, int size) {
/*  puts("unimpl memset"); err(); */
  int i;
  char *x;
  x=(char *)xi;
  for(i=0;i<size;i=i+1){
    x[i]=v;
  }
}

int strcpy(int d, int s) {
//  puts("strcpy not impl");
  int c;
  char *dest;
  char *src;
  dest=(char *)d;
  src=(char *)s;
  while((c=src[0])!=0){
    src=src+1;
    dest[0]=c;
    dest=dest+1;
  }
  dest[0]=0;
}

int strcmp(void){
  puts("strcmp not impl");
  exit(1);
}

int strcat(void){
  puts("strcat not impl");
  exit(1);
}

int fprintf(void){
  puts("fprintf not impl");
  exit(1);
}

int vfprintf(void){
  puts("vfprintf not impl");
  exit(1);
}

int memcmp(int s1, int s2, int n) {
  int i;
  int r;
  char *p1;
  char *p2;
  r=0;
  p1=(char *)s1;
  p2=(char *)s2;
  for(i=0;i<n;i=i+1){
    if(p1[i]!=p2[i]){
/* FIXME ljw not quite right */
      r=1;
    }
  }
  return r;
}

int memcpy(int a, int b, int c) {
  char *dest;
  char *src;
  int i;
  dest=(char *)a;
  src=(char *)b;
  for(i=0;i<c;i=i+1){
    dest[i]=src[i];
  }
}

int sprintf(void){
  puts("sprintf not impl");
  exit(1);
}

int read(void){
  puts("read not impl");
  exit(1);
}

int memmove(void){
  puts("memmove not impl");
  exit(1);
}

int strrchr(void){
  puts("strrchr not impl");
  exit(1);
}

int ldexp(void){
  puts("ldexp not impl");
  exit(1);
}

int snprintf(void){
  puts("snprintf not impl");
  exit(1);
}

int getcwd(void){
  puts("getcwd not impl");
  exit(1);
}

int dlsym(void){
  puts("dlsym not impl");
  exit(1);
}

int fwrite(void){
  puts("fwrite not impl");
  exit(1);
}

int lseek(void){
  puts("lseek not impl");
  exit(1);
}

int strtol(void){
  puts("strtol not impl");
  exit(1);
}

int strchr(void){
  puts("strchr not impl");
  exit(1);
}

int sigemptyset(void){
  puts("sigemptyset not impl");
  exit(1);
}

int sigaction(void){
  puts("sigaction not impl");
  exit(1);
}

int dlopen(void){
  puts("dlopen not impl");
  exit(1);
}

int printf(int x){
  puts("printf not impl");
  exit(1);
}
