int wi8(int o, int v);
int ri8(int o);
int wi32(int o, int v);
int ri32(int o);
int expr(void);
int decl(int l);

int init_c(void);
int mk_char(int c);
int mk_c_string(int s);
int dummy(void);

int v_alloca(int x);
int enter(void);
int leave(int x);

int isalnum(int a);
int isspace(int a);
int isdigit(int a);
int strtol(int a, int b, int c);
int strstr(int a, int b);
int memcpy(int a, int b, int c);
int strcpy(int a, int b);
int fwrite(int a, int b, int c, int d);
int puts(char *a);
int printf(int a);

/*
int fgetc(int a);
int fopen(int a, int b);
int fclose(int a);
int puts(int a);
int calloc(int a, int b);
*/
