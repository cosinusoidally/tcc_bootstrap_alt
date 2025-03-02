int or(int a, int b);
int and(int a, int b);
int ri8(int a);
int shl(int a, int b);
int add(int a, int b);
int wi8(int a, int b);
int shr(int a, int b);
int sub(int a, int b);
int lt(int a, int b);
int gt(int a, int b);
int mks(char *a);
int mul(int a, int b);
int eq(int a, int b);
int neq(int a, int b);
int mod(int a, int b);
int _div(int a, int b);
int gte(int a, int b);
int mkc(int a);

int v_calloc(int a, int b);
int v_fgetc(int a);
int v_fputs(int a, int b);
int v_free(int a);
int v_fopen(int a, int b);
int v_fclose(int a);

int v_init_support();

int exit(int a);

int emit_out(int a);
int expression();
int statement();
