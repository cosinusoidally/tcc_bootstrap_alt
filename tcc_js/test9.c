#include "test9.h"

/* token symbol management */
typedef struct TokenSym {
    struct TokenSym *hash_next;
    int tok; /* token number */
    int len;
    char str[1];
} TokenSym;

/* symbol management */
typedef struct Sym {
    int v;    /* symbol token */
    int t;    /* associated type */
    int c;    /* associated number */
    struct Sym *next; /* next related symbol */
    struct Sym *prev; /* prev symbol in stack */
    struct Sym *hash_next; /* next symbol in hash table */
} Sym;

#define INCLUDE_PATHS_MAX   32

#define TOK_HASH_SIZE       521
#define TOK_ALLOC_INCR      256 /* must be a power of two */

/* all identificators and strings have token above that */
#define TOK_IDENT 256

char *include_paths[INCLUDE_PATHS_MAX];
int nb_include_paths;

int tok_ident;
TokenSym **table_ident;
TokenSym *hash_ident[TOK_HASH_SIZE];

#define SYM_FIRST_ANOM (1 << (31 - VT_STRUCT_SHIFT)) /* first anonymous sym */

#define VT_STRUCT_SHIFT 16   /* structure/enum name shift (16 bits left) */

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

//    /* standard defines */
//    define_symbol("__STDC__");
//    define_symbol("__i386__");
//    /* tiny C specific defines */
//    define_symbol("__TINYC__");
//    
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
