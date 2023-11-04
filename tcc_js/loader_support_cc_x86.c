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
