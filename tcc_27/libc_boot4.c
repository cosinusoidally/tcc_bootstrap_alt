/*
 *
 * This libc is cobbled together from various other libc libraries
 * includes parts of:
 *
 * https://github.com/oriansj/M2libc
 * http://git.savannah.gnu.org/cgit/mes.git/tree/?h=v0.25.1
 *
 * modifications are (C) 2023 Liam Wilson
 *
 * this libc is under the GPLv3 (same license as the libc libraries it derives
 * from)
 *
 */

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
  fputs("tcc strtod: ",stdout);
  fputs(a,stdout);
  fputs("\n",stdout);
  if(strcmp("4294967296.0",a) == 0){
    x[1]=0x41F00000;
  }
  if(strcmp("18446744073709551616.0",a) == 0){
    x[1]=0x43F00000;
  }
  if(strcmp("0.001",a) == 0){
    x[0]=0xD2F1A9FC;
    x[1]=0x3F50624D;
  }
  if(strcmp("1000000.0",a) == 0){
    x[1]=0x412E8480;
  }
  if(strcmp("1.0",a) == 0){
    x[1]=0x3FF00000;
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

int
strcmp (int a1, int b1)
{
  char *a;
  char *b;
  a=(char *)a1;
  b=(char *)b1;
  while (a[0] != 0 && b[0] != 0 && a[0] == b[0])
    {
      a = a + 1;
      b = b + 1;
    }

  return a[0] - b[0];
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

int fprintf(int stream, int fmt){
  fputs("fprintf stream: ",stdout);
  fputs(int2str(stream,10,0),stdout);
  fputs(" fmt: \"",stdout);
  fputs(fmt,stdout);
  fputs("\"\n",stdout);
  /* FIXME real impl */
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
  } else if(strcmp("%s/include", format) ==0) {
    puts("generating \"%s/include\" sprintf/snprintf string");
    fputs(a3, stdout);
    fputs("/include",stdout);
    fputs("\n",stdout);
    o=strcpy(a1, a3);
    o=strcat(a1, "/include");
    return o-a1;
  } else if(strcmp("%s/%s", format) ==0) {
    puts("generating \"%s/i%s\" sprintf/snprintf string");
    fputs(a3, stdout);
    fputs("/",stdout);
    fputs(a4, stdout);
    fputs("\n",stdout);
    o=strcpy(a1, a3);
    o=strcat(a1, "/");
    o=strcpy(a1, a4);
    return o-a1;
  } else if(strcmp("__%s_start", format) ==0) {
    puts("generating \"__%s_start\" sprintf/snprintf string");
    fputs("__",stdout);
    fputs(a3, stdout);
    fputs("_start",stdout);
    fputs("\n",stdout);
    o=strcat(a1, "__");
    o=strcpy(a1, a3);
    o=strcat(a1, "_start");
    return o-a1;
  } else if(strcmp("__%s_end", format) ==0) {
    puts("generating \"__%s_end\" sprintf/snprintf string");
    fputs("__",stdout);
    fputs(a3, stdout);
    fputs("_end",stdout);
    fputs("\n",stdout);
    o=strcat(a1, "__");
    o=strcpy(a1, a3);
    o=strcat(a1, "_end");
    return o-a1;
  } else if(strcmp("%%%s", format) ==0) {
    puts("generating \"%%%s\" sprintf/snprintf string");
    fputs("%",stdout);
    fputs(a3, stdout);
    fputs("\n",stdout);
    o=strcpy(a1, "%");
    o=strcat(a1, a3);
    return o-a1;
  } else if(strcmp("(%%%s)", format) ==0) {
    puts("generating \"(%%%s)\" sprintf/snprintf string");
    fputs("(%",stdout);
    fputs(a3, stdout);
    fputs(")\n",stdout);
    o=strcpy(a1, "(%");
    o=strcat(a1, a3);
    o=strcat(a1, ")");
    return o-a1;
  } else if(strcmp("%d(%%ebp)", format) ==0) {
    puts("generating \"%d(%%ebp)\" sprintf/snprintf string");
    /* FIXME leaky */
    fputs(int2str(a3, 10, 1), stdout);
    fputs("(%ebp)", stdout);
    fputs("\n",stdout);
    o=strcpy(a1, int2str(a3, 10, 1));
    o=strcat(a1, "(%ebp)");
    return o-a1;
  } else if(strcmp("%d", format) ==0) {
    puts("generating \"%d\" sprintf/snprintf string");
    fputs(int2str(a3, 10, 1), stdout);
    fputs("\n",stdout);
    o=strcpy(a1, int2str(a3, 10, 1));
    return o-a1;
  } else if(strcmp("%s", format) ==0) {
    puts("generating \"%s\" sprintf/snprintf string");
    fputs(a3, stdout);
    fputs("\n",stdout);
    o=strcpy(a1, a3);
    return o-a1;
  } else {
    puts("unsupported sprintf/snprintf format string");
    exit(1);
  }
  return 0;
}

unsigned int
memmove (unsigned int dest,  unsigned int src, int n)
{
  if (dest < src)
    return memcpy (dest, src, n);
  char *p=(char *)(dest + n);
  char *q = (char *)(src + n);
  while (n--)
    *--p = *--q;
  return dest;
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

int fwrite(int ptr,int size, int nitems, int stream) {
/*
  puts("fwrite:");
  puts_num(ptr);
  puts_num(size);
  puts_num(nitems);
  puts_num(stream);
  puts("fwrite not impl");
  exit(1);
*/
  int t=size*nitems;
  char *c=(char *)ptr;
  while(t>0){
    fputc(c[0],stream);
    t=t-1;
    c=c+1;
  }
}
/*
int lseek(void){
  puts("lseek not impl");
  exit(1);
}
*/
char *
strchr (char *s, int c)
{
  char *p = s;
  while (*p || !c)
    {
      if (c == *p)
        return (char *) p;
      p++;
    }
  return 0;
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

int vsnprintf(int a, int b, int fmt){
  fputs("vsnprintf fmt: \"",stdout);
  fputs(fmt,stdout);
  fputs("\"\n",stdout);
  /* FIXME real impl */
}

int longjmp(void){
  puts("longjmp not impl");
  exit(1);
}

int strtof(void){
  puts("strtof not impl");
  exit(1);
}

int strtold(void){
  puts("strtold not impl");
  exit(1);
}

int time(void){
  puts("time not impl");
  exit(1);
}

int localtime(void){
  puts("localtime not impl");
  exit(1);
}

int _setjmp(void){
  puts("_setjmp not impl");
/* FIXME should remove callers */
  return 0;
}

int
strncmp (char *a, char *b, int size)
{
  if (size == 0)
    return 0;

  while (a[0] != 0 && b[0] != 0 && a[0] == b[0] && size > 1)
    {
      size = size - 1;
      a = a + 1;
      b = b + 1;
    }

  return a[0] - b[0];
}

int
isdigit (int c)
{
  return c >= '0' && c <= '9';
}

int
isxdigit (int c)
{
  return isdigit (c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int
isnumber (int c, int base)
{
  if (base == 2)
    return (c >= '0') && (c <= '1');
  if (base == 8)
    return (c >= '0') && (c <= '7');
  if (base == 10)
    return isdigit (c);
  if (base == 16)
    return isxdigit (c);
}

int
isspace (int c)
{
  return (c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == ' ');
}

long
abtol (char **p, int base)
{
  char *s = p[0];
  int i = 0;
  int sign_p = 0;
  int m = '0';
  if (base == 0)
    base = 10;
  while (isspace (s[0]) != 0)
    s = s + 1;
  if (s[0] != 0 && s[0] == '+')
    s = s + 1;
  if (s[0] != 0 && s[0] == '-')
    {
      sign_p = 1;
      s = s + 1;
    }
  while (isnumber (s[0], base) != 0)
    {
      i = i * base;
      if (s[0] >= 'a')
        m = 'a' - 10;
      else
        {
          if (s[0] >= 'A')
            m = 'A' - 10;
          else
            m = '0';
        }
      i = i + s[0] - m;
      s = s + 1;
    }
  p[0] = s;
  if (sign_p != 0)
    return -i;

  return i;
}

long
strtol (char *string, char **tailptr, int base)
{
  if (!strncmp (string, "0x", 2))
    {
      string += 2;
      base = 16;
    }
  if (tailptr)
    {
      *tailptr = (char *) string;
      return abtol ((char const **) tailptr, base);
    }
  char **p = (char **) &string;
  return abtol ((char const **) p, base);
}

unsigned long
strtoul (char *string, char **tailptr, int base)
{
  fputs("strtoul called nptr_str: ",stdout);
  fputs(string,stdout);
  fputs(" endptr: 0x",stdout);
  fputs(int2str(tailptr,16,0),stdout);
  fputs(" base: ",stdout);
  fputs(int2str(base,10,0),stdout);
  fputs("\n",stdout);
  return strtol (string, tailptr, base);
}


int strtoll(void){
  puts("strtoll not impl");
  exit(1);
}

int gettimeofday(void){
  puts("gettimeofday not impl");
  exit(1);
}

int
atoi (char const *string)
{
  char const *p = string;
  return abtol (&p, 0);
}

int mprotect(void){
  puts("mprotect not impl");
  exit(1);
}

int getenv(int str){
  fputs("getenv str: ",stdout);
  fputs(str,stdout);
  fputs("\n",stdout);
  /* FIXME non dummy impl */
  return 0;
}

int unlink(int pathname){
  fputs("unlink pathname: ",stdout);
  fputs(pathname,stdout);
  fputs("\n",stdout);
  /* FIXME actual impl */
}

int fflush(void){
  puts("fflush not impl");
  /* FIXME real impl */
}

int sscanf(int str, int format,int *a1, int *a2, int *a3, int *a4){
  fputs("sscanf str: \"",stdout);
  fputs(str,stdout);
  fputs("\" format: \"",stdout);
  fputs(format,stdout);
  fputs("\n",stdout);
  if(strcmp(str, "0.9.26") == 0){
    a1[0]=0;
    a2[0]=9;
    a3[0]=26;
  } else if(strcmp(str, "0.9.27") == 0){
    a1[0]=0;
    a2[0]=9;
    a3[0]=27;
  } else {
    puts("unsupported sscanf format string");
    exit(1);
  }
  return 0;
}

int dlclose(void){
  puts("dlclose not impl");
  exit(1);
}

unsigned long long
strtoull (char *string, char **tailptr, int base)
{
  return strtoul (string, tailptr, base);
}

int execvp(void){
  puts("execvp not impl");
  exit(1);
}

void
qswap (void *a, void *b, int size)
{
  char *pa = a;
  char *pb = b;
  do
  {
    char tmp = *pa;
    *pa++ = *pb;
    *pb++ = tmp;
  } while (--size > 0);
}

int
qpart (void *base, int count, int size, int (*compare) (void *, void *))
{
  void *p = base + count * size;
  int i = 0;
  int j;
  for (j = 0; j < count; j++)
    {
      int c = compare (base + j * size, p);
      if (c < 0)
        {
          qswap (base + i * size, base + j * size, size);
          i++;
        }
      else if (c == 0)
        i++;
    }
  if (compare (base + count * size, base + i * size) < 0)
    qswap (base + i * size, base + count * size, size);
  return i;
}

void
qsort (void *base, int count, int size, int (*compare) (void *, void *))
{
  puts("qsort called");
  if (count > 1)
    {
      int p = qpart (base, count - 1, size, compare);
      qsort (base, p, size, compare);
      qsort (base + p * size, count - p, size, compare);
    }
}

int strstr(void){
  puts("strstr not impl");
  exit(1);
}

int fseek(void){
  puts("fseek not impl");
  exit(1);
}

int ftell(void){
  puts("ftell not impl");
  exit(1);
}

int fread(void){
  puts("fread not impl");
  exit(1);
}

int remove(void){
  puts("remove not impl");
  exit(1);
}

#define SYS_lseek  0x13

long
_sys_call3 (long sys_call, long one, long two, long three)
{
  long r;
  asm (
       "mov    %2,%%ebx\n\t"
       "mov    %3,%%ecx\n\t"
       "mov    %4,%%edx\n\t"
       "mov    %1,%%eax\n\t"
       "int    $0x80\n\t"
       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "rm" (sys_call), "rm" (one), "rm" (two), "rm" (three)
       : "eax", "ebx", "ecx", "edx"
       );
  return r;
}


int
lseek (int filedes, int offset, int whence)
{
  long long_offset = offset;
  return _sys_call3 (SYS_lseek, filedes, long_offset, whence);
}
