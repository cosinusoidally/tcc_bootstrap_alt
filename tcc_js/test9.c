#include "test9.h"

#define SYM_HASH_SIZE       263

/* token symbol management */
typedef struct TokenSym {
    struct TokenSym *hash_next;
    int tok; /* token number */
    int len;
    char str[1];
} TokenSym;

/* constant value */
typedef union CValue {
    int i;
    unsigned int ui;
    unsigned int ul; /* address (should be unsigned long on 64 bit cpu) */
    struct TokenSym *ts;
    int tab[1];
    struct Sym *sym;
} CValue;

/* symbol management */
typedef struct Sym {
    int v;    /* symbol token */
    int t;    /* associated type */
    int c;    /* associated number */
    struct Sym *next; /* next related symbol */
    struct Sym *prev; /* prev symbol in stack */
    struct Sym *hash_next; /* next symbol in hash table */
} Sym;

typedef struct SymStack {
  struct Sym *top;
  struct Sym *hash[SYM_HASH_SIZE];
} SymStack;

#define INCLUDE_PATHS_MAX   32

#define TOK_HASH_SIZE       521
#define TOK_ALLOC_INCR      256 /* must be a power of two */

#define TOK_DEC   0xa2
#define TOK_MID   0xa3 /* inc/dec, to void constant */
#define TOK_INC   0xa4
#define TOK_ARROW 0xa7
#define TOK_DOTS  0xa8 /* three dots */
#define TOK_SHR   0xa9 /* unsigned shift right */
#define TOK_UDIV  0xb0 /* unsigned division */
#define TOK_UMOD  0xb1 /* unsigned modulo */
#define TOK_PDIV  0xb2 /* fast division with undefined rounding for pointers */
#define TOK_NUM   0xb3 /* number in tokc */
#define TOK_CCHAR 0xb4 /* char constant in tokc */
#define TOK_STR   0xb5 /* pointer to string in tokc */
#define TOK_TWOSHARPS 0xb6 /* ## preprocessing token */
#define TOK_LCHAR 0xb7
#define TOK_LSTR  0xb8
#define TOK_CFLOAT   0xb9 /* float constant */
#define TOK_CDOUBLE  0xc0 /* double constant */
#define TOK_CLDOUBLE 0xc1 /* long double constant */

/* field 'Sym.t' for macros */
#define MACRO_OBJ      0 /* object like macro */
#define MACRO_FUNC     1 /* function like macro */

/* all identificators and strings have token above that */
#define TOK_IDENT 256

char *include_paths[INCLUDE_PATHS_MAX];
int nb_include_paths;

int tok_ident;
TokenSym **table_ident;
TokenSym *hash_ident[TOK_HASH_SIZE];

#define SYM_FIRST_ANOM (1 << (31 - VT_STRUCT_SHIFT)) /* first anonymous sym */

#define VT_STRUCT_SHIFT 16   /* structure/enum name shift (16 bits left) */

/* contains global symbols which remain between each translation unit */
SymStack extern_stack;
SymStack define_stack, global_stack, local_stack, label_stack;

TokenSym *tok_alloc(char *str, int len)
{
    TokenSym *ts, **pts, **ptable;
    int h, i;

    if (len <= 0)
        len = strlen(str);
    h = 1;
    for(i=0;i<len;i++)
        h = ((h << 8) | (str[i] & 0xff)) % TOK_HASH_SIZE;

    pts = &hash_ident[h];
    while (1) {
        ts = *pts;
        if (!ts)
            break;
        if (ts->len == len && !memcmp(ts->str, str, len))
            return ts;
        pts = &(ts->hash_next);
    }

    if (tok_ident >= SYM_FIRST_ANOM)
        error("memory full");

    /* expand token table if needed */
    i = tok_ident - TOK_IDENT;
    if ((i % TOK_ALLOC_INCR) == 0) {
        ptable = realloc(table_ident, (i + TOK_ALLOC_INCR) * sizeof(TokenSym *));
        if (!ptable)
            error("memory full");
        table_ident = ptable;
    }

    ts = malloc(sizeof(TokenSym) + len);
    if (!ts)
        error("memory full");
    table_ident[i] = ts;
    ts->tok = tok_ident++;
    ts->len = len;
    ts->hash_next = NULL;
    memcpy(ts->str, str, len + 1);
    *pts = ts;
    return ts;
}

/* return the number of additionnal 'ints' necessary to store the
   token */
static inline int tok_ext_size(int t)
{
    switch(t) {
       /* 4 bytes */
    case TOK_NUM:
    case TOK_CCHAR:
    case TOK_LCHAR:
    case TOK_STR:
    case TOK_LSTR:
    case TOK_CFLOAT:
        return 1;
    case TOK_CDOUBLE:
        return 2;
    default:
        return 0;
    }
}

void tok_add(int **tok_str, int *tok_len, int t)
{
    int len, *str;
    len = *tok_len;
    str = *tok_str;
    if ((len & 63) == 0) {
        str = realloc(str, (len + 64) * sizeof(int));
        if (!str)
            return;
        *tok_str = str;
    }
    str[len++] = t;
    *tok_len = len;
}

