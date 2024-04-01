/* decls for real functions */
void *malloc(int size);
int strlen(char *s);
int fopen(int path, int mode);
int fclose(int f);
int fread(int a,int b, int c, int d);
int puts(char *a);
int dlsym(int h, int sym);
int exit(int x);
int memset(int a, int b, int c);
int memcpy(int a, int b, int c);

int p_size;
int verbose;
