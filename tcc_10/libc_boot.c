extern int stdout;

int strcpy(int d, int s);
int strcat(int de,int s);

int puts(int x) {
  fputs(x, stdout);
  fputs("\n", stdout);
}

/* adapted from bootstrappable_load.c */
char* int2str(int x, int base, int signed_p)
{
        /* Be overly conservative and save space for 32binary digits and padding null */
        char* p = (char *)calloc(34, sizeof(char));
        /* if calloc fails return null to let calling code deal with it */
        if(0 == p) return p;

        p = p + 32;
        unsigned i;
        int sign_p = 0;
        char* table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        if(signed_p && (10 == base) && (0 != (x & 0x80000000)))
        {
                /* Truncate to 31bits */
                i = -x & 0x7FFFFFFF;
                if(0 == i) return "-2147483648";
                sign_p = 1;
        } /* Truncate to 32bits */
        else i = x & (0x7FFFFFFF | (1 << 31));

        do
        {
                p[0] = table[i % base];
                p = p - 1;
                i = i / base;
        } while(0 < i);

        if(sign_p)
        {
                p[0] = '-';
                p = p - 1;
        }

        return p + 1;
}

/* FIXME dummy impl */
double strtod(char *a, char **p){
  double v;
  int *x;
  x=&v;
  x[0]=0;
  x[1]=0;
  puts("tcc strtod");
  if(strcmp("4294967296.0",a) == 0){
    x[1]=0x41F00000;
  }
  return v;
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

int strcmp(int a, int b){
  char *s1;
  char *s2;
  int i;
  int r;
  i=0;
  r=1;
  s1=(char *)a;
  s2=(char *)b;
  while(s1[i]!=0){
    r=0;
    if(s1[i]!=s2[i]){
      r=1;
      break;
    }
    i=i+1;
  }
  return r;
}

int strcat(int de,int s) {
  int d;
  char *dest;
  char *src;
  dest=(char *)de;
  src=(char *)s;
  d=dest;
  int c;
  while(dest[0]){
    dest=dest+1;
  }
  while(c=src[0]){
    src=src+1;
    dest[0]=c;
    dest=dest+1;
  }
  dest[0]=0;
  return d;
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

int sprintf(int a1, int a2, int a3, int a4, int a5, int a6){
  int format;
  int o;
  format = a2;
  fputs("sprintf \"", stdout);
  fputs(format, stdout);
  fputs("\"\n", stdout);
  if(strcmp(".rel%s", format) ==0) {
    puts("generating \".rel%s\" sprintf/snprintf string");
    fputs(".rel", stdout);
    fputs(a3,stdout);
    fputs("\n",stdout);
    o=strcpy(a1,".rel");
    o=strcat(a1, a3);
    return o-a1;
  } else if(strcmp("L.%u", format) ==0) {
    puts("generating \".L.%u\" sprintf/snprintf string");
    fputs("L.", stdout);
    /* FIXME leaky */
    fputs(int2str(a3, 10, 0), stdout);
    fputs("\n",stdout);
    o=strcpy(a1,"L.");
    o=strcat(a1, int2str(a3, 10, 0));
    return o-a1;
  } else {
    puts("unsupported sprintf/snprintf format string");
    exit(1);
  }
  return 0;
}

int read_wrap(int fd, int buf, int count) {
  int r;
  puts("read");
//  puts_num(count);
  r=fread(buf ,1, count, fd);
//  puts_num(r);
  return r;
}

int memmove(void){
  puts("memmove not impl");
  exit(1);
}

int strrchr(int p, int c) {
  int c1;
  char *s;
  char *r;
  r=0;
  s=(char *)p;
  while(c1=s[0]){
    s=s+1;
    if(c1==c){
      r=s-1;
    }
  }
  return r;
}

int ldexp(void){
  puts("ldexp not impl");
  exit(1);
}

int snprintf(int a1, int a2, int a3, int a4, int a5, int a6){
  int format;
  int size;
  size = a2;
  format = a3;
  fputs("snprintf size: ", stdout);
  /* FIXME leaky */
  fputs(int2str(size, 10, 0), stdout);
  fputs(" format: \"", stdout);
  fputs(format, stdout);
  fputs("\"\n", stdout);
  sprintf(a1, a3, a4, a5, a6, 0);
  return 0;
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