void tok_add2(int **tok_str, int *tok_len, int t, CValue *cv)
{
    int n, i;

    tok_add(tok_str, tok_len, t);
    n = tok_ext_size(t);
    for(i=0;i<n;i++)
        tok_add(tok_str, tok_len, cv->tab[i]);
}

//unsigned int HASH_SYM(int v) {
//    return ((unsigned)(v) % SYM_HASH_SIZE);
//}

Sym *sym_push1(SymStack *st, int v, int t, int c)
{
    Sym *s, **ps;
//    s = sym_push2(&st->top, v, t, c);
//    /* add in hash table */
//    if (v) {
//        ps = &st->hash[HASH_SYM(v)];
//        s->hash_next = *ps;
//        *ps = s;
//    }
    return s;
}

/* XXX: should be more factorized */
void define_symbol(char *sym)
{
    TokenSym *ts;
    int *str, len;
    CValue cval;

    ts = tok_alloc(sym, 0);
    str = NULL;
    len = 0;
    cval.i = 1;
    tok_add2(&str, &len, TOK_NUM, &cval);
    tok_add(&str, &len, 0);
    sym_push1(&define_stack, ts->tok, MACRO_OBJ, (int)str);
}


int main(int argc, char **argv)
{
    puts("tcc 1_7 start");
    Sym *s;
    int (*t)();
    char *p, *r, *outfile;
    int optind;

    include_paths[0] = "../tcc_1_7/";
    nb_include_paths = 1;

    /* add all tokens */
    tok_ident = TOK_IDENT;
    p = "int\0void\0char\0if\0else\0while\0break\0return\0for\0extern\0static\0unsigned\0goto\0do\0continue\0switch\0case\0const\0volatile\0long\0register\0signed\0auto\0inline\0restrict\0float\0double\0_Bool\0short\0struct\0union\0typedef\0default\0enum\0sizeof\0define\0include\0ifdef\0ifndef\0elif\0endif\0defined\0undef\0error\0line\0__LINE__\0__FILE__\0__DATE__\0__TIME__\0__VA_ARGS__\0__func__\0main\0";
   while (*p) {
       r = p;
       while (*r++);
       tok_alloc(p, r - p - 1);
       p = r;
   }

   /* standard defines */
   define_symbol("__STDC__");
   define_symbol("__i386__");
   /* tiny C specific defines */
   define_symbol("__TINYC__");

//    glo = (int)mmap(NULL, DATA_SIZE,
//                PROT_READ | PROT_WRITE,
//                MAP_PRIVATE | MAP_ANONYMOUS,
//                -1, 0);
//    glo_base=glo;
//    printf("glo: %x %x\n",glo,glo_base);
//    memset((void *)glo, 0, DATA_SIZE);
//    prog = (int)mmap(NULL, TEXT_SIZE,
//                PROT_EXEC | PROT_READ | PROT_WRITE,
//                MAP_PRIVATE | MAP_ANONYMOUS,
//                -1, 0);
//    ind = prog;
//    printf("prog: %x \n",prog);
//
//    optind = 1;
//    outfile = NULL;
//int loader=0;
//    while (1) {
//printf("argc %d\n",argc);
//        if (optind >= argc) {
//        return show_help();
//        }
//        r = argv[optind];
//        if (r[0] != '-')
//            break;
//        optind++;
//        if (r[1] == 'I') {
//            if (nb_include_paths >= INCLUDE_PATHS_MAX)
//                error("too many include paths");
//            include_paths[nb_include_paths++] = r + 2;
//        } else if (r[1] == 'D') {
//            define_symbol(r + 2);
//        } else if (r[1] == 'i') {
//            if (optind >= argc)
//                return show_help();
//            tcc_compile_file(argv[optind++]);
//        } else if (r[1] == 'r') {
//            reloc=1;
//        } else if (r[1] == 'R') {
//            t=(int (*)())load_obj();
//            loader=1;
//        } else {
//            fprintf(stderr, "invalid option -- '%s'\n", r);
//            exit(1);
//        }
//    }
//
//if(loader){
//  printf("running loader\n");
//  return (*t)(argc - optind, argv + optind);
//}
//if(reloc){
//global_relocs=(int)malloc(64*1024);
//global_relocs_base=global_relocs;
//
//printf("global_relocs %d\n",global_relocs);
//
//global_relocs_table=(int)malloc(64*1024);
//global_relocs_table_base=global_relocs_table;
//
//relocs=(int)malloc(64*1024);
//relocs_base=relocs;
//
//};
//
//    tcc_compile_file(argv[optind]);
//    puts("tcc 1_7 compile done");
//
//    resolve_extern_syms();
//    s = sym_find1(&extern_stack, TOK_MAIN);
//    if (!s || (s->t & VT_FORWARD))
//        error("main() not defined");
//
//if(reloc){
//  gen_obj(s->c);
//}
//
//    t = (int (*)())s->c;
//    return (*t)(argc - optind, argv + optind);
    return 0;
}
