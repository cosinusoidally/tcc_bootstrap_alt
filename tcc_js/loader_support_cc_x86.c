int stdout;
void puts_num(int x);

int call_wrap(FUNCTION t, int a, int b){
  puts("call_wrap dummy stub");
  exit(1);
  return 1;
}

int strcmp(int a, int b){
  char *s1;
  char *s2;
  int i;
  int r;
  i=0;
  r=1;
  s1=a;
  s2=b;
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

int dlsym(int h,int sym){
  if(strcmp(sym, "malloc")==0) {
    return malloc;
  }
  if(strcmp(sym, "strlen")==0) {
    return strlen;
  }
  if(strcmp(sym, "fopen")==0) {
    return fopen;
  }
  if(strcmp(sym, "fclose")==0) {
    return fclose;
  }
  if(strcmp(sym, "fwrite")==0) {
    return 0;
  }
  if(strcmp(sym, "getc_unlocked")==0) {
    return 0;
  }
  if(strcmp(sym, "puts")==0) {
    return puts;
  }
  if(strcmp(sym, "printf")==0) {
    return 0;
  }
  if(strcmp(sym, "exit")==0) {
    return exit;
  }
  puts("dlsym missing sym");
  exit(1);
}

int puts(char *a) {
  fputs(a,stdout);
  fputs("\n",stdout);
}

void puts_num(int x){
  char *s;
  s=int2str(x,10,0);
  puts(s);
  free(s);
  return;
}

int fread(int ptr,int size, int nitems, int stream) {
/*
  puts("fread not impl");
  exit(1);
*/
  int t=size*nitems;
  char *c=ptr;
  while(t>0){
    c[0]=fgetc(stream);
    t=t-1;
    c=c+1;
  }
}

int memcpy(int a, int b, int c) {
  char *dest;
  char *src;
  int i;
  dest=a;
  src=b;
  for(i=0;i<c;i=i+1){
    dest[i]=src[i];
  }
}
