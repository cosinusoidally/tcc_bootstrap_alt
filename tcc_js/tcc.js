print("tcc.js start");
load("support.js");
load("metadata.js");
load("parser_init.js");

/* js_to_c compiled version can't parse this number */
/* var VT_TYPE=0xFFFFFE00; */
/* note the space around = is important here as cc_x86 and M2 cannot parse
   without */
var VT_TYPE = -512;

/*
 *  TCC - Tiny C Compiler
 * 
 *  Copyright (c) 2001 Fabrice Bellard
 *  Modifications Copyright (c) 2023 Liam Wilson
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

var esc_f=0xC;

var TEXT_SIZE = (256*1024);
var DATA_SIZE = (256*1024);

var INCLUDE_STACK_SIZE = 32;
var VSTACK_SIZE = 64;
var STRING_MAX_SIZE = 1024;
var INCLUDE_PATHS_MAX = 32;

var TOK_HASH_SIZE = 521;
var TOK_ALLOC_INCR = 256;  /* must be a power of two */
// #define SYM_HASH_SIZE       263
var SYM_HASH_SIZE = 263;

/* token symbol management */
// typedef struct TokenSym {
//     struct TokenSym *hash_next;
var TokenSym_hash_next_o=0;
//     int tok; /* token number */
var TokenSym_tok_o=4;
//     int len;
var TokenSym_len_o=8;
//     char str[1];
var TokenSym_str_o=12;
// } TokenSym;

// FIXME ljw should probably make explicit CValue_o.. values but I haven't
// bothered since they will all be offset 0 since this is a single union
/* constant value */
// typedef union CValue {
//     int i;
//     unsigned int ui;
//     unsigned int ul; /* address (should be unsigned long on 64 bit cpu) */
//     struct TokenSym *ts;
//     int tab[1];
//     struct Sym *sym;
// } CValue;

/* value on stack */
// typedef struct SValue {
//     int t;
var SValue_t_o=0;
//     CValue c;
var SValue_c_o=4;
// } SValue;

/* symbol management */
// typedef struct Sym {
//     int v;    /* symbol token */
var Sym_v_o=0;
//     int t;    /* associated type */
var Sym_t_o=4;
//     int c;    /* associated number */
var Sym_c_o=8;
//     struct Sym *next; /* next related symbol */
var Sym_next_o=12;
//     struct Sym *prev; /* prev symbol in stack */
var Sym_prev_o=16;
//     struct Sym *hash_next; /* next symbol in hash table */
var Sym_hash_next_o=20;
// } Sym;

// typedef struct SymStack {
//   struct Sym *top;
var SymStack_top_o=0;
//   struct Sym *hash[SYM_HASH_SIZE];
var SymStack_hash_o=4;
// } SymStack;

/* relocation entry (currently only used for functions or variables */
// typedef struct Reloc {
//     int type;            /* type of relocation */
var Reloc_type_o=0;
//     int addr;            /* address of relocation */
var Reloc_addr_o=4;
//     struct Reloc *next;  /* next relocation */
var Reloc_next_o=8;
// } Reloc;

var RELOC_ADDR32 = 1; /* 32 bits relocation */
var RELOC_REL32 = 2;  /* 32 bits relative relocation */

// #define SYM_STRUCT     0x40000000 /* struct/union/enum symbol space */
// #define SYM_FIELD      0x20000000 /* struct/union field symbol space */
// #define SYM_FIRST_ANOM (1 << (31 - VT_STRUCT_SHIFT)) /* first anonymous sym */

var FUNC_NEW = 1;           /* ansi function prototype */
var FUNC_OLD = 2;           /* old function prototype */
var FUNC_ELLIPSIS = 3;      /* ansi function prototype with ... */

/* field 'Sym.t' for macros */
var MACRO_OBJ = 0;          /* object like macro */
// #define MACRO_FUNC     1 /* function like macro */

/* type_decl() types */
var TYPE_ABSTRACT = 1;      /* type without variable */
var TYPE_DIRECT = 2;        /* type with variable */

// typedef struct {
//     FILE *file;
var IncludeFile_file_o=0;
//     char *filename;
var IncludeFile_filename_o=4;
//     int line_num;
var IncludeFile_line_num_o=8;
// } IncludeFile;

var special=0;

/* parser */
var file;
var line_num=0;

var ch;
var ch1;
var tok;
var tok1;

// CValue tokc, tok1c;
var tokc=v_malloc(CValue_size);
var tok1c=v_malloc(CValue_size);

/* loc : local variable index
   glo : global variable index
   ind : output code ptr
   rsym: return symbol
   prog: output code
   anon_sym: anonymous symbol index
*/
var rsym;
var anon_sym;

var prog;
var ind;
var loc;
var glo;
var glo_base;
var const_wanted=0;

/* true if compound literals must be allocated
   globally (used during initializers parsing */
var global_expr=0;

/* current function return type (used by return instruction) */
var func_vt;
var func_vc;

var tok_ident;

// TokenSym **table_ident;
var table_ident;

// TokenSym *hash_ident[TOK_HASH_SIZE];
var hash_ident=v_malloc(TOK_HASH_SIZE*4);

// char token_buf[STRING_MAX_SIZE + 1];
var token_buf=v_malloc(STRING_MAX_SIZE + 1);

// char *filename, *funcname;
var filename;
var funcname;

/* contains global symbols which remain between each translation unit */
// SymStack extern_stack;
// SymStack define_stack, global_stack, local_stack, label_stack;
var define_stack=v_malloc(SymStack_size);
var local_stack=v_malloc(SymStack_size);
var global_stack=v_malloc(SymStack_size);
var extern_stack=v_malloc(SymStack_size);

// SValue vstack[VSTACK_SIZE], *vtop;
var vstack=v_malloc(SValue_size*VSTACK_SIZE);
var vtop;

// int *macro_ptr, *macro_ptr_allocated;
var macro_ptr=v_malloc(4);
var macro_ptr_allocated=0;

// IncludeFile include_stack[INCLUDE_STACK_SIZE], *include_stack_ptr;
var include_stack=v_malloc(IncludeFile_size*INCLUDE_STACK_SIZE);
var include_stack_ptr;

// char *include_paths[INCLUDE_PATHS_MAX];
var include_paths = v_malloc(4*INCLUDE_PATHS_MAX);

var nb_include_paths=0;

/* use GNU C extensions */
var gnu_ext = 0;

/* use Tiny C extensions */
var tcc_ext = 1;

var reloc=0;
// int reloc_global=0;
var reloc_global=0;

var relocs;
var relocs_base;

var global_relocs;
var global_relocs_base;

var global_relocs_table;
var global_relocs_table_base;


// FIXME ljw it's tempting to clean up these VT* and TOK* defines/enums but
// I think it is worth keeping them as they contain useful documentation
/* The current value can be: */
// #define VT_VALMASK 0x000f
var VT_VALMASK = 0x000F;
// #define VT_CONST   0x000a  /* constant in vc 
var VT_CONST = 0x000A;
//                               (must be first non register value) */
// #define VT_LLOCAL  0x000b  /* lvalue, offset on stack */
// #define VT_LOCAL   0x000c  /* offset on stack */
// #define VT_CMP     0x000d  /* the value is stored in processor flags (in vc) */
// #define VT_JMP     0x000e  /* value is the consequence of jmp true */
// #define VT_JMPI    0x000f  /* value is the consequence of jmp false */
// #define VT_LVAL    0x0010  /* var is an lvalue */
var VT_LVAL = 0x0010;
// #define VT_LVALN   -17         /* ~VT_LVAL */
var VT_LVALN = -17;         /* ~VT_LVAL */
// #define VT_FORWARD 0x0020  /* value is forward reference 
//                               (only used for functions) */
// /* storage */
// #define VT_EXTERN  0x00000040  /* extern definition */
// #define VT_STATIC  0x00000080  /* static variable */
// #define VT_TYPEDEF 0x00000100  /* typedef definition */
// 
// /* types */
// #define VT_STRUCT_SHIFT 16   /* structure/enum name shift (16 bits left) */
// 
// #define VT_BTYPE_SHIFT 9
// #define VT_INT        (0 << VT_BTYPE_SHIFT)  /* integer type */
// #define VT_BYTE       (1 << VT_BTYPE_SHIFT)  /* signed byte type */
// #define VT_SHORT      (2 << VT_BTYPE_SHIFT)  /* short type */
// #define VT_VOID       (3 << VT_BTYPE_SHIFT)  /* void type */
// #define VT_PTR        (4 << VT_BTYPE_SHIFT)  /* pointer increment */
// #define VT_ENUM       (5 << VT_BTYPE_SHIFT)  /* enum definition */
// #define VT_FUNC       (6 << VT_BTYPE_SHIFT)  /* function type */
// #define VT_STRUCT     (7 << VT_BTYPE_SHIFT)  /* struct/union definition */
// #define VT_FLOAT      (8 << VT_BTYPE_SHIFT)  /* IEEE float */
// #define VT_DOUBLE     (9 << VT_BTYPE_SHIFT)  /* IEEE double */
// #define VT_LDOUBLE   (10 << VT_BTYPE_SHIFT)  /* IEEE long double */
// #define VT_BOOL      (11 << VT_BTYPE_SHIFT)  /* ISOC99 boolean type */
// #define VT_LLONG     (12 << VT_BTYPE_SHIFT)  /* 64 bit integer */
// #define VT_LONG      (13 << VT_BTYPE_SHIFT)  /* long integer (NEVER
//                                                 USED as type, only
//                                                 during parsing) */
// #define VT_BTYPE      (0xf << VT_BTYPE_SHIFT) /* mask for basic type */
// #define VT_UNSIGNED   (0x10 << VT_BTYPE_SHIFT)  /* unsigned type */
// #define VT_ARRAY      (0x20 << VT_BTYPE_SHIFT)  /* array type (also has VT_PTR) */
// #define VT_BITFIELD   (0x40 << VT_BTYPE_SHIFT)  /* bitfield modifier */
// 
// #define VT_TYPE    0xfffffe00  /* type mask */
// 
// /* token values */
// 
// /* warning: the following compare tokens depend on i386 asm code */
// #define TOK_ULT 0x92
// #define TOK_UGE 0x93
// #define TOK_EQ  0x94
// #define TOK_NE  0x95
// #define TOK_ULE 0x96
// #define TOK_UGT 0x97
// #define TOK_LT  0x9c
// #define TOK_GE  0x9d
// #define TOK_LE  0x9e
// #define TOK_GT  0x9f
// 
// #define TOK_LAND  0xa0
// #define TOK_LOR   0xa1
// 
// #define TOK_DEC   0xa2
// #define TOK_MID   0xa3 /* inc/dec, to void constant */
// #define TOK_INC   0xa4
// #define TOK_ARROW 0xa7 
// #define TOK_DOTS  0xa8 /* three dots */
// #define TOK_SHR   0xa9 /* unsigned shift right */
// #define TOK_UDIV  0xb0 /* unsigned division */
// #define TOK_UMOD  0xb1 /* unsigned modulo */
// #define TOK_PDIV  0xb2 /* fast division with undefined rounding for pointers */
// #define TOK_NUM   0xb3 /* number in tokc */
// #define TOK_CCHAR 0xb4 /* char constant in tokc */
// #define TOK_STR   0xb5 /* pointer to string in tokc */
// #define TOK_TWOSHARPS 0xb6 /* ## preprocessing token */
// #define TOK_LCHAR 0xb7
// #define TOK_LSTR  0xb8
// #define TOK_CFLOAT   0xb9 /* float constant */
// #define TOK_CDOUBLE  0xc0 /* double constant */
// #define TOK_CLDOUBLE 0xc1 /* long double constant */
// 
//  #define TOK_SHL   0x01 /* shift left */
// #define TOK_SAR   0x02 /* signed shift right */
//   
// /* assignement operators : normal operator or 0x80 */
// #define TOK_A_MOD 0xa5
// #define TOK_A_AND 0xa6
// #define TOK_A_MUL 0xaa
// #define TOK_A_ADD 0xab
// #define TOK_A_SUB 0xad
// #define TOK_A_DIV 0xaf
// #define TOK_A_XOR 0xde
// #define TOK_A_OR  0xfc
// #define TOK_A_SHL 0x81
// #define TOK_A_SAR 0x82
// 
// /* all identificators and strings have token above that */
// #define TOK_IDENT 256
// 
// enum {
//     TOK_INT = TOK_IDENT,
//     TOK_VOID,
//     TOK_CHAR,
//     TOK_IF,
//     TOK_ELSE,
//     TOK_WHILE,
//     TOK_BREAK,
//     TOK_RETURN,
//     TOK_FOR,
//     TOK_EXTERN,
//     TOK_STATIC,
//     TOK_UNSIGNED,
//     TOK_GOTO,
//     TOK_DO,
//     TOK_CONTINUE,
//     TOK_SWITCH,
//     TOK_CASE,
// 
//     /* ignored types Must have contiguous values */
//     TOK_CONST,
//     TOK_VOLATILE,
//     TOK_LONG,
//     TOK_REGISTER,
//     TOK_SIGNED,
//     TOK_AUTO,
//     TOK_INLINE,
//     TOK_RESTRICT,
// 
//     /* unsupported type */
//     TOK_FLOAT,
//     TOK_DOUBLE,
//     TOK_BOOL,
// 
//     TOK_SHORT,
//     TOK_STRUCT,
//     TOK_UNION,
//     TOK_TYPEDEF,
//     TOK_DEFAULT,
//     TOK_ENUM,
//     TOK_SIZEOF,
// 
//     /* preprocessor only */
//     TOK_UIDENT, /* first "user" ident (not keyword) */
//     TOK_DEFINE = TOK_UIDENT,
//     TOK_INCLUDE,
//     TOK_IFDEF,
//     TOK_IFNDEF,
//     TOK_ELIF,
//     TOK_ENDIF,
//     TOK_DEFINED,
//     TOK_UNDEF,
//     TOK_ERROR,
//     TOK_LINE,
//     TOK___LINE__,
//     TOK___FILE__,
//     TOK___DATE__,
//     TOK___TIME__,
//     TOK___VA_ARGS__,
// 
//     /* special identifiers */
//     TOK___FUNC__,
//     TOK_MAIN,
// };

// #include "i386-gen.c"
load("i386-gen.js");

// static inline int isid(int c)
function isid(c) {
    return (c >= mk_char('a') && c <= mk_char('z')) ||
        (c >= mk_char('A') && c <= mk_char('Z')) ||
        c == mk_char('_');
}

// static inline int isnum(int c)
function isnum(c) {
    return c >= mk_char('0') & c <= mk_char('9');
}

// static inline int toup(int c)
// {
//     if (ch >= 'a' && ch <= 'z')
//         return ch - 'a' + 'A';
//     else
//         return ch;
// }

// void printline(void)
// {
//     IncludeFile *f;
//     for(f = include_stack; f < include_stack_ptr; f++)
//         fprintf(stderr, "In file included from %s:%d:\n", 
//                 f->filename, f->line_num);
//     fprintf(stderr, "%s:%d: ", filename, line_num);
// }

// void printline2(void)
// {
//     IncludeFile *f;
//     for(f = include_stack; f < include_stack_ptr; f++)
//         fprintf(stdout, "In file included from %s:%d:\n", 
//                 f->filename, f->line_num);
//     fprintf(stdout, "%s:%d: ", filename, line_num);
// }

// void error(const char *fmt, ...)
// function error(fmt) {
// err();
//     va_list ap;
// //    va_start(ap, fmt);
//     ap = ((char *)&(fmt)) + sizeof(int);
//     printline();
//     vfprintf(stderr, fmt, ap);
//     fprintf(stderr, "\n");
//     exit(1);
// //    va_end(ap);
// }

// void expect(const char *msg)
// function expect(msg) {
// err();
//     error("%s expected", msg);
// }

// void warning(const char *msg)
//  function warning(msg) {
// err();
//     printline();
//     fprintf(stderr, "warning: %s\n", msg);
// }

// void skip(int c)
function skip(c) {
    if (tok != c) {
// FIXME ljw remove this debug logging
//        puts(heap+token_buf);
//        puts_num(c);
//        puts_num(tok);
        err();
//        error("'%c' expected", c);
    }
    next();
}

// void test_lvalue(void)
function test_lvalue() {
    if ((ri32(vtop+SValue_t_o) & VT_LVAL) == 0)
        expect("lvalue");
}

function dummy(){
  return;
}

// TokenSym *tok_alloc(char *str, int len)
function tok_alloc(str, len) {
    var ts;
    var pts;
    var ptable;
    var h;
    var i;
if(line_num == 5){ dummy();}
    enter();
    print("tok_alloc str: "+to_hex(str)+" len: "+len+ " str contents: "+ mk_js_string_len(str,len));
//     TokenSym *ts, **pts, **ptable;
    ts = v_alloca(4);
    pts = v_alloca(4);
    ptable =v_alloca(4);

    if (len <= 0) {
        len = v_strlen(str);
    print("tok_alloc str: "+to_hex(str)+" len: "+len+ " str contents: "+ mk_js_string_len(str,len));
    }
    // FIXME ljw remove verbose debug logging
    print("len: "+len);
    h = 1;
    for(i=0;i<len;i=i+1) {
        h = ((h << 8) | (ri8(str+i) & 0xFF)) % TOK_HASH_SIZE;
    }

    // FIXME ljw this is verbose debug output
    print("h: "+h);
    if(h==28){
      print("int?");
    }
    print("hash_ident: "+to_hex(hash_ident));
    wi32(pts, hash_ident+(h*4));
    print("pts: "+to_hex(pts));
    while (1) {
        ts = ri32(ri32(pts));
        print("ts: "+ts); /* debug logging */
        if (ts == 0) {
            break;
        }
        print("len: "+len+" ts-table_ident:"+(ts-table_ident)); /* dbg log */
        if ((ri32(ts+TokenSym_len_o) == len)) {
            if( v_memcmp(ts+TokenSym_str_o, str, len) == 0) {
                return leave(ts);
            }
        }
        wi32(pts, ts+TokenSym_hash_next_o);
    }

    if (tok_ident >= SYM_FIRST_ANOM) {
        error("memory full");
    }

    /* expand token table if needed */
    i = tok_ident - TOK_IDENT;
    if ((i % TOK_ALLOC_INCR) == 0) {
        wi32(ptable, v_realloc(table_ident, (i + TOK_ALLOC_INCR) * 4));
        if (ptable == 0) {
            error("memory full");
        }
        table_ident = ri32(ptable);
    }

    ts = v_malloc(TokenSym_size + len);
     if (ts == 0) {
         error("memory full");
     }
    wi32(table_ident+(i*4), ts);
    wi32(ts+TokenSym_tok_o , tok_ident);
    tok_ident=tok_ident+1;
    wi32(ts+TokenSym_len_o, len);
    wi32(ts+TokenSym_hash_next_o,0);
    v_memcpy(ts+TokenSym_str_o, str, len + 1);
    wi32(ri32(pts),ts);
    return leave(ts);
}

// void add_char(char **pp, int c)
// {
//     char *p;
//     p = *pp;
//     if (c == '\'' || c == '\"' || c == '\\') {
//         /* XXX: could be more precise if char or string */
//         *p++ = '\\';
//     }
//     if (c >= 32 && c <= 126) {
//         *p++ = c;
//     } else {
//         *p++ = '\\';
//         if (c == '\n') {
//             *p++ = 'n';
//         } else {
//             *p++ = '0' + ((c >> 6) & 7);
//             *p++ = '0' + ((c >> 3) & 7);
//             *p++ = '0' + (c & 7);
//         }
//     }
//     *pp = p;
// }

/* XXX: buffer overflow */
// char *get_tok_str(int v, CValue *cv)
function get_tok_str(v, cv) {
    var buf;
//     TokenSym *ts;
    var ts;
    var p;
    var i;
    enter();
//     static char buf[STRING_MAX_SIZE + 1];
// FIXME ljw should this be reserved on the heap?
    buf = v_alloca(STRING_MAX_SIZE + 1);
//     char *p;
    p=v_alloca(4);

    print("v: "+v); /* dbg log */
    if (v == TOK_NUM) {
err();
//         sprintf(buf, "%u", cv->ui);
//         return buf;
//     } else if (v == TOK_CCHAR || v == TOK_LCHAR) {
    } else if (v == TOK_CCHAR || v == TOK_LCHAR) {
err();
//         p = buf;
//         *p++ = '\'';
//         add_char(&p, cv->i);
//         *p++ = '\'';
//         *p = '\0';
//         return buf;
//     } else if (v == TOK_STR || v == TOK_LSTR) {
    } else if (v == TOK_STR || v == TOK_LSTR) {
err();
//         ts = cv->ts;
//         p = buf;
//         *p++ = '\"';
//         for(i=0;i<ts->len;i++)
//             add_char(&p, ts->str[i]);
//         *p++ = '\"';
//         *p = '\0';
//         return buf;
//     } else if (v < TOK_IDENT) {
    } else if (v < TOK_IDENT) {
err();
//         p = buf;
//         *p++ = v;
//         *p = '\0';
//         return buf;
//     } else if (v < tok_ident) {
    } else if (v < tok_ident) {
        return leave(ri32(table_ident+(4*(v - TOK_IDENT)))+TokenSym_str_o);
    } else {
err();
//         /* should never happen */
//         return NULL;
    }
err();
   return leave(0);
}

/* push, without hashing */
// Sym *sym_push2(Sym **ps, int v, int t, int c)
function sym_push2(ps, v, t, c) {
    var s;
    enter();
//     Sym *s;
    s = v_alloca(4);
//     s = v_malloc(sizeof(Sym));
    wi32(s, v_malloc(Sym_size));
    if (ri32(s) == 0){
        error("memory full");
    }
    wi32(ri32(s)+Sym_v_o, v);
    wi32(ri32(s)+Sym_t_o, t);
    wi32(ri32(s)+Sym_c_o, c);
    wi32(ri32(s)+Sym_next_o, 0);
    /* add in stack */
//     s->prev = *ps;
// FIXME ljw is this right?
    wi32(ri32(s)+Sym_prev_o, ri32(ps));
    wi32(ps,ri32(s));
    return leave(ri32(s));
}

/* find a symbol and return its associated structure. 's' is the top
   of the symbol stack */
// Sym *sym_find2(Sym *s, int v)
function sym_find2(s, v) {
    while (s) {
        if (ri32(s+Sym_v_o) == v)
            return s;
        s = ri32(s+Sym_prev_o);
    }
    return NULL;
}

// unsigned int HASH_SYM(int v) {
function HASH_SYM(v) {
// FIXME ljw is unsigned needed?
//     return ((unsigned)(v) % SYM_HASH_SIZE);
    return (v % SYM_HASH_SIZE);
}

/* find a symbol and return its associated structure. 'st' is the
   symbol stack */
// Sym *sym_find1(SymStack *st, int v)
function sym_find1(st, v) {
    print("sym_find1: "+v); /* dbg log */
//     Sym *s;
    var s;
    var h;
    var a;

    h=HASH_SYM(v);
    print("sym_find1 hash: "+h); /* dbg log */
    a = st+SymStack_hash_o+(4*h);
    s = ri32(a);
    print("s: "+s); /* dbg log */
     while (s) {
         if (ri32(s+Sym_v_o) == v) {
             return s;
         }
         s = ri32(s+Sym_hash_next_o);
     }
     return 0;
}

// Sym *sym_push1(SymStack *st, int v, int t, int c)
function sym_push1(st, v, t, c) {
print("sym_push1: v: "+v+" t: "+t+" c: "+c);
//     Sym *s, **ps;
    var s;
    var ps;
    var h;
    enter();
    s = sym_push2(st+SymStack_top_o, v, t, c);
    /* add in hash table */
    if (v) {
        h =  HASH_SYM(v);
        ps = st+SymStack_hash_o+(4*h);
        wi32(s+Sym_hash_next_o, ri32(ps));
        wi32(ps,s);
    }
    print("sym_push1: "+JSON.stringify(decode_Sym(s))); /* dbg log */
    return leave(s);
}

/* find a symbol in the right symbol space */
// Sym *sym_find(int v)
function sym_find(v) {
//     Sym *s;
    var s;
    s = sym_find1(local_stack, v);
    if (s == 0) {
        s = sym_find1(global_stack, v);
    }
    return s;
}

/* push a given symbol on the symbol stack */
// Sym *sym_push(int v, int t, int c)
function sym_push(v, t, c) {
    if (ri32(local_stack+SymStack_top_o)){
        return sym_push1(local_stack, v, t, c);
    } else {
        return sym_push1(global_stack, v, t, c);
    }
}

/* pop symbols until top reaches 'b' */
// void sym_pop(SymStack *st, Sym *b)
function sym_pop(st, b) {
//     Sym *s, *ss;
    var s;
    var ss;

    s = ri32(st+SymStack_top_o);
    while(s != b) {
        ss = ri32(s+Sym_prev_o);
        /* free hash table entry, except if symbol was freed (only
           used for #undef symbols) */
        if (ri32(s+Sym_v_o)){
            wi32(st+SymStack_hash_o+(4*(HASH_SYM(ri32(s+Sym_v_o)))), ri32(s+Sym_hash_next_o));
        }
        v_free(s);
        s = ss;
    }
    wi32(st+SymStack_top_o, b);
}

// /* undefined a hashed symbol (used for #undef). Its name is set to
//    zero */
// void sym_undef(SymStack *st, Sym *s)
// {
//     Sym **ss;
//     ss = &st->hash[HASH_SYM(s->v)];
//     while (*ss != NULL) {
//         if (*ss == s)
//             break;
//         ss = &(*ss)->hash_next;
//     }
//     *ss = s->hash_next;
//     s->v = 0;
// }

/* no need to put that inline */
// int handle_eof(void)
function handle_eof() {
    if (include_stack_ptr == include_stack) {
        return -1;
    }
    /* pop include stack */
    v_fclose(file);
    v_free(filename);
    include_stack_ptr=include_stack_ptr-IncludeFile_size;
    file = ri32(include_stack_ptr+IncludeFile_file_o);
    filename = ri32(include_stack_ptr+IncludeFile_filename_o);
    line_num = ri32(include_stack_ptr+IncludeFile_line_num_o);
    return 0;
}

/* read next char from current input file */
// static inline void inp(void)
function inp(){
    var redo;
    redo = 1;
    while(redo){
        redo=0;
        /* faster than fgetc */
        ch1 = v_getc_unlocked(file);
        if (ch1 == -1) {
            if (handle_eof() < 0)
                return;
            else
                redo=1;
        }
    }
//     printf("%c",ch1);
    print(String.fromCharCode(ch1)); /* dbg log */
    if (ch1 == mk_char('\n')){
        line_num=line_num+1;
    }
    //    printf("ch1=%c 0x%x\n", ch1, ch1);
}

/* input with '\\n' handling */
// static inline void minp(void)
function minp() {
    var redo;
    redo = 1;
    while(redo){
        redo=0;
        ch = ch1;
        inp();
        if (ch == mk_char('\\') && ch1 == mk_char('\n')) {
            inp();
            redo=1;
        }
    }
    // printf("ch=%c 0x%x\n", ch, ch);
}

/* same as minp, but also skip comments */
// void cinp(void)
function cinp() {
    var c;

    if (ch1 == mk_char('/')) {
        inp();
        if (ch1 == mk_char('/')) {
            /* single line C++ comments */
            inp();
            while (ch1 != mk_char('\n') && ch1 != -1)
                inp();
            inp();
            ch = mk_char(' '); /* return space */
        } else if (ch1 == mk_char('*')) {
            /* C comments */
            inp();
            while (ch1 != -1) {
                c = ch1;
                inp();
            if (c == mk_char('*') && ch1 == mk_char('/')) {
                inp();
                ch = mk_char(' '); /* return space */
                break;
            }
            }
        } else {
            ch = mk_char('/');
        }
    } else {
        minp();
    }
}

// void skip_spaces(void)
function skip_spaces() {
    while (ch == mk_char(' ') || ch == mk_char('\t'))
         cinp();
}

/* return the number of additionnal 'ints' necessary to store the
   token */
// static inline int tok_ext_size(int t)
function tok_ext_size(t) {
//  4 bytes
    if(t==TOK_NUM |
       t==TOK_CCHAR|
       t==TOK_STR|
       t==TOK_LSTR|
       t==TOK_CFLOAT){
        return 1;
    } else if(t==TOK_CDOUBLE){
        return 2;
    } else {
        return 0;
    }
}

// void tok_add(int **tok_str, int *tok_len, int t)
function tok_add(tok_str, tok_len, t) {
//     int len, *str;
    var len;
    var str;
    enter();
    str = v_alloca(4);
    len=ri32(tok_len);
    wi32(str, ri32(tok_str));
    if ((len & 63) == 0) {
        wi32(str, v_realloc(ri32(str), (len + 64) * 4));
        if (ri32(str) == 0) {
            return leave(0);
        }
        wi32(tok_str, ri32(str));
    }
    wi32(ri32(str)+((len)*4), t);
    len=len+1;
    wi32(tok_len, len);
    return leave(0);
}

// void tok_add2(int **tok_str, int *tok_len, int t, CValue *cv)
function tok_add2(tok_str, tok_len, t, cv) {
    var n;
    var i;
    enter();

    tok_add(tok_str, tok_len, t);
    n = tok_ext_size(t);
    for(i=0;i<n;i=i+1) {
//         tok_add(tok_str, tok_len, cv->tab[i]);
        tok_add(tok_str, tok_len, ri32(cv+(i*4)));
    }
    return leave(0);
}

/* get a token from an integer array and increment pointer accordingly */
// int tok_get(int **tok_str, CValue *cv)
function tok_get(tok_str, cv) {
//     int *p, t, n, i;
     var p;
     var t;
     var n;
     var i;

    p = ri32(tok_str);
    t = ri32(p);
    p=p+4;
    n = tok_ext_size(t);
    for(i=0;i<n;i=i+1) {
//         cv->tab[i] = *p++;
        wi32(cv+(i*4),ri32(p));
        p=p+4;
    }
    wi32(tok_str, p);
    return t;
}

/* XXX: should be more factorized */
// void define_symbol(char *sym)
function define_symbol(sym) {
//     TokenSym *ts;
    var ts;
    var str;
    var len;
//     CValue cval;
    var cval;
    enter();
//     int *str, len;
    str = v_alloca(4);
    len = v_alloca(4);
    cval = v_alloca(CValue_size);

    ts = tok_alloc(sym, 0);
    wi32(str,0);
    wi32(len,0);
//     cval.i = 1;
    wi32(cval,1);
    tok_add2(str, len, TOK_NUM, cval);
    tok_add(str, len, 0);
    sym_push1(define_stack, ri32(ts+TokenSym_tok_o), MACRO_OBJ, ri32(str));
    return leave(0);
}

// void preprocess(void)
function preprocess() {
//     int size, i, c, v, t, *str, len;
    var size;
    var i;
    var c;
    var v;
    var t;
    var str;
    var len;
//     int found=0;
    var found;
//     char buf[1024], *q, *p;
    var buf_size;
    var buf;
    var q;
    var p;
//     char buf1[1024];
    var buf1;
//     FILE *f;
    var f;
//     Sym **ps, *first, *s;
    var ps;
    var first;
    var s;

    str = v_alloca(4);
    len = v_alloca(4);
    found = 0;
    buf_size = 1024;
    buf = v_alloca(buf_size);
    buf1 = v_alloca(buf_size);

    cinp();
    next_nomacro();
    if (tok == TOK_DEFINE) {
        next_nomacro();
        v = tok;
        /* XXX: should check if same macro (ANSI) */
        first = NULL;
        t = MACRO_OBJ;
        wi32(str, NULL);
        wi32(len, 0);
        while (1) {
            skip_spaces();
            if (ch == mk_char('\n') || ch == -1)
                break;
            next_nomacro();
            tok_add2(str, len, tok, tokc);
        }
        tok_add(str, len, 0);
        s = sym_push1(define_stack, v, t, ri32(str));
        wi32(s+Sym_next_o,first);
    } else if (tok == TOK_UNDEF) {
err();
//         next_nomacro();
//         s = sym_find1(&define_stack, tok);
//         /* undefine symbol by putting an invalid name */
//         if (s)
//             sym_undef(&define_stack, s);
    } else if (tok == TOK_INCLUDE) {
// FIXME ljw TOK_INCLUDE handling current has some porting bugs
        skip_spaces();
        if ((ch == mk_char('<')) || (ch == mk_char('\"'))) {
            c = mk_char('>');
        if (ch == mk_char('\"'))
            c = ch;
            minp();
            q = buf;
            while (ch != c && ch != mk_char('\n') && ch != -1) {
                if ((q - buf) < buf_size - 1)
                    wi8(q, ch);
                    q=q+1;
                minp();
            }
            wi8(q, 0);
        } else {
err();
//             next();
//             if (tok != TOK_STR)
//                 error("#include syntax error");
//             /* XXX: buffer overflow */
//             v_strcpy(buf, get_tok_str(tok, &tokc));
//             c = '\"';
        }
        /* eat all spaces and comments after include */
        /* XXX: slightly incorrect */
        while (ch1 != mk_char('\n') && ch1 != -1)
            inp();

        if (include_stack_ptr >= include_stack + INCLUDE_STACK_SIZE)
            error("memory full");
        if (c == mk_char('\"')) {
            /* first search in current dir if "header.h" */
            /* XXX: buffer overflow */
            size = 0;
            p = v_strrchr(filename, mk_c_string("/"));
            if (p) 
                size = p + 1 - filename;
            v_memcpy(buf1, filename, size);
            wi8(buf1+size, 0);
            v_strcat(buf1, buf);
            print("buf1: "+mk_js_string(buf1)+" buf: "+mk_js_string(buf)); /* dbg log */
            f = v_fopen(buf1, mk_c_string("r"));
            if (f)
                found=1;
        }
        /* now search in standard include path */
        if(found == 0){
            for(i=nb_include_paths - 1;i>=0;i=i-1) {
                v_strcpy(buf1, ri32(include_paths+(4*i)));
                v_strcat(buf1, mk_c_string("/"));
                v_strcat(buf1, buf);
                f = v_fopen(buf1, mk_c_string("r"));
                if (f)
                    found=1;
            }
        }
        if(found == 0){
            print("include file "+mk_js_string(buf1)+" not found");
err();
//             f = NULL;
        }
        /* push current file in stack */
        /* XXX: fix current line init */
/* useful for debugging */
/* puts("writing a FILE reference to the heap"); */
        wi32(include_stack_ptr+ IncludeFile_file_o, file);
/* puts("done writing a FILE reference to the heap"); */
        wi32(include_stack_ptr+IncludeFile_filename_o, filename);
        wi32(include_stack_ptr+IncludeFile_line_num_o, line_num);
        include_stack_ptr = include_stack_ptr+IncludeFile_size;
        file = f;
        filename = v_strdup(buf1);
        print("filename: "+mk_js_string(filename)); /* dbg log */
        line_num = 1;
    }
    if (tok == TOK_ERROR) {
err();
//         error("#error");
    }
    /* ignore other preprocess commands or #! for C scripts */
    while (ch != mk_char('\n') && ch != -1)
        cinp();
}

// /* read a number in base b */
// int getn(b)
// {
//     int n, t;
//     n = 0;
//     while (1) {
//         if (ch >= 'a' & ch <= 'f')
//             t = ch - 'a' + 10;
//         else if (ch >= 'A' & ch <= 'F')
//             t = ch - 'A' + 10;
//         else if (isnum(ch))
//             t = ch - '0';
//         else
//             break;
//         if (t < 0 | t >= b)
//             break;
//         n = n * b + t;
//         cinp();
//     }
//     return n;
// }

/* read a character for string or char constant and eval escape codes */
// int getq()
function getq() {
    var c;

    c = ch;
    minp();
    if (c == mk_char('\\')) {
        if (isnum(ch)) {
            /* at most three octal digits */
            c = ch - mk_char('0');
            minp();
            if (isnum(ch)) {
                c = c * 8 + ch - mk_char('0');
                minp();
                if (isnum(ch)) {
                    c = c * 8 + ch - mk_char('0');
                    minp();
                }
            }
            return c;
        } else if (ch == mk_char('x')) {
err();
//             minp();
//             return getn(16);
        } else {
            if (ch == mk_char('a'))
// JS doesn't support the a escape code
                c = 7;
            else if (ch == mk_char('b'))
// cc_x86 doesn't support this escape code
                c = 8;
            else if (ch == mk_char('f'))
// cc_x86 doesn't support this escape code
                c = 0xC;
            else if (ch == mk_char('n'))
                c = mk_char('\n');
            else if (ch == mk_char('r'))
                c = mk_char('\r');
            else if (ch == mk_char('t'))
                c = mk_char('\t');
            else if (ch == mk_char('v'))
// cc_x86 doesn't support this escape code
                c = 0xB;
            else if (ch == mk_char('e') && gnu_ext)
                c = 27;
            else if (ch == mk_char('\'') || ch == mk_char('\"') || ch == mk_char('\\') || ch == mk_char('?'))
                c = ch;
            else
                error("invalid escaped char");
            minp();
        }
    }
    return c;
}

// void parse_number(void)
function parse_number() {
    var b;
    var t;
    var shift;
    var frac_bits;
    var s;
    var exp_val;
//     char *q;
    var q;
//     unsigned int n, n1;
    var n;
    var n1;

    /* number */
    q = token_buf;
    t = ch;
    cinp();
    wi8(q, t);
    q=q+1;
    b = 10;
    if (t == mk_char('.')) {
        /* special dot handling */
        if (ch == mk_char('.')) {
            cinp();
            if (ch != mk_char('.'))
                expect("'.'");
            cinp();
            tok = TOK_DOTS;
        } else {
            /* dots */
            tok = t;
        }
        return;
    } else if (t == mk_char('0')) {
        if (ch == mk_char('x') || ch == mk_char('X')) {
            q=q-1;
            cinp();
            b = 16;
        } else if ((tcc_ext != 0) && (ch == 'b' || ch == 'B')) {
err();
//             q--;
//             cinp();
//             b = 2;
        }
    }
    /* parse all digits. cannot check octal numbers at this stage
       because of floating point constants */
    while (1) {
        if (ch >= mk_char('a') & ch <= mk_char('f'))
            t = ch - mk_char('a') + 10;
        else if (ch >= mk_char('A') & ch <= mk_char('F'))
            t = ch - mk_char('A') + 10;
        else if (isnum(ch))
            t = ch - mk_char('0');
        else
            break;
        if (t >= b)
            break;
        if (q >= token_buf + STRING_MAX_SIZE) {
            error("number too long");
        }
        wi8(q, ch);
        q=q+1;
        cinp();
    }
    /* integer number */
    wi8(q, 0);
    q = token_buf;
    if (b == 10 && ri8(q) == mk_char('0')) {
        b = 8;
        q=q+1;
    }
    n = 0;
    while(1) {
        t = ri8(q);
        q=q+1;
        /* no need for checks except for base 10 / 8 errors */
        if (t == 0) {
            break;
        } else if (t >= mk_char('a')) {
            t = t - mk_char('a') + 10;
        } else if (t >= mk_char('A')) {
            t = t - mk_char('A') + 10;
        } else {
            t = t -  mk_char('0');
            if (t >= b)
                error("invalid digit");
        }
// FIXME ljw is unsigned needed?
        n1 = n;
// FIXME ljw is unsigned needed?
        n = n * b + t;
        /* detect overflow */
        if (n < n1) {
            error("integer constant overflow");
        }
    }
//     tokc.ui = n;
    wi32(tokc, n);
    tok = TOK_NUM;
    /* XXX: add unsigned constant support (ANSI) */
    while (ch == mk_char('L') || ch == mk_char('l') || ch == mk_char('U') || ch == mk_char('u'))
        cinp();
}


/* return next token without macro substitution */
// void next_nomacro1(void)
function next_nomacro1() {
    var b;
//     char *q;
    var q;
//     TokenSym *ts;
    var ts;
    enter();

    /* skip spaces */
    while(1) {
        while (ch == mk_char('\n')) {
            cinp();
            while (ch == mk_char(' ') || ch == 9) {
                cinp();
            }
            if (ch == mk_char('#')) {
                /* preprocessor command if # at start of line after
                   spaces */
                preprocess();
            }
        }
        if (ch != mk_char(' ') && ch != mk_char('\t') && ch != esc_f) {
            break;
        }
        cinp();
    }
    if (isid(ch)) {
        q = token_buf;
        wi8(q, ch);
        q=q+1;
        cinp();
        while (isid(ch) | isnum(ch)) {
            if (q >= token_buf + STRING_MAX_SIZE) {
                error("ident too long");
            }
               wi8(q, ch);
               q=q+1;
            cinp();
        }
        wi8(q, 0);
        ts = tok_alloc(token_buf, q - token_buf);
         tok = ri32(ts+TokenSym_tok_o);
    } else if (isnum(ch) || ch == mk_char('.')) {
        parse_number();
     } else if (ch == mk_char('\'')) {
        tok = TOK_CCHAR;
        minp();
//         tokc.i = getq();
        wi32(tokc, getq());
        if (ch != mk_char('\''))
            expect("\'");
        minp();
    } else if (ch == mk_char('\"')) {
        tok = TOK_STR;
        minp();
        q = token_buf;
        while (ch != mk_char('\"')) {
            b = getq();
            if (ch == -1)
                error("unterminated string");
            if (q >= token_buf + STRING_MAX_SIZE)
                error("string too long");
            wi8(q, b);
            q=q+1;
        }
        wi8(q, 0);
//         tokc.ts = tok_alloc(token_buf, q - token_buf);
        wi32(tokc, tok_alloc(token_buf, q - token_buf));
        minp();
    } else {
        q = get_twochar_str();
        /* two chars */
        tok = ch;
        cinp();
        while (ri8(q)) {
            if (ri8(q) == tok && ri8(q+1) == ch) {
                cinp();
                tok = ri8(q+2) & 0xFF;
                /* three chars tests */
                if (tok == TOK_SHL | tok == TOK_SAR) {
                    if (ch == mk_char('=')) {
                        tok = tok | 0x80;
                        cinp();
                    }
                } else if (tok == TOK_DOTS) {
err();
//                     if (ch != '.')
//                         error("parse error");
//                     cinp();
                }
                return leave(0);
            }
            q = q + 3;
        }
        /* single char substitutions */
        if (tok == mk_char('<')){
            tok = TOK_LT;
        } else if (tok == mk_char('>')) {
            tok = TOK_GT;
        }
    }
    leave(0);
}

/* return next token without macro substitution. Can read input from
   macro_ptr buffer */
// void next_nomacro()
function next_nomacro() {
    enter();
    if (ri32(macro_ptr)) {
        tok = ri32(ri32(macro_ptr));
        if (tok)
            tok = tok_get(macro_ptr, tokc);
    } else {
        next_nomacro1();
    }
    leave(0);
}

// /* substitute args in macro_str and return allocated string */
// int *macro_arg_subst(Sym **nested_list, int *macro_str, Sym *args)
// {
//     int *st, last_tok, t, notfirst, *str, len;
//     Sym *s;
//     TokenSym *ts;
//     CValue cval;
// 
//     str = NULL;
//     len = 0;
//     last_tok = 0;
//     while(1) {
//         t = tok_get(&macro_str, &cval);
//         if (!t)
//             break;
//         if (t == '#') {
//             /* stringize */
//             t = tok_get(&macro_str, &cval);
//             if (!t)
//                 break;
//             s = sym_find2(args, t);
//             if (s) {
//                 token_buf[0] = '\0';
//                 st = (int *)s->c;
//                 /* XXX: buffer overflow */
//                 notfirst = 0;
//                 while (*st) {
//                     if (notfirst)
//                         v_strcat(token_buf, " ");
//                     t = tok_get(&st, &cval);
//                     v_strcat(token_buf, get_tok_str(t, &cval));
//                     notfirst = 1;
//                 }
//                 /* add string */
//                 ts = tok_alloc(token_buf, 0);
//                 cval.ts = ts;
//                 tok_add2(&str, &len, TOK_STR, &cval);
//             } else {
//                 tok_add2(&str, &len, t, &cval);
//             }
//         } else if (t >= TOK_IDENT) {
//             s = sym_find2(args, t);
//             if (s) {
//                 st = (int *)s->c;
//                 /* if '##' is present before or after , no arg substitution */
//                 if (*macro_str == TOK_TWOSHARPS || last_tok == TOK_TWOSHARPS) {
//                     while (*st)
//                         tok_add(&str, &len, *st++);
//                 } else {
//                     macro_subst(&str, &len, nested_list, st);
//                 }
//             } else {
//                 tok_add(&str, &len, t);
//             }
//         } else {
//             tok_add2(&str, &len, t, &cval);
//         }
//         last_tok = t;
//     }
//     tok_add(&str, &len, 0);
//     return str;
// }

/* handle the '##' operator LJW HACK DELETED CODE*/
// int *macro_twosharps(int *macro_str)
function macro_twosharps(macro_str) {
//     TokenSym *ts;
    var ts;
//     int *macro_str1, macro_str1_len, *macro_ptr1;
    var macro_str1;
    var macro_str1_len;
    var macro_ptr1;
    var t;
//     char *p;
    var p;
//     CValue cval;
    var cval;
enter();
    macro_str1 = v_alloca(4);
    macro_str1_len = v_alloca(4);
    macro_ptr1 = v_alloca(4);

    wi32(macro_str1, NULL);
    wi32(macro_str1_len, 0);
    tok = 0;
    while (1) {
        next_nomacro();
        if (tok == 0)
            break;
        tok_add2(macro_str1, macro_str1_len, tok, tokc);
    }
    tok_add(macro_str1, macro_str1_len, 0);
    return leave(ri32(macro_str1));
}

/* do macro substitution of macro_str and add result to
   (tok_str,tok_len). If macro_str is NULL, then input stream token is
   substituted. 'nested_list' is the list of all macros we got inside
   to avoid recursing. */
// void macro_subst(int **tok_str, int *tok_len, 
//                  Sym **nested_list, int *macro_str)
function macro_subst(tok_str, tok_len, nested_list, macro_str) {
//     Sym *s, *args, *sa, *sa1;
    var s;
    var args;
    var sa;
    var sa1;
//     int *str, parlevel, len, *mstr, t, *saved_macro_ptr;
    var str;
    var parlevel;
    var len;
    var mstr;
    var t;
    var saved_macro_ptr;
//     int mstr_allocated, *macro_str1;
    var mstr_allocated;
    var macro_str1;
    var no_subst;
//     CValue cval;
    var cval;
    enter();
    s = v_alloca(4);
    args = v_alloca(4);
    sa = v_alloca(4);
    sa1 = v_alloca(4);
    str = v_alloca(4);
    parlevel = v_alloca(4);
    len = v_alloca(4);
    mstr = v_alloca(4);
    t = v_alloca(4);
    mstr_allocated = v_alloca(4);
    cval = v_alloca(CValue_size);

    saved_macro_ptr = ri32(macro_ptr);
    wi32(macro_ptr, macro_str);
    macro_str1 = NULL;
    if (macro_str) {
        /* first scan for '##' operator handling */
        macro_str1 = macro_twosharps(macro_str);
        wi32(macro_ptr, macro_str1);
    }

    while (1) {
        no_subst=0;
        next_nomacro();
         if (tok == 0) {
             break;
         }
         if ((s = sym_find1(define_stack, tok)) != NULL) {
            /* if symbol is a macro, prepare substitution */
            /* if nested substitution, do nothing */
            if (sym_find2(ri32(nested_list), tok)) {
err();
//                 no_subst=1;
                no_subst=1;
            }
            if(no_subst == 0){
// FIXME ljw might not be right
                mstr = ri32(s+Sym_c_o);
                mstr_allocated = 0;
                sym_push2(nested_list, ri32(s+Sym_v_o), 0, 0);
                macro_subst(tok_str, tok_len, nested_list, mstr);
                /* pop nested defined symbol */
                sa1 = ri32(nested_list);
                wi32(nested_list, ri32(sa1+Sym_prev_o));
                v_free(sa1);
                if (mstr_allocated)
                v_free(mstr);
            }
         } else {
             no_subst=1;
         }
         if (no_subst) {
            /* no need to add if reading input stream */
             if (macro_str == 0) 
                 return leave(0);
             tok_add2(tok_str, tok_len, tok, tokc);
         }
        /* only replace one macro while parsing input stream */
         if (macro_str == 0)
             return leave(0);
     }
    wi32(macro_ptr, saved_macro_ptr);
// FIXME ljw should free
//     if (macro_str1)
//         v_free(macro_str1);
    leave(0);
}

/* return next token with macro substitution */
// void next(void)
function next() {
//     int len, *ptr;
    var len;
    var ptr;
    var redo;
    var cont;
//     Sym *nested_list;
    var nested_list;

    enter();

    len = v_alloca(4);
    ptr = v_alloca(4);
    redo = 1;
    nested_list = v_alloca(4);

    /* special 'ungettok' case for label parsing */
    if (tok1) {
err();
//         tok = tok1;
//         tokc = tok1c;
//         tok1 = 0;
    } else {
    /* cc_x86 doesn't support continue so we need this hack */
    cont=1;
    while(cont) {
    cont=0;
    while(redo){
        redo=0;
        if (ri32(macro_ptr) == 0) {
            /* if not reading from macro substuted string, then try to substitute */
            wi32(len, 0);
            wi32(ptr, NULL);
            wi32(nested_list, NULL);
            macro_subst(ptr, len, nested_list, NULL);
             if (ri32(ptr)) {
                tok_add(ptr, len, 0);
                wi32(macro_ptr, ri32(ptr));
                macro_ptr_allocated = ri32(ptr);
                redo=1;
                cont=1;break;
             }
             if (tok == 0) {
                 redo=1;
                 cont=1;break;
              }
        } else {
            next_nomacro();
            if (tok == 0) {
                /* end of macro string: free it */
                v_free(macro_ptr_allocated);
                wi32(macro_ptr, NULL);
                redo=1;
                cont=1;break;
            }
        }
        break;
    }
    }
    }
    leave(0);
}

// void swap(int *p, int *q)
// {
//     int t;
//     t = *p;
//     *p = *q;
//     *q = t;
// }

// void vsetc(int t, CValue *vc)
function vsetc(t, vc) {
    if (vtop >= vstack + VSTACK_SIZE) {
        error("memory full");
    }
    /* cannot let cpu flags if other instruction are generated */
    /* XXX: VT_JMP test too ? */
    if ((ri32(vtop+SValue_t_o) & VT_VALMASK) == VT_CMP) {
        gv();
    }
    vtop=vtop+SValue_size;
    wi32(vtop+SValue_t_o,t);
//    vtop->c = *vc;
   v_memcpy(vtop+SValue_c_o, vc,CValue_size);
}

// void vset(int t, int v)
function vset(t, v) {
    print("t: "+t+" v: "+v); /* dbg log */
//     CValue cval;
    var cval;

    enter();

    cval = v_malloc(CValue_size);
//     cval.i = v;
    wi32(cval, v);
    vsetc(t, cval);
    leave(0);
}

// void vswap(void)
function vswap() {
//     SValue tmp;
    var tmp;
    enter();

    tmp = v_alloca(SValue_size);
//     tmp = vtop[0];
    v_memcpy(tmp, vtop,SValue_size);
//     vtop[0] = vtop[-1];
    v_memcpy(vtop, vtop-SValue_size, SValue_size);
//     vtop[-1] = tmp;
    v_memcpy(vtop-SValue_size, tmp, SValue_size);
    leave(0);
}

// void vdup(void)
function vdup() {
    if (vtop >= vstack + VSTACK_SIZE)
        error("memory full");
//     vtop++;
    vtop=vtop+SValue_size;
//     *vtop = vtop[-1];
    v_memcpy(vtop, vtop-SValue_size,SValue_size);
}

// int save_reg_forced(int r)
function save_reg_forced(r) {
    var i;
    var l;
    var t;
//     SValue *p;
    var p;

    /* store register */
    loc = (loc - 4) & -3;
    store(r, VT_LOCAL, loc);
    l = loc;

    /* modify all stack values */
    for(p=vstack;p<=vtop;p=p+SValue_size) {
        i = ri32(p+SValue_t_o) & VT_VALMASK;
        if (i == r) {
            if (ri32(p+SValue_t_o) & VT_LVAL) {
                t = VT_LLOCAL;
            } else {
                t = VT_LOCAL;
            }
            wi32(p+SValue_t_o, (ri32(p+SValue_t_o) & VT_TYPE) | VT_LVAL | t);
            wi32(p+SValue_c_o, l);
        }
    }
    return l;
}

/* save r to memory. and mark it as being free */
// void save_reg(int r)
function save_reg(r) {
    var i;
    var p;

    /* modify all stack values */
    for(p=vstack;p<=vtop;p=p+SValue_size) {
        i = ri32(p+SValue_t_o) & VT_VALMASK;
        if (i == r) {
            save_reg_forced(r);
            break;
        }
    }
}

/* find a free register of class 'rc'. If none, save one register */
// int get_reg(int rc)
function get_reg(rc) {
    var r;
    var i;
    var notfound;
//     SValue *p;
    var p;

    enter();

    p = v_alloca(SValue_size);

    /* find a free register */
    for(r=0;r<NB_REGS;r=r+1) {
        notfound=0;
        if (ri32(reg_classes+(4*r)) & rc) {
            for(p=vstack;p<=vtop;p=p+SValue_size) {
                i = ri32(p+SValue_t_o) & VT_VALMASK;
                if (i == r)
                    notfound=1;
            }
            if(notfound  == 0){
            return leave(r);
            }
        }
    }

    /* no register left : free the first one on the stack (very
       important to start from the bottom to ensure that we don't
       spill registers used in gen_op()) */
    for(p=vstack;p<=vtop;p=p+SValue_size) {
        r = ri32(p+SValue_t_o) & VT_VALMASK;
        if (r < VT_CONST && ((ri32(reg_classes+(4*r)) & rc) != 0)) {
            save_reg(r);
            break;
        }
    }
    return leave(r);
}

// void save_regs()
function save_regs() {
    var r;
//     SValue *p;
    var p;

    for(p=vstack;p<=vtop;p=p+SValue_size) {
        r = ri32(p+SValue_t_o) & VT_VALMASK;
        if (r < VT_CONST) {
            save_reg(r);
        }
    }
}

/* move register 's' to 'r', and flush previous value of r to memory
   if needed */
// void move_reg(int r, int s)
function move_reg(r, s) {
    if (r != s) {
        save_reg(r);
        _load(r, s, 0);
    }
 }

/* convert a (vtop->t, vtop->c) in register. lvalues are converted as
   values. Cannot be used if cannot be converted to register value
   (such as structures). */
// int gv(void)
function gv() {
    var r;
    var bit_pos;
    var bit_size;
    var rc;
    var size;
    var align;
    var i;

    /* NOTE: get_reg can modify vstack[] */
    r = ri32(vtop+SValue_t_o) & VT_VALMASK;
    print("gv r_init: "+r); /* dbg log */
    if (r >= VT_CONST || (ri32(vtop+SValue_t_o) & VT_LVAL)) {
        rc = REG_CLASS_INT;
        r = get_reg(rc);
    }

    print("gv r_ret: "+r); /* dbg log */
// FIXME ljw is unsigned needed?
//     load(r, vtop->t, vtop->c.ul);
    _load(r, ri32(vtop+SValue_t_o), ri32(vtop+SValue_c_o));
    wi32(vtop+SValue_t_o, (ri32(vtop+SValue_t_o) & VT_TYPE) | r);
    return r;
}

/* handle constant optimizations and various machine independant opt */
// void gen_opc(int op)
function gen_opc(op) {
    print("gen_opc: "+op); /* dbg log */
    var fc;
    var c1;
    var c2;
    var n;
    var general_case;
//     SValue *v1, *v2;
    var v1;
    var v2;
    var tmp;

    general_case = 0;
    v1 = vtop - SValue_size;
    v2 = vtop;
    /* currently, we cannot do computations with forward symbols */
    c1 = (ri32(v1+SValue_t_o) & (VT_VALMASK | VT_LVAL | VT_FORWARD)) == VT_CONST;
    c2 = (ri32(v2+SValue_t_o) & (VT_VALMASK | VT_LVAL | VT_FORWARD)) == VT_CONST;
    while(1) {
    if ((c1 != 0) && (c2 != 0)) {
//         fc = v2->c.i;
        fc =ri32( v2+SValue_c_o);

//  NOTE ljw full original switch statement is after this if else chain. When
// porting convert case statements from the switch/case statements
	if(op==mk_char('+')) {
            wi32(v1+SValue_c_o, ri32(v1+SValue_c_o) + fc);
        } else if(op==mk_char('-')) {
            wi32(v1+SValue_c_o, ri32(v1+SValue_c_o) - fc);
        } else if(op==mk_char('^')) {
            wi32(v1+SValue_c_o, ri32(v1+SValue_c_o) ^ fc);
        } else if(op==mk_char('|')) {
            wi32(v1+SValue_c_o, ri32(v1+SValue_c_o) | fc);
        } else if(op==mk_char('*')) {
            wi32(v1+SValue_c_o, ri32(v1+SValue_c_o) * fc);
        } else if(op==TOK_SHL) {
            wi32(v1+SValue_c_o, ri32(v1+SValue_c_o) << fc);
        } else {
            general_case=1;
        }

//        switch(op) {
//         case '+': v1->c.i += fc; break;
//         case '-': v1->c.i -= fc; break;
//         case '&': v1->c.i &= fc; break;
//         case '^': v1->c.i ^= fc; break;
//         case '|': v1->c.i |= fc; break;
//         case '*': v1->c.i *= fc; break;
//         case TOK_PDIV:
//         case '/': v1->c.i /= fc; break; /* XXX: zero case ? */
//         case '%': v1->c.i %= fc; break; /* XXX: zero case ? */
//         case TOK_UDIV: v1->c.i = (unsigned)v1->c.i / fc; break; /* XXX: zero case ? */
//         case TOK_UMOD: v1->c.i = (unsigned)v1->c.i % fc; break; /* XXX: zero case ? */
//         case TOK_SHL: v1->c.i <<= fc; break;
//         case TOK_SHR: v1->c.i = (unsigned)v1->c.i >> fc; break;
//         case TOK_SAR: v1->c.i >>= fc; break;
//             /* tests */
//         case TOK_ULT: v1->c.i = (unsigned)v1->c.i < (unsigned)fc; break;
//         case TOK_UGE: v1->c.i = (unsigned)v1->c.i >= (unsigned)fc; break;
//         case TOK_EQ: v1->c.i = v1->c.i == fc; break;
//         case TOK_NE: v1->c.i = v1->c.i != fc; break;
//         case TOK_ULE: v1->c.i = (unsigned)v1->c.i <= (unsigned)fc; break;
//         case TOK_UGT: v1->c.i = (unsigned)v1->c.i > (unsigned)fc; break;
//         case TOK_LT: v1->c.i = v1->c.i < fc; break;
//         case TOK_GE: v1->c.i = v1->c.i >= fc; break;
//         case TOK_LE: v1->c.i = v1->c.i <= fc; break;
//         case TOK_GT: v1->c.i = v1->c.i > fc; break;
//             /* logical */
//         case TOK_LAND: v1->c.i = v1->c.i && fc; break;
//         case TOK_LOR: v1->c.i = v1->c.i || fc; break;
//        default:
//            general_case=1;
//        }

        if(general_case){
          print("general gen_opc: "+op);
          err();
          break;
        }
        vtop=vtop-SValue_size;
    } else {
        /* if commutative ops, put c2 as constant */
        if (c1 && (op == mk_char('+') || op == mk_char('&') || op == mk_char('^') || 
                   op == mk_char('|') || op == mk_char('*'))) {
            vswap();
//             swap(&c1, &c2);
// FIXME ljw can't do swap
            tmp=c1;
            c1=c2;
            c2=tmp;
        }
//         fc = vtop->c.i;
        fc = ri32(vtop+SValue_c_o);
        if (c2 && (((op == mk_char('*') || op == mk_char('/') || op == TOK_UDIV || 
                     op == TOK_PDIV) && 
                    fc == 1) ||
                   ((op == mk_char('+') || op == mk_char('-') || op == mk_char('|') || op == mk_char('^') || 
                     op == TOK_SHL || op == TOK_SHR || op == TOK_SAR) && 
                    fc == 0) ||
                   (op == mk_char('&') && 
                    fc == -1))) {
            /* nothing to do */
            vtop=vtop - SValue_size;
        } else if (c2 && (op == mk_char('*') || op == TOK_PDIV || op == TOK_UDIV)) {
            /* try to use shifts instead of muls or divs */
            if (fc > 0 && (fc & (fc - 1)) == 0) {
                n = -1;
                while (fc) {
                    fc = urs(fc,1);
                    n=n+1;
                }
                wi32(vtop+SValue_c_o, n);
                if (op == mk_char('*'))
                    op = TOK_SHL;
                else if (op == TOK_PDIV)
                    op = TOK_SAR;
                else
                    op = TOK_SHR;
            }
            general_case=1;break;
        } else {
        general_case=1;
        }
    }
    break;
    }
    if(general_case){
        /* call low level op generator */
        /* XXX: remove explicit registers */
        gen_opi(op);
    }
}

// int pointed_size(int t)
function pointed_size(t) {
    var tmp;
    enter();
    tmp = v_alloca(4);
    wi32(tmp,t);
//    return type_size(pointed_type(t), &t);
    return leave(type_size(pointed_type(ri32(tmp)), tmp));
}

/* generic gen_op: handles types problems */
// void gen_op(int op)
function gen_op(op) {
    var u;
    var t1;
    var t2;
    var bt1;
    var bt2;
    var t;

//     t1 = vtop[-1].t;
    t1 = ri32(vtop-SValue_size+SValue_t_o);
//     t2 = vtop[0].t;
    t2 = ri32(vtop+SValue_t_o);
    bt1 = t1 & VT_BTYPE;
    bt2 = t2 & VT_BTYPE;

    if (op == mk_char('+') || op == mk_char('-')) {
        if ((t1 & VT_BTYPE) == VT_PTR &&
            (t2 & VT_BTYPE) == VT_PTR) {
            if (op != mk_char('-'))
                error("invalid type");
            /* XXX: check that types are compatible */
            u = pointed_size(t1);
            gen_opc(op);
            /* set to integer type */
            wi32(vtop+SValue_t_o,(ri32(vtop+SValue_t_o) & ~VT_TYPE) | VT_INT); 
            vset(VT_CONST, u);
            gen_op(TOK_PDIV);
        } else if ((t1 & VT_BTYPE) == VT_PTR ||
                   (t2 & VT_BTYPE) == VT_PTR) {
            if ((t2 & VT_BTYPE) == VT_PTR) {
err();
//                 vswap();
//                 swap(&t1, &t2);
            }
            /* stack-4 contains pointer, stack-2 value to add */
//             vset(VT_CONST, pointed_size(vtop[-1].t));
            vset(VT_CONST, pointed_size(ri32(vtop-SValue_size+SValue_t_o)));
            gen_op(mk_char('*'));
            gen_opc(op);
            /* put again type if gen_opc() swaped operands */
            wi32(vtop+SValue_t_o, (ri32(vtop+SValue_t_o) & ~VT_TYPE) | (t1 & VT_TYPE));
        } else {
            gen_opc(op);
        }
    } else {
        /* XXX: test types and compute returned value */
        if ((t1 | t2) & VT_UNSIGNED ||
            (t1 & VT_BTYPE) == VT_PTR ||
            (t2 & VT_BTYPE) == VT_PTR) {
            if (op == TOK_SAR)
                op = TOK_SHR;
            else if (op == mk_char('/'))
                op = TOK_UDIV;
            else if (op == mk_char('%'))
                op = TOK_UMOD;
            else if (op == TOK_LT)
                op = TOK_ULT;
            else if (op == TOK_GT)
                op = TOK_UGT;
            else if (op == TOK_LE)
                op = TOK_ULE;
            else if (op == TOK_GE)
                op = TOK_UGE;
        }
        gen_opc(op);
    }
}

/* cast 'vtop' to 't' type */
// void gen_cast(int t)
function gen_cast(t){
    var r;
    var bits;
    var sbt;
    var dbt;
    var sf;
    var df;
    var c;
    var st1;
    var dt1;

    r = ri32(vtop+SValue_t_o) & VT_VALMASK;
    if ((t & VT_LVAL) == 0) {
        /* if not lvalue, then we convert now */
        dbt = t & VT_BTYPE;
        sbt = ri32(vtop+SValue_t_o) & VT_BTYPE;
        if (sbt != dbt) {
            c = (ri32(vtop+SValue_t_o) & (VT_VALMASK | VT_LVAL | VT_FORWARD)) == VT_CONST;
            if (dbt == VT_BYTE || dbt == VT_SHORT) {
err();
//                 if (dbt == VT_BYTE)
//                     bits = 8;
//                 else
//                     bits = 16;
//                 if (t & VT_UNSIGNED) {
//                     vset(VT_CONST, (1 << bits) - 1);
//                     gen_op('&');
//                 } else {
//                     bits = 32 - bits;
//                     vset(VT_CONST, bits);
//                     gen_op(TOK_SHL);
//                     vset(VT_CONST, bits);
//                     gen_op(TOK_SAR);
//                 }
            }
        }
    }
    wi32(vtop+SValue_t_o,(ri32(vtop+SValue_t_o) & ~VT_TYPE) | t);
}

/* return type size. Put alignment at 'a' */
// int type_size(int t, int *a)
function type_size(t, a) {
//     Sym *s;
    var s;
    var bt;

    bt = t & VT_BTYPE;
    if (bt == VT_STRUCT) {
        /* struct/union */
        s = sym_find((urs(t, VT_STRUCT_SHIFT)) | SYM_STRUCT);
        wi32(a, 4); /* XXX: cannot store it yet. Doing that is safe */
        return ri32(s+Sym_c_o);
    } else if (bt == VT_PTR) {
        if (t & VT_ARRAY) {
            s = sym_find(urs(t, VT_STRUCT_SHIFT));
            return type_size(ri32(s+Sym_t_o), a) * ri32(s+Sym_c_o);
        } else {
            wi32(a, PTR_SIZE);
            return PTR_SIZE;
        }
    } else if (bt == VT_INT || bt == VT_ENUM ) {
        wi32(a, 4);
        return 4;
    } else if (bt == VT_SHORT) {
err();
//         *a = 2;
//         return 2;
    } else {
        /* char, void, function, _Bool */
        wi32(a, 1);
        return 1;
    }
}

/* return the pointed type of t */
// int pointed_type(int t)
function pointed_type(t) {
//     Sym *s;
    var s;
    s = sym_find(urs(t, VT_STRUCT_SHIFT));
    return ri32(s+Sym_t_o) | (t & ~VT_TYPE);
}

// int mk_pointer(int t)
function mk_pointer(t) {
    var p;
    p = anon_sym;
    anon_sym=anon_sym+1;
    sym_push(p, t, -1);
    return VT_PTR | (p << VT_STRUCT_SHIFT) | (t & ~VT_TYPE);
}

// int is_compatible_types(int t1, int t2)
function is_compatible_types(t1, t2) {
//     Sym *s1, *s2;
    var s1;
    var s2;
    var bt1;
    var bt2;

    t1 = t1 & VT_TYPE;
    t2 = t2 & VT_TYPE;
    bt1 = t1 & VT_BTYPE;
    bt2 = t2 & VT_BTYPE;
    if (bt1 == VT_PTR) {
        t1 = pointed_type(t1);
        /* if function, then convert implicitely to function pointer */
        if (bt2 != VT_FUNC) {
            if (bt2 != VT_PTR)
                return 0;
            t2 = pointed_type(t2);
        }
        /* void matches everything */
        t1 = t1 & VT_TYPE;
        t2 = t2 & VT_TYPE;
        if (t1 == VT_VOID || t2 == VT_VOID)
            return 1;
        return is_compatible_types(t1, t2);
    } else if (bt1 == VT_STRUCT) {
        return (t2 == t1);
    } else if (bt1 == VT_FUNC) {
        if (bt2 != VT_FUNC)
            return 0;
        s1 = sym_find(urs(t1, VT_STRUCT_SHIFT));
        s2 = sym_find(urs(t2, VT_STRUCT_SHIFT));
        if (is_compatible_types(ri32(s1+Sym_t_o), ri32(s2+Sym_t_o)) == 0)
            return 0;
        /* XXX: not complete */
        if (ri32(s1+Sym_c_o) == FUNC_OLD || ri32(s2+Sym_c_o) == FUNC_OLD)
            return 1;
err();
//         if (s1->c != s2->c)
//             return 0;
//         while (s1 != NULL) {
//             if (s2 == NULL)
//                 return 0;
//             if (!is_compatible_types(s1->t, s2->t))
//                 return 0;
//             s1 = s1->next;
//             s2 = s2->next;
//         }
//         if (s2)
//             return 0;
//         return 1;
    } else {
        /* XXX: not complete */
        return 1;
    }
}

// int check_assign_types(int t1, int t2)
function check_assign_types(t1, t2){
    t1 = t1 & VT_TYPE;
    t2 = t2 & VT_TYPE;
    if ((t1 & VT_BTYPE) == VT_PTR &&
        (t2 & VT_BTYPE) == VT_FUNC) {
err();
//         return is_compatible_types(pointed_type(t1), t2);
    } else {
        return is_compatible_types(t1, t2);
    }
}

// /* print a type. If 'varstr' is not NULL, then the variable is also
//    printed in the type */
// /* XXX: add array and function pointers */
// /* XXX: buffer overflows */
// void type_to_str(char *buf, int buf_size, 
//                  int t, const char *varstr)
// {
//     int bt, v;
//     Sym *s, *sa;
//     char buf1[256];
// 
//     t = t & VT_TYPE;
//     bt = t & VT_BTYPE;
//     buf[0] = '\0';
//     if (t & VT_UNSIGNED)
//         v_strcat(buf, "unsigned ");
//     switch(bt) {
//     case VT_VOID:
//         v_strcat(buf, "void");
//         break;
//     case VT_BYTE:
//         v_strcat(buf, "char");
//         break;
//     case VT_SHORT:
//         v_strcat(buf, "short");
//         break;
//     case VT_INT:
//         v_strcat(buf, "int");
//         break;
//     case VT_ENUM:
//     case VT_STRUCT:
//         if (bt == VT_STRUCT)
//             v_strcat(buf, "struct ");
//         else
//             v_strcat(buf, "enum ");
//         v = (unsigned)t >> VT_STRUCT_SHIFT;
//         if (v >= SYM_FIRST_ANOM)
//             v_strcat(buf, "<anonymous>");
//         else
//             v_strcat(buf, get_tok_str(v, NULL));
//         break;
//     case VT_FUNC:
//         s = sym_find((unsigned)t >> VT_STRUCT_SHIFT);
//         type_to_str(buf, buf_size, s->t, varstr);
//         v_strcat(buf, "(");
//         sa = s->next;
//         while (sa != NULL) {
//             type_to_str(buf1, sizeof(buf1), sa->t, NULL);
//             v_strcat(buf, buf1);
//             sa = sa->next;
//             if (sa)
//                 v_strcat(buf, ", ");
//         }
//         v_strcat(buf, ")");
//         return;
//     case VT_PTR:
//         s = sym_find((unsigned)t >> VT_STRUCT_SHIFT);
//         v_strcpy(buf1, "*");
//         if (varstr)
//             v_strcat(buf1, varstr);
//         type_to_str(buf, buf_size, s->t, buf1);
//         return;
//     }
//     if (varstr) {
//         v_strcat(buf, " ");
//         v_strcat(buf, varstr);
//     }
// }

/* verify type compatibility to store vtop in 'st' type, and generate
   casts if needed */
// void gen_assign_cast(int dt)
function gen_assign_cast(dt) {
    var st;
//     char buf1[256], buf2[256];
    var buf1;
    var buf2;

    enter();

    buf1 = v_alloca(256);
    buf2 = v_alloca(256);
    st = ri32(vtop+SValue_t_o); /* destination type */
    if (check_assign_types(dt, st) == 0) {
err();
//         type_to_str(buf1, sizeof(buf1), st, NULL);
//         type_to_str(buf2, sizeof(buf2), dt, NULL);
//         error("cannot cast '%s' to '%s'", buf1, buf2);
    }
    leave(0);
}

/* store vtop in lvalue pushed on stack */
// void vstore(void)
function vstore() {
    var ft;
    var fc;
    var r;
    var t;
    var size;
    var align;
    var bit_size;
    var bit_pos;
//     GFuncContext gf;
    var gf;

    enter();

    align = v_alloca(4);
    gf = v_alloca(GFuncContext_size);

//     ft = vtop[-1].t;
    ft = ri32(vtop-SValue_size+SValue_t_o);
    gen_assign_cast(ft & VT_TYPE);

    if ((ri32(vtop+SValue_t_o) & VT_BTYPE) == VT_STRUCT) {
        /* if structure, only generate pointer */
        /* structure assignment : generate memcpy */
        /* XXX: optimize if small size */

        vdup();
        gfunc_start(gf);
        /* type size */
        size = type_size(ri32(vtop+SValue_t_o), align);
        vset(VT_CONST, size);
        gfunc_param(gf);
        /* source */
        wi32(vtop+SValue_t_o,ri32(vtop+SValue_t_o) & ~VT_LVAL);
        gfunc_param(gf);
        /* destination */
        vswap();
        wi32(vtop+SValue_t_o,ri32(vtop+SValue_t_o) & ~VT_LVAL);
        gfunc_param(gf);

        save_regs();
// FIXME ljw dummy value
//         vset(VT_CONST, (int)&memcpy);
        vset(VT_CONST, 0x12345678);

        if(reloc){
            special=1;
            //   printf("memcpy function pointer stuff\n");
            print("memcpy function pointer stuff\n"); /* dbg log */
        }

        gfunc_call(gf);

        if(reloc){
            special=0;
        }
        /* leave source on stack */
    } else {
        r = gv();  /* generate value */
//         ft = vtop[-1].t;
        ft = ri32(vtop-SValue_size+SValue_t_o);
//         fc = vtop[-1].c.i;
        fc = ri32(vtop-SValue_size+SValue_c_o);
        /* if lvalue was saved on stack, must read it */
        if ((ft & VT_VALMASK) == VT_LLOCAL) {
            t = get_reg(REG_CLASS_INT);
            _load(t, VT_LOCAL | VT_LVAL, fc);
            ft = (ft & ~VT_VALMASK) | t;
        }

        store(r, ft, fc);
//         vtop--;
        vtop=vtop-SValue_size;
        wi32(vtop+SValue_t_o, (ft & VT_TYPE) | r);
//         vtop->c.i = 0;
        wi32(vtop+SValue_c_o, 0);
    }
    leave(0);
}

/* post defines POST/PRE add. c is the token ++ or -- */
// void inc(int post, int c)
function inc(post, c) {
    var r;
    var r1;

    test_lvalue();
    if (post)
        vdup(); /* room for returned value */
    vdup(); /* save lvalue */
    r = gv();
    if (post) {
        /* duplicate value */
        /* XXX: handle floats */
        r1 = get_reg(REG_CLASS_INT);
        _load(r1, r, 0); /* move r to r1 */
        /* duplicates value */
//         vtop[-2].t = (vtop->t & VT_TYPE) | r1;
        wi32(vtop-(2*SValue_size)+SValue_t_o, (ri32(vtop+SValue_t_o) & VT_TYPE) | r1);
//         vtop[-2].c.i = 0;
        wi32(vtop-(2*SValue_size)+SValue_c_o, 0);
    }
    /* add constant */
    vset(VT_CONST, c - TOK_MID); 
    gen_op(mk_char('+'));
    vstore(); /* store value */
    if (post)
        vpop(); /* if post op, return saved value */
}

/* enum/struct/union declaration */
// int struct_decl(int u)
function struct_decl(u) {
    var a;
    var t;
    var b;
    var v;
    var size;
    var align;
    var maxalign;
    var c;
    var offset;
    var bit_size;
    var bit_pos;
    var bsize;
    var bt;
    var lbit_pos;
//     Sym *s, *ss, **ps;
    var s;
    var ss;
    var ps;

    enter();

    v = v_alloca(4);
    align = v_alloca(4);
    ps = v_alloca(4);

    a = tok; /* save decl type */
    next();
    while(1){
    if (tok != mk_char('{')) {
        wi32(v, tok);
        next();
        /* struct already defined ? return it */
        /* XXX: check consistency */
        if (s = sym_find(ri32(v) | SYM_STRUCT)) {
            if (ri32(s+Sym_t_o) != a)
                error("invalid type");
            break;
        }
    } else {
        wi32(v, anon_sym);
        anon_sym=anon_sym+1;
    }
    s = sym_push(ri32(v) | SYM_STRUCT, a, 0);
    /* put struct/union/enum name in type */
    break;
    }
    u = u | (ri32(v) << VT_STRUCT_SHIFT);
    if (tok == mk_char('{')) {
        next();
        if (ri32(s+Sym_c_o))
            error("struct/union/enum already defined");
        /* cannot be empty */
        c = 0;
        maxalign = 0;
        wi32(ps, s+Sym_next_o);
        bit_pos = 0;
        offset = 0;
        while (1) {
            if (a == TOK_ENUM) {
                wi32(v, tok);
                next();
                if (tok == mk_char('=')) {
                    next();
                    c = expr_const();
                }
                /* enum symbols have static storage */
                sym_push(ri32(v), VT_CONST | VT_STATIC, c);
                if (tok == mk_char(','))
                    next();
                c=c+1;
            } else {
                b = ist();
                while (1) {
                    bit_size = -1;
                    wi32(v, 0);
                    if (tok != mk_char(':')) {
                        t = type_decl(v, b, TYPE_DIRECT);
                        if ((t & VT_BTYPE) == VT_FUNC ||
                            (t & (VT_TYPEDEF | VT_STATIC | VT_EXTERN)))
err();
/*
                            error("invalid type for '%s'", 
                                  get_tok_str(v, NULL));
*/
                    }
                    size = type_size(t, align);
                    lbit_pos = 0;
                    bit_pos = 0;
                    if (ri32(v)) {
                        /* add new memory data only if starting
                           bit field */
                        if (lbit_pos == 0) {
                            if (a == TOK_STRUCT) {
                                c = (c + ri32(align) - 1) & -ri32(align);
                                offset = c;
                                c = c + size;
                            } else {
                                offset = 0;
                                if (size > c)
                                    c = size;
                            }
                            if (ri32(align) > maxalign) {
                                maxalign = ri32(align);
                            }
                        }
                        ss = sym_push(ri32(v) | SYM_FIELD, t, offset);
                        wi32(ri32(ps), ss);
                        wi32(ps, ss+Sym_next_o);
                    }
                    if (tok == mk_char(';') || tok == -1)
                        break;
                    skip(mk_char(','));
                }
                skip(mk_char(';'));
            }
            if (tok == mk_char('}'))
                break;
        }
        skip(mk_char('}'));
        /* size for struct/union, dummy for enum */
        wi32(s+Sym_c_o, (c + maxalign - 1) & -maxalign);
    }
    return leave(u);
}

// int basic_type1(int t,int u){
function basic_type1(t,u){
    if ((t & VT_BTYPE) != 0)
        err();
//        error("too many basic types %x", t);
    t = t | u;
    return t;
}

// int basic_type(int t, int u){
function basic_type(t, u){
    next();
    return basic_type1(t,u);
}

/* return 0 if no type declaration. otherwise, return the basic type
   and skip it. 
   XXX: A '2' is ored to ensure non zero return if int type.
 */
// int ist(void)
function ist() {
    var t;
    var u;
//     Sym *s;
    var s;

    enter();

    t = 0;
    while(1) {
        if(tok==TOK_CHAR) {
//             /* basic types */
            u = VT_BYTE;
            t=basic_type(t,u);
        } else if(tok==TOK_VOID) {
            u = VT_VOID;
            t=basic_type(t,u);
        } else if(tok==TOK_SHORT) {
            u = VT_SHORT;
            t=basic_type(t,u);
        } else if(tok==TOK_INT) {
            next();
        } else if(tok==TOK_ENUM) {
            u = struct_decl(VT_ENUM);
            t=basic_type1(t,u);
        } else if(tok==TOK_STRUCT|
                  tok==TOK_UNION) {
            u = struct_decl(VT_STRUCT);
            t=basic_type1(t,u);
//             /* type modifiers */
        } else if(tok==TOK_CONST|
                  tok==TOK_VOLATILE|
                  tok==TOK_REGISTER|
                  tok==TOK_SIGNED|
                  tok==TOK_AUTO|
                  tok==TOK_INLINE|
                  tok==TOK_RESTRICT) {
            next();
        } else if(tok==TOK_UNSIGNED) {
            t = t | VT_UNSIGNED;
            next();
        } else if(tok==TOK_EXTERN) {
//             /* storage */
            t = t | VT_EXTERN;
            next();
        } else if(tok==TOK_STATIC) {
            t = t | VT_STATIC;
            next();
        } else if(tok==TOK_TYPEDEF) {
            t = t | VT_TYPEDEF;
            next();
        } else {
          s = sym_find(tok);
          if ((s == 0) || ((ri32(s+Sym_t_o) & VT_TYPEDEF) == 0)) {
               return leave(t);
          }
          t = t | (ri32(s+Sym_t_o) & ~VT_TYPEDEF);
          next();
        }
        t = t | 2;
    }
    return leave(t);
}

// int post_type(int t)
// {
function post_type(t) {
    var p;
    var n;
    var pt;
    var l;
    var t1;
//     int foo=v_alloca(4);
    var foo;
//     Sym **plast, *s, *first;
    var plast;
    var s;
    var first;

    enter();

    p = v_alloca(4);
    n = v_alloca(4);
    l = v_alloca(4);
    t1 = v_alloca(4);
    plast = v_alloca(4);
    s = v_alloca(4);
    first = v_alloca(4);

// FIXME ljw there is some bug that I have introduced that causes function
// declarations of the form int foo(a,b) to not parse correctly
    if (tok == mk_char('(')) {
        /* function declaration */
        next();
        l = 0;
        wi32(first, NULL);
        wi32(plast, first);
        while (tok != mk_char(')')) {
            foo=0;
            /* read param name and compute offset */
            while(1){
            if (l != FUNC_OLD) {
                if ((pt = ist()) == 0) {
                    if (l) {
err();
//                         error("invalid type");
                    } else {
                        l = FUNC_OLD;
                        n = tok;
                        pt = VT_INT;
                        next();
                        break;
                    }
                }
                l = FUNC_NEW;
                if ((pt & VT_BTYPE) == VT_VOID && tok == mk_char(')')) {
                    foo=1;
                    break;
                }
                pt = type_decl(n, pt, TYPE_DIRECT | TYPE_ABSTRACT);
                if ((pt & VT_BTYPE) == VT_VOID)
                    error("parameter declared as void");
            } else {
                wi32(n, tok);
                pt = VT_INT;
                next();
            }
            break;
            }
            if(foo){break;}
            /* array must be transformed to pointer according to ANSI C */
            pt = pt & ~VT_ARRAY;
            s = sym_push(ri32(n) | SYM_FIELD, pt, 0);
            wi32(ri32(plast), s);
// FIXME ljw is this right?
//             plast = &s->next;
            wi32(plast, s+Sym_next_o);
            if (tok == mk_char(',')) {
                next();
                if (l == FUNC_NEW && tok == TOK_DOTS) {
                    l = FUNC_ELLIPSIS;
                    next();
                    break;
                }
            }
        }
        /* if no parameters, then old type prototype */
        if (l == 0) {
            l = FUNC_OLD;
        }
        skip(mk_char(')'));
        t1 = t & (VT_TYPEDEF | VT_STATIC | VT_EXTERN);
        t = post_type(t & ~(VT_TYPEDEF | VT_STATIC | VT_EXTERN));
        /* we push a anonymous symbol which will contain the function prototype */
        p = anon_sym;
        anon_sym=anon_sym+1;
        s = sym_push(p, t, l);
        wi32(s+Sym_next_o , ri32(first));
        t = t1 | VT_FUNC | (p << VT_STRUCT_SHIFT);
    } else if (tok == mk_char('[')) {
        /* array definition */
        next();
        n = -1;
        if (tok != mk_char(']')) {
            n = expr_const();
            if (n < 0)
                error("invalid array size");    
        }
        skip(mk_char(']'));
        /* parse next post type */
        t1 = t & (VT_TYPEDEF | VT_STATIC | VT_EXTERN);
        t = post_type(t & ~(VT_TYPEDEF | VT_STATIC | VT_EXTERN));

        /* we push a anonymous symbol which will contain the array
           element type */
        p = anon_sym;
        anon_sym=anon_sym+1;
        sym_push(p, t, n);
        t = t1 | VT_ARRAY | VT_PTR | (p << VT_STRUCT_SHIFT);
    }
    return leave(t);
}

/* Read a type declaration (except basic type), and return the
   type. If v is true, then also put variable name in 'vtop->c' */
// int type_decl(int *v, int t, int td)
function type_decl(v, t, td) {
    var u;
    var p;
//     Sym *s;
    var s;

    enter();

    t = t & -3; /* suppress the ored '2' */
    while (tok == mk_char('*')) {
       next();
       while (tok == TOK_CONST || tok == TOK_VOLATILE || tok == TOK_RESTRICT)
           next();
       t = mk_pointer(t);
    }
    /* recursive type */
    /* XXX: incorrect if abstract type for functions (e.g. 'int ()') */
    if (tok == mk_char('(')) {
        next();
        u = type_decl(v, 0, td);
        skip(mk_char(')'));
    } else {
        u = 0;
        /* type identifier */
        if (tok >= TOK_IDENT && ((td & TYPE_DIRECT) != 0)) {
            wi32(v, tok);
            next();
         } else {
            if ((td & TYPE_ABSTRACT) == 0)
                expect("identifier");
            wi32(v, 0);
         }
    }

    /* append t at the end of u */
    print("tok: "+tok+" t: "+t); /* dbg log */
    t = post_type(t);
    if (u == 0) {
        return leave(t);
    }
    p = u;
    while(1) {
        s = sym_find(urs(p, VT_STRUCT_SHIFT));
        p = ri32(s+Sym_t_o);
        if (p == 0) {
            wi32(s+Sym_t_o, t);
            break;
        }
    }
    return leave(u);
}

/* define a new external reference to a function 'v' of type 'u' */
// Sym *external_sym(int v, int u)
function external_sym(v, u) {
//     Sym *s;
    var s;
    s = sym_find(v);
    if (s == 0) {
        /* push forward reference */
        s = sym_push1(global_stack, 
                      v, u | VT_CONST | VT_FORWARD, 0);
    }
    return s;
}

// void indir(void)
function indir() {
    if (ri32(vtop+SValue_t_o) & VT_LVAL) {
        gv();
    }
    if ((ri32(vtop+SValue_t_o) & VT_BTYPE) != VT_PTR) {
err();
//         expect("pointer");
    }
    wi32(vtop+SValue_t_o,pointed_type(ri32(vtop+SValue_t_o)));
    if ((ri32(vtop+SValue_t_o) & VT_ARRAY) == 0) { /* an array is never an lvalue */
        wi32(vtop+SValue_t_o, ri32(vtop+SValue_t_o) | VT_LVAL);
    }
}

/* pass a parameter to a function and do type checking and casting */
// void gfunc_param_typed(GFuncContext *gf, Sym *func, Sym *arg)
function gfunc_param_typed(gf, func, arg) {
    var func_type;
    func_type = ri32(func+Sym_c_o);
    print("func_type: "+func_type+" arg "+arg); /* dbg log */
    if (func_type == FUNC_OLD ||
        (func_type == FUNC_ELLIPSIS && arg == NULL)) {
        /* default casting : only need to convert float to double */
        if ((ri32(vtop+SValue_t_o) & VT_BTYPE) == VT_FLOAT) {
err();
//             gen_cast(VT_DOUBLE);
        }
    } else if (arg == NULL) {
err();
//         error("too many arguments to function");
    } else {
        gen_assign_cast(ri32(arg+Sym_t_o));
    }
    gfunc_param(gf);
}

// void unary(void)
function unary() {
    var n;
    var t;
    var ft;
    var fc;
    var p;
    var align;
    var size;
//     Sym *s;
    var s;
//     GFuncContext gf;
    var gf;

// hoisted declarations outside of later loop to allow M2 to compile this (and
// otccelf)
    var rett;
    var retc;
    var sa;
    var str;
    var len;
    var parlevel;
    var saved_macro_ptr;
    var args;
    var s1;

    enter();

    n = v_alloca(4);
    align = v_alloca(4);
    gf = v_alloca(4);

    if (tok == TOK_NUM || tok == TOK_CCHAR || tok == TOK_LCHAR) {
//         vset(VT_CONST | VT_INT, tokc.i);
        vset(VT_CONST | VT_INT, ri32(tokc));
        next();
    } else if (tok == TOK___FUNC__) {
err();
//         /* special function name identifier */
//         /* generate (char *) type */
//         vset(VT_CONST | mk_pointer(VT_BYTE), glo);
//         v_strcpy((void *)glo, funcname);
//         glo += v_strlen(funcname) + 1;
//         next();
    } else if (tok == TOK_STR) {
        /* string parsing */
        t = VT_BYTE;
        type_size(t, align);
        glo = (glo + ri32(align) - 1) & -ri32(align);
        fc = glo;
        /* we must declare it as an array first to use initializer parser */
        t = VT_CONST | VT_ARRAY | mk_pointer(t);
        decl_initializer(t, glo, 1, 0);
        glo = glo + type_size(t, ri32(align));
        /* put it as pointer */
        vset(t & ~VT_ARRAY, fc);
    } else {
        t = tok;
        next();
        if (t == mk_char('(')) {
            /* cast ? */
            if (t = ist()) {
                ft = type_decl(n, t, TYPE_ABSTRACT);
                skip(mk_char(')'));
                /* check ISOC99 compound literal */
                if (tok == mk_char('{')) {
err();
//                     /* data is allocated locally by default */
//                     if (global_expr)
//                         ft |= VT_CONST;
//                     else
//                         ft |= VT_LOCAL;
//                     /* all except arrays are lvalues */
//                     if (!(ft & VT_ARRAY))
//                         ft |= VT_LVAL;
//                     fc = decl_initializer_alloc(ft, 1);
//                     vset(ft, fc);
                } else {
                    unary();
                    gen_cast(ft);
                }
            } else {
                gexpr();
                skip(mk_char(')'));
            }
        } else if (t == mk_char('*')) {
            unary();
            indir();
        } else if (t == mk_char('&')) {
            unary();
            /* functions names must be treated as function pointers,
               except for unary '&' and sizeof. Since we consider that
               functions are not lvalues, we only have to handle it
               there and in function calls. */
            if ((ri32(vtop+SValue_t_o) & VT_BTYPE) != VT_FUNC) {
                test_lvalue();
            }
            wi32(vtop+SValue_t_o, mk_pointer(ri32(vtop+SValue_t_o) & VT_LVALN));
        } else if (t == mk_char('!')) {
            unary();
            if ((ri32(vtop+SValue_t_o) & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
err();
//                 vtop->c.i = !vtop->c.i;
            } else if ((ri32(vtop+SValue_t_o) & VT_VALMASK) == VT_CMP) {
//                 vtop->c.i = vtop->c.i ^ 1;
                wi32(vtop+SValue_c_o, ri32(vtop+SValue_c_o) ^ 1);
            } else {
                vset(VT_JMP, gtst(1, 0));
            }
        } else if (t == mk_char('~')) {
            unary();
            vset(VT_CONST, -1);
            gen_op(mk_char('^'));
        } else if (t == mk_char('+')) {
err();
//             unary();
        } else if (t == TOK_SIZEOF) {
            if (tok == mk_char('(')) {
                next();
                if (t = ist()) {
                    t = type_decl(n, t, TYPE_ABSTRACT);
                } else {
                    /* XXX: some code could be generated: add eval
                       flag */
                    gexpr();
                    t = ri32(vtop+SValue_t_o);
                    vpop();
                }
                skip(mk_char(')'));
            } else {
err();
//                 unary();
//                 t = vtop->t;
//                 vpop();
            }

// FIXME ljw had to use dummy alloca
//             vset(VT_CONST, type_size(t, &t));
            vset(VT_CONST, type_size(t, v_alloca(4)));
        } else if (t == TOK_INC || t == TOK_DEC) {
            unary();
            inc(0, t);
        } else if (t == mk_char('-')) {
            vset(VT_CONST, 0);
            unary();
            gen_op(mk_char('-'));
        } else {
            s = sym_find(t);
            if (s == 0) {
                if (tok != mk_char('('))
                    err();
//                    error("'%s' undeclared", get_tok_str(t, NULL));
                /* for simple function calls, we tolerate undeclared
                   external reference */
                p = anon_sym;
                anon_sym=anon_sym+1;
                sym_push1(global_stack, p, 0, FUNC_OLD);
                /* int() function */
                s = external_sym(t, VT_FUNC | (p << VT_STRUCT_SHIFT));
            }
            vset(ri32(s+Sym_t_o), ri32(s+Sym_c_o));
            /* if forward reference, we must point to s */
            if (ri32(vtop+SValue_t_o) & VT_FORWARD) {
//                 vtop->c.sym = s;
                wi32(vtop+SValue_c_o, s);
            }
        }
    }

    /* post operations */
    while (1) {
        if (tok == TOK_INC | tok == TOK_DEC) {
            inc(1, tok);
            next();
        } else if (tok == mk_char('.') | tok == TOK_ARROW) {
            /* field */ 
            if (tok == TOK_ARROW) {
                indir();
            }
            test_lvalue();
            wi32(vtop+SValue_t_o, ri32(vtop+SValue_t_o) & VT_LVALN);
            next();
            /* expect pointer on structure */
            if ((ri32(vtop+SValue_t_o) & VT_BTYPE) != VT_STRUCT) {
err();
//                 expect("struct or union");
            }
            s = sym_find((urs(ri32(vtop+SValue_t_o), VT_STRUCT_SHIFT)) | SYM_STRUCT);
            /* find field */
            tok = tok | SYM_FIELD;
            while (s = ri32(s+Sym_next_o)) {
                if (ri32(s+Sym_v_o) == tok)
                    break;
            }
            if (s == 0)
                error("field not found");
            /* add field offset to pointer */
            wi32(vtop+SValue_t_o,(ri32(vtop+SValue_t_o) & ~VT_TYPE) | VT_INT); /* change type to int */
            vset(VT_CONST, ri32(s+Sym_c_o));
            gen_op(mk_char('+'));
            /* change type to field type, and set to lvalue */
            wi32(vtop+SValue_t_o, (ri32(vtop+SValue_t_o) & ~VT_TYPE) | ri32(s+Sym_t_o));
            /* an array is never an lvalue */
            if ((ri32(vtop+SValue_t_o) & VT_ARRAY) == 0) {
                wi32(vtop+SValue_t_o, ri32(vtop+SValue_t_o) | VT_LVAL);
            }
            next();
        } else if (tok == mk_char('[')) {
            next();
            gexpr();
            gen_op(mk_char('+'));
            indir();
            skip(mk_char(']'));
        } else if (tok == mk_char('(')) {
// have to hoist these declarations outside the loop to allow this to compile
// with M2 or cc_x86
//             CValue retc;
            retc=v_alloca(4);
//             Sym *sa;

            /* function call  */
            if ((ri32(vtop+SValue_t_o) & VT_BTYPE) != VT_FUNC) {
err();
//                 /* pointer test (no array accepted) */
//                 if ((vtop->t & (VT_BTYPE | VT_ARRAY)) == VT_PTR) {
//                     vtop->t = pointed_type(vtop->t);
//                     if ((vtop->t & VT_BTYPE) != VT_FUNC)
//                         expect("function pointer");
//                 } else {
//                     expect("function pointer");
//                 }
            } else {
                wi32(vtop+SValue_t_o, ri32(vtop+SValue_t_o) & ~VT_LVAL); /* no lvalue */
            }
            /* get return type */
            s = sym_find(urs(ri32(vtop+SValue_t_o), VT_STRUCT_SHIFT));
            save_regs(); /* save used temporary registers */
            gfunc_start(gf);
            next();
            sa = ri32(s+Sym_next_o); /* first parameter */
// FIXME ljw this is a block can we hoist varables to top of block?
// ljw declarations have now be hoisted
// {
//                 int *str, len, parlevel, *saved_macro_ptr;
                str=v_alloca(4);
                len=v_alloca(4);
                parlevel;
                saved_macro_ptr;
//                 Sym *args, *s1;
                args=v_alloca(4);
                s1;

                /* read each argument and store it on a stack */
                /* XXX: merge it with macro args ? */
                wi32(args, NULL);
                while (tok != mk_char(')')) {
                    wi32(len, 0);
                    wi32(str, NULL);
                    parlevel = 0;
                    while ((parlevel > 0 || (tok != mk_char(')') && tok != mk_char(','))) && 
                           tok != -1) {
                        if (tok == mk_char('('))
                            parlevel=parlevel+1;
                        else if (tok == mk_char(')'))
                            parlevel=parlevel-1;
                        tok_add2(str, len, tok, tokc);
                        next();
                    }
                    tok_add(str, len, -1); /* end of file added */
                    tok_add(str, len, 0);
// FIXME ljw is this right
//                     s1 = sym_push2(&args, 0, 0, (int)str);
                    s1 = sym_push2(args, 0, 0, ri32(str));
                    wi32(s1+Sym_next_o, sa); /* add reference to argument */
                    if (sa)
                        sa = ri32(sa+Sym_next_o);
                    if (tok != mk_char(','))
                        break;
                    next();
                }
                if (tok != mk_char(')'))
                    expect(")");

                /* now generate code in reverse order by reading the stack */
                saved_macro_ptr = ri32(macro_ptr);
                while (ri32(args)) {
                    wi32(macro_ptr, ri32(ri32(args)+Sym_c_o));
                    next();
                    expr_eq();
                    if (tok != -1)
                        expect("',' or ')'");
                    gfunc_param_typed(gf, s, ri32(ri32(args)+Sym_next_o));
                    s1 = ri32(ri32(args)+Sym_prev_o);
// FIXME ljw should free
//                     v_free((int *)args->c);
//                     v_free(args);
                    wi32(args, s1);
                }
                wi32(macro_ptr, saved_macro_ptr);
                /* restore token */
                tok = mk_char(')');
            /* compute first implicit argument if a structure is returned */
            if ((ri32(s+Sym_t_o) & VT_BTYPE) == VT_STRUCT) {
err();
//                 /* get some space for the returned structure */
//                 size = type_size(s->t, &align);
//                 loc = (loc - size) & -align;
//                 rett = s->t | VT_LOCAL | VT_LVAL;
//                 /* pass it as 'int' to avoid structure arg passing
//                    problems */
//                 vset(VT_INT | VT_LOCAL, loc);
//                 retc = vtop->c;
//                 gfunc_param(&gf);
            } else {
                rett = ri32(s+Sym_t_o) | FUNC_RET_REG; /* return in register */
//                 retc.i = 0;
                wi32(retc, 0);
            }
            if (sa)
                error("too few arguments to function %x");
            skip(mk_char(')'));
            gfunc_call(gf);
            /* return value */
            vsetc(rett, retc);
        } else {
            break;
        }
    }
    leave(0);
}

// void uneq(void)
function uneq() {
    var t;

    unary();
    if (tok == mk_char('=') ||
        (tok >= TOK_A_MOD && tok <= TOK_A_DIV) ||
        tok == TOK_A_XOR || tok == TOK_A_OR ||
        tok == TOK_A_SHL || tok == TOK_A_SAR) {
        test_lvalue();
        t = tok;
        next();
        if (t == mk_char('=')) {
            expr_eq();
        } else {
            vdup();
            expr_eq();
            gen_op(t & 0x7F);
        }
        vstore();
    }
}

// void sum(int l)
function sum(l) {
    var t;

    if (l == 0) {
        uneq();
    } else {
//         sum(--l);
        sum(l=l-1);
        while ((l == 0 & (tok == mk_char('*') | tok == mk_char('/') | tok == mk_char('%'))) |
               (l == 1 & (tok == mk_char('+') | tok == mk_char('-'))) |
               (l == 2 & (tok == TOK_SHL | tok == TOK_SAR)) |
               (l == 3 & ((tok >= TOK_ULE & tok <= TOK_GT) |
                          tok == TOK_ULT | tok == TOK_UGE)) |
               (l == 4 & (tok == TOK_EQ | tok == TOK_NE)) |
               (l == 5 & tok == mk_char('&')) |
               (l == 6 & tok == mk_char('^')) |
               (l == 7 & tok == mk_char('|')) |
               (l == 8 & tok == TOK_LAND) |
               (l == 9 & tok == TOK_LOR)) {
            t = tok;
            next();
            sum(l);
            gen_op(t);
       }
    }
}

/* only used if non constant */
// void eand(void)
function eand() {
    var t;

    sum(8);
    t = 0;
    while (1) {
        if (tok != TOK_LAND) {
            if (t) {
                t = gtst(1, t);
                vset(VT_JMPI, t);
            }
            break;
        }
        t = gtst(1, t);
        next();
        sum(8);
    }
}

// void eor(void)
function eor() {
    var t;

    eand();
    t = 0;
    while (1) {
        if (tok != TOK_LOR) {
            if (t) {
                t = gtst(0, t);
                vset(VT_JMP, t);
            }
            break;
        }
        t = gtst(0, t);
        next();
        eand();
    }
}

/* XXX: better constant handling */
// void expr_eq(void)
function expr_eq() {
    var t;
    var u;
    var c;
    var r1;
    var r2;

    if (const_wanted) {
        sum(10);
        if (tok == mk_char('?')) {
err();
//             c = vtop->c.i;
//             vpop();
//             next();
//             gexpr();
//             t = vtop->c.i;
//             vpop();
//             skip(':');
//             expr_eq();
//             if (c)
//                 vtop->c.i = t;
        }
    } else {
        eor();
        if (tok == mk_char('?')) {
err();
//             next();
//             t = gtst(1, 0);
// 
//             gexpr();
//             r1 = gv();
//             vpop();
//             skip(':');
//             u = gjmp(0);
// 
//             gsym(t);
//             expr_eq();
//             r2 = gv();
//             move_reg(r1, r2);
//             vtop->t = (vtop->t & VT_TYPE) | r1;
//             gsym(u);
        }
    }
}

// void gexpr(void)
function gexpr() {
    while (1) {
        expr_eq();
        if (tok != mk_char(','))
            break;
        vpop();
        next();
    }
}

/* parse a constant expression and return value in vtop */
// void expr_const1(void)
function expr_const1() {
    var a;
    a = const_wanted;
    const_wanted = 1;
    expr_eq();
    if ((ri32(vtop+SValue_t_o) & (VT_CONST | VT_LVAL)) != VT_CONST)
        expect("constant");
    const_wanted = a;
}

/* parse an integer constant and return its value */
// int expr_const(void)
function expr_const() {
    var c;
    expr_const1();
//     c = vtop->c.i;
    c = ri32(vtop+SValue_c_o);
    vpop();
    return c;
}

// /* return the label token if current token is a label, otherwise
//    return zero */
// int is_label(void)
// {
//     int t;
//     CValue c;
// 
//     /* fast test first */
//     if (tok < TOK_UIDENT)
//         return 0;
//     /* no need to save tokc since we expect an identifier */
//     t = tok;
//     c = tokc;
//     next();
//     if (tok == ':') {
//         next();
//         return t;
//     } else {
//         /* XXX: may not work in all cases (macros ?) */
//         tok1 = tok;
//         tok1c = tokc;
//         tok = t;
//         tokc = c;
//         return 0;
//     }
// }

// void block(int *bsym, int *csym, int *case_sym, int *def_sym, int case_reg)
function block(bsym, csym, case_sym, def_sym, case_reg) {
    var a;
    var b;
    var c;
    var d;
//     Sym *s;
    var s;

    enter();

    a = v_alloca(4);
    b = v_alloca(4);
    c = v_alloca(4);

    if (tok == TOK_IF) {
        /* if test */
        next();
        skip(mk_char('('));
        gexpr();
        skip(mk_char(')'));
        a = gtst(1, 0);
        block(bsym, csym, case_sym, def_sym, case_reg);
        c = tok;
        if (c == TOK_ELSE) {
            next();
            d = gjmp(0);
            gsym(a);
            block(bsym, csym, case_sym, def_sym, case_reg);
            gsym(d); /* patch else jmp */
        } else {
            gsym(a);
        }
    } else if (tok == TOK_WHILE) {
        next();
        d = ind;
        skip(mk_char('('));
        gexpr();
        skip(mk_char(')'));
        wi32(a, gtst(1, 0));
        wi32(b, 0);
        block(a, b, case_sym, def_sym, case_reg);
        oad(0xE9, d - ind - 5); /* jmp */
        gsym(a);
        gsym_addr(b, d);
    } else if (tok == mk_char('{')) {
        next();
        /* declarations */
        s = ri32(local_stack+SymStack_top_o);
        while (tok != mk_char('}')) {
            decl(VT_LOCAL);
            if (tok != mk_char('}')) {
                block(bsym, csym, case_sym, def_sym, case_reg);
            }
        }
        /* pop locally defined symbols */
        sym_pop(local_stack, s);
        next();
    } else if (tok == TOK_RETURN) {
        next();
        if (tok != mk_char(';')) {
            gexpr();
            if ((func_vt & VT_BTYPE) == VT_STRUCT) {
err();
//                 /* if returning structure, must copy it to implicit
//                    first pointer arg location */
//                 vset(mk_pointer(func_vt) | VT_LOCAL | VT_LVAL, func_vc);
//                 indir();
//                 vswap();
//                 /* copy structure value to pointer */
//                 vstore();
            } else {
                /* move return value to standard return register */
                move_reg(FUNC_RET_REG, gv());
            }
            vpop();
        }
        skip(mk_char(';'));
        rsym = gjmp(rsym); /* jmp */
    } else if (tok == TOK_BREAK) {
        /* compute jump */
        if (bsym == 0)
            error("cannot break");
        wi32(bsym, gjmp(ri32(bsym)));
        next();
        skip(mk_char(';'));
    } else if (tok == TOK_CONTINUE) {
        /* compute jump */
        if (csym == 0)
            error("cannot continue");
        wi32(csym, gjmp(ri32(csym)));
        next();
        skip(mk_char(';'));
    } else if (tok == TOK_FOR) {
        var e;
        next();
        skip(mk_char('('));
        if (tok != mk_char(';')) {
            gexpr();
            vpop();
        }
        skip(mk_char(';'));
        d = ind;
        c = ind;
        wi32(a, 0);
        wi32(b, 0);
        if (tok != mk_char(';')) {
            gexpr();
            wi32(a, gtst(1, 0));
        }
        skip(mk_char(';'));
        if (tok != ')') {
            e = gjmp(0);
            c = ind;
            gexpr();
            vpop();
            oad(0xE9, d - ind - 5); /* jmp */
            gsym(e);
        }
        skip(mk_char(')'));
        block(a, b, case_sym, def_sym, case_reg);
        oad(0xE9, c - ind - 5); /* jmp */
        gsym(ri32(a));
        gsym_addr(ri32(b), c);
    } else if (tok == TOK_DO) {
err();
//         next();
//         a = 0;
//         b = 0;
//         d = ind;
//         block(&a, &b, case_sym, def_sym, case_reg);
//         skip(TOK_WHILE);
//         skip('(');
//         gsym(b);
//         gexpr();
//         c = gtst(0, 0);
//         gsym_addr(c, d);
//         skip(')');
//         gsym(a);
//         skip(';');
    } else if (tok == TOK_SWITCH) {
        next();
        skip(mk_char('('));
        gexpr();
        case_reg = gv();
        vpop();
        skip(mk_char(')'));
        wi32(a, 0);
        wi32(b, gjmp(0)); /* jump to first case */
        wi32(c, 0);
        block(a, csym, b, c, case_reg);
        /* if no default, jmp after switch */
        if (ri32(c) == 0)
            wi32(c, ind);
        /* default label */
        gsym_addr(ri32(b), ri32(c));
        /* break label */
        gsym(ri32(a));
    } else if (tok == TOK_CASE) {
        next();
        a = expr_const();
        if (case_sym == 0)
            expect("switch");
        /* since a case is like a label, we must skip it with a jmp */
        b = gjmp(0);
        gsym(ri32(case_sym));
        vset(case_reg, 0);
        vset(VT_CONST, a);
        gen_op(TOK_EQ);
        wi32(case_sym, gtst(1, 0));
        gsym(b);
        skip(mk_char(':'));
        block(bsym, csym, case_sym, def_sym, case_reg);
    } else if (tok == TOK_DEFAULT) {
        next();
        skip(mk_char(':'));
        if (def_sym == 0)
            expect("switch");
        if (ri32(def_sym))
            error("too many 'default'");
        wi32(def_sym, ind);
        block(bsym, csym, case_sym, def_sym, case_reg);
    } else {
        /* expression case */
        if (tok != mk_char(';')) {
            gexpr();
            vpop();
        }
        skip(mk_char(';'));
    }
    leave(0);
}

/* t is the array or struct type. c is the array or struct
   address. cur_index/cur_field is the pointer to the current
   value. 'size_only' is true if only size info is needed (only used
   in arrays) */
// void decl_designator(int t, int c, 
//                      int *cur_index, Sym **cur_field, 
//                      int size_only)
function decl_designator(t, c, cur_index, cur_field, size_only) {
//     Sym *s, *f;
    var s;
    var f;
    var notfirst;
    var index;
    var align;
    var l;

    enter();

    align = v_alloca(4);

    notfirst = 0;

    while (tok == mk_char('[') || tok == mk_char('.')) {
err();
//         if (tok == '[') {
//             if (!(t & VT_ARRAY))
//                 expect("array type");
//             s = sym_find(((unsigned)t >> VT_STRUCT_SHIFT));
//             next();
//             index = expr_const();
//             if (index < 0 || (s->c >= 0 && index >= s->c))
//                 expect("invalid index");
//             skip(']');
//             if (!notfirst)
//                 *cur_index = index;
//             t = pointed_type(t);
//             c += index * type_size(t, &align);
//         } else {
//             next();
//             l = tok;
//             next();
//             if ((t & VT_BTYPE) != VT_STRUCT)
//                 expect("struct/union type");
//             s = sym_find(((unsigned)t >> VT_STRUCT_SHIFT) | SYM_STRUCT);
//             l |= SYM_FIELD;
//             f = s->next;
//             while (f) {
//                 if (f->v == l)
//                     break;
//                 f = f->next;
//             }
//             if (!f)
//                 expect("field");
//             if (!notfirst)
//                 *cur_field = f;
//             t = f->t | (t & ~VT_TYPE);
//             c += f->c;
//         }
//         notfirst = 1;
    }
    if (notfirst) {
err();
//         if (tok == '=') {
//             next();
//         } else {
//             if (!gnu_ext)
//                 expect("=");
//         }
    } else {
        if (t & VT_ARRAY) {
            index = ri32(cur_index);
            t = pointed_type(t);
            c = c + (index * type_size(t, align));
        } else {
            f = ri32(cur_field);
            if (f == 0)
                error("too many field init");
            t = ri32(f+Sym_t_o) | (t & ~VT_TYPE);
            c = c + ri32(f+Sym_c_o);
        }
    }
    decl_initializer(t, c, 0, size_only);
    leave(0);
}

/* store a value or an expression directly in global data or in local array */

// void init_putv(int t, int c, int v, int is_expr)
function init_putv(t, c, v, is_expr) {
    var saved_global_expr;
    var bt;

    if ((t & VT_VALMASK) == VT_CONST) {
        if (is_expr) {
            /* compound literals must be allocated globally in this case */
            saved_global_expr = global_expr;
            global_expr = 1;
            expr_const1();
            global_expr = saved_global_expr;
        } else {
            vset(VT_CONST | VT_INT, v);
        }
        /* XXX: do casting */
        /* XXX: not portable */
        bt = ri32(vtop+SValue_t_o) & VT_BTYPE;
        if(bt==VT_BYTE) {
err();
//             *(char *)c = vtop->c.i;
        } else if(bt==VT_SHORT) {
err();
//             *(short *)c = vtop->c.i;
        } else {
            wi32(c, ri32(vtop+SValue_c_o));
        }
        vpop();
    } else {
        vset(t, c);
        if (is_expr)
            expr_eq();
        else
            vset(VT_CONST | VT_INT, v);
        vstore();
        vpop();
    }
}

/* put zeros for variable based init */
// void init_putz(int t, int c, int size)
function init_putz(t, c, size) {
err();
//     GFuncContext gf;
// 
//     if ((t & VT_VALMASK) == VT_CONST) {
//         /* nothing to do because global are already set to zero */
//     } else {
//         gfunc_start(&gf);
//         vset(VT_CONST, size);
//         gfunc_param(&gf);
//         vset(VT_CONST, 0);
//         gfunc_param(&gf);
//         vset(VT_LOCAL, c);
//         gfunc_param(&gf);
//         vset(VT_CONST, (int)&memset);
// if(reloc){
//   printf("memset function point stuff\n");
// }
//         gfunc_call(&gf);
//     }
}

/* 't' contains the type and storage info. c is the address of the
   object. 'first' is true if array '{' must be read (multi dimension
   implicit array init handling). 'size_only' is true if size only
   evaluation is wanted (only for arrays). */
// void decl_initializer(int t, int c, int first, int size_only)
function decl_initializer(t, c, first, size_only) {
    print("decl_initializer: t: "+t+" c: "+c+" first: "+first+" size_only: "+size_only); /* dbg log */

    var index;
    var array_length;
    var n;
    var no_oblock;
    var nb;
    var parlevel;
    var i;
    var t1;
    var size1;
    var align1;
//     Sym *s, *f;
    var s;
    var f;
//     TokenSym *ts;
    var ts;

    enter();

    index = v_alloca(4);
    align1 = v_alloca(4);
    s = v_alloca(4);
    f = v_alloca(4);
    ts = v_alloca(4);

    if (t & VT_ARRAY) {
        s = sym_find((urs(t, VT_STRUCT_SHIFT)));
        n = ri32(s+Sym_c_o);
        array_length = 0;
        t1 = pointed_type(t);
        size1 = type_size(t1, align1);

        no_oblock = 1;
        if (((first != 0) && tok != TOK_LSTR && tok != TOK_STR) || 
            tok == mk_char('{')) {
            skip(mk_char('{'));
            no_oblock = 0;
        }

        /* only parse strings here if correct type (otherwise: handle
           them as ((w)char *) expressions */
        if ((tok == TOK_LSTR && 
             (t1 & VT_BTYPE) == VT_INT) ||
            (tok == TOK_STR &&
             (t1 & VT_BTYPE) == VT_BYTE)) {
             /* XXX: move multiple string parsing in parser ? */
            while (tok == TOK_STR || tok == TOK_LSTR) {
//                 ts = tokc.ts;
                ts = ri32(tokc);
                /* compute maximum number of chars wanted */
                nb = ri32(ts+TokenSym_len_o);
                if (n >= 0 && nb > (n - array_length))
                    nb = n - array_length;
                if (size_only == 0) {
                    if (ri32(ts+TokenSym_len_o) > nb)
                        warning("initializer-string for array is too long");
                    for(i=0;i<nb;i=i+1) {
//                         init_putv(t1, c + (array_length + i) * size1, 
//                                   ts->str[i], 0);
                        init_putv(t1, c + (array_length + i) * size1, 
                                  ri8(ts+TokenSym_str_o+i), 0);
                    }
                }
                array_length = array_length + nb;
                next();
            }
            /* only add trailing zero if enough storage (no
               warning in this case since it is standard) */
            if (n < 0 || array_length < n) {
                if (size_only == 0) {
                    init_putv(t1, c + (array_length * size1), 0, 0);
                }
                array_length=array_length+1;
            }
        } else {
            wi32(index, 0);
             while (tok != mk_char('}')) {
                decl_designator(t, c, index, NULL, size_only);
                if (n >= 0 && ri32(index) >= n)
                    error("index too large");
                /* must put zero in holes (note that doing it that way
                   ensures that it even works with designators) */
                if ((size_only == 0) && array_length < ri32(index)) {
err();
//                     init_putz(t1, c + array_length * size1, 
//                               (index - array_length) * size1);
                }
                wi32(index,ri32(index)+1);
                if (ri32(index) > array_length) {
                    array_length = ri32(index);
                }
                /* special test for multi dimensional arrays (may not
                   be strictly correct if designators are used at the
                   same time) */
                if (ri32(index) >= n && no_oblock) {
                    break;
                }
                if (tok == mk_char('}'))
                    break;
                skip(mk_char(','));
            }
        }
        if (no_oblock == 0)
            skip(mk_char('}'));
        /* put zeros at the end */
        if ((size_only == 0) && n >= 0 && array_length < n) {
            init_putz(t1, c + array_length * size1, 
                      (n - array_length) * size1);
        }
        /* patch type size if needed */
        if (n < 0)
            wi32(s+Sym_c_o, array_length);
    } else if ((t & VT_BTYPE) == VT_STRUCT && tok == mk_char('{')) {
        /* XXX: union needs only one init */
        next();
        s = sym_find((urs(t, VT_STRUCT_SHIFT)) | SYM_STRUCT);
        wi32(f, ri32(s+Sym_next_o));
        array_length = 0;
        index = 0;
        n = ri32(s+Sym_c_o);
        while (tok != mk_char('}')) {
            decl_designator(t, c, NULL, f, size_only);
            /* fill with zero between fields */
            index = ri32(ri32(f)+Sym_c_o);
            if ((size_only == 0) && array_length < index) {
err();
//                 init_putz(t, c + array_length, 
//                           index - array_length);
            }
            index = index + type_size(ri32(ri32(f)+Sym_t_o), align1);
            if (index > array_length)
                array_length = index;
            if (tok == mk_char('}'))
                break;
            skip(mk_char(','));
            wi32(f, ri32(ri32(f)+Sym_next_o));
        }
        /* put zeros at the end */
        if ((size_only == 0) && array_length < n) {
err();
//             init_putz(t, c + array_length, 
//                       n - array_length);
        }
        skip(mk_char('}'));
    } else if (tok == mk_char('{')) {
err();
//         next();
//         decl_initializer(t, c, first, size_only);
//         skip('}');
    } else if (size_only) {
        /* just skip expression */
        parlevel = 0;
        while ((parlevel > 0 || (tok != mk_char('}') && tok != mk_char(','))) && 
               tok != -1) {
             if (tok == mk_char('('))
                parlevel=parlevel+1;
            else if (tok == mk_char(')'))
                parlevel=parlevel-1;
            next();
        }
    } else {
        init_putv(t, c, 0, 1);
    }
   return leave(0);
}

/* parse an initializer for type 't' if 'has_init' is true, and
   allocate space in local or global data space. The allocated address
   in returned */
// int decl_initializer_alloc(int t, int has_init)
function decl_initializer_alloc(t, has_init) {
print("decl_initializer_alloc: t: "+t+" has_init: "+has_init);
    var size;
    var align;
    var addr;
    var tok1;
//     int *init_str, init_len, level, *saved_macro_ptr;
    var init_str;
    var init_len;
    var level;
    var saved_macro_ptr;

    enter();

    align = v_alloca(4);
    init_str = v_alloca(4);
    init_len = v_alloca(4);
    level = v_alloca(4);

    size = type_size(t, align);
    /* If unknown size, we must evaluate it before
       evaluating initializers because
       initializers can generate global data too
       (e.g. string pointers or ISOC99 compound
       literals). It also simplifies local
       initializers handling */
    wi32(init_len, 0);
    wi32(init_str, NULL);
    saved_macro_ptr = NULL; /* avoid warning */
    tok1 = 0;
    if (size < 0) {
        if (has_init == 0) 
            error("unknown type size");
        /* get all init string */
        level = 0;
        while (level > 0 || (tok != mk_char(',') && tok != mk_char(';'))) {
            if (tok < 0)
                error("unexpect end of file in initializer");
            tok_add2(init_str, init_len, tok, tokc);
            if (tok == mk_char('{'))
                level=level+1;
            else if (tok == mk_char('}')) {
                if (level == 0)
                    break;
                level=level-1;
            }
            next();
        }
        tok1 = tok;
        tok_add(init_str, init_len, -1);
        tok_add(init_str, init_len, 0);

        /* compute size */
        saved_macro_ptr = ri32(macro_ptr);
        wi32(macro_ptr, ri32(init_str));
        next();
        decl_initializer(t, 0, 1, 1);
        /* prepare second initializer parsing */
        wi32(macro_ptr, ri32(init_str));
        next();

        /* if still unknown size, error */
        size = type_size(t, align);
        if (size < 0) 
            error("unknown type size");
    }
    if ((t & VT_VALMASK) == VT_LOCAL) {
        loc = (loc - size) & -(ri32(align));
        addr = loc;
    } else {
        glo = (glo + ri32(align) - 1) & -ri32(align);
        addr = glo;
        /* very important to increment global
           pointer at this time because
           initializers themselves can create new
           initializers */
        glo = glo + size;
    }
    if (has_init) {
        decl_initializer(t, addr, 1, 0);
        /* restore parse state if needed */
        if (ri32(init_str)) {
            v_free(ri32(init_str));
            wi32(macro_ptr, saved_macro_ptr);
            tok = tok1;
        }
    }
    return leave(addr);
}

/* 'l' is VT_LOCAL or VT_CONST to define default storage type */
// void decl(int l)
function decl(l) {
//     int *a, t, b, v, u, addr, has_init, size, align;
    var a;
    var t;
    var b;
    var v;
    var u;
    var addr;
    var has_init;
    var size;
    var align;
//     Sym *sym;
    var sym;
    var cont;

    enter();

    a = v_alloca(4);
    t = v_alloca(4);
    b = v_alloca(4);
    v = v_alloca(4);
    u = v_alloca(4);
    addr = v_alloca(4);
    has_init = v_alloca(4);
    size = v_alloca(4);
    align = v_alloca(4);

    /* cc_c86 doesn't support continue so we need this hack */
    cont=1;

    while(cont){
    cont=0;
    while (1) {
         b = ist();
         if (b == 0) {
            /* skip redundant ';' */
            /* XXX: find more elegant solution */
            if (tok == mk_char(';')) {
                next();
                cont=1;break;
            }
            /* special test for old K&R protos without explicit int
               type. Only accepted when defining global data */
            if (l == VT_LOCAL || tok < TOK_DEFINE) {
                break;
            }
err();
//             b = VT_INT;
         }
         if (((b & VT_BTYPE) == VT_ENUM ||
              (b & VT_BTYPE) == VT_STRUCT) &&
             tok == mk_char(';')) {
            /* we accept no variable after */
             next();
             cont=1;break;
         }
         while (1) { /* iterate thru each declaration */
            t = type_decl(v, b, TYPE_DIRECT);
            if (tok == mk_char('{')) {
                if (l == VT_LOCAL) {
                    error("cannot use local functions");
                }
                if ((t & VT_FUNC) == 0) {
                    expect("function definition");
                }
                /* patch forward references */
                if (((sym = sym_find(ri32(v))) != 0) && ((ri32(sym+Sym_t_o) & VT_FORWARD) != 0)) {
                    greloc_patch(sym, ind);
                    wi32(sym+Sym_t_o, VT_CONST | t);
                } else {
                    /* put function address */
                    sym_push1(global_stack, ri32(v), VT_CONST | t, ind);
                }
                funcname = get_tok_str(ri32(v), NULL);
                print("funcname: "+mk_js_string(funcname)); /* dbg log */
                /* push a dummy symbol to enable local sym storage */
                sym_push1(local_stack, 0, 0, 0);
                /* define parameters */
                sym = sym_find(urs(t, VT_STRUCT_SHIFT));
                /* XXX: the following is x86 dependant -> move it to
                   x86 code gen */
                addr = 8;
                /* if the function returns a structure, then add an
                   implicit pointer parameter */
                func_vt = ri32(sym+Sym_t_o);
                print("func_vt: "+func_vt); /* dbg log */
                if ((func_vt & VT_BTYPE) == VT_STRUCT) {
err();
//                     func_vc = addr;
//                     addr += 4;
                }
                while (sym = ri32(sym+Sym_next_o)) {
                    u = ri32(sym+Sym_t_o);
                    sym_push(ri32(sym+Sym_v_o) & ~SYM_FIELD, 
                             u | VT_LOCAL | VT_LVAL, 
                             addr);
                    size = type_size(u, align);
                    size = (size + 3) & ~3;
                    addr = addr + size;
                }
                loc = 0;
                o(0xE58955); /* push   %ebp, mov    %esp, %ebp */
                a = oad(0xEC81, 0);
                rsym = 0;
                block(NULL, NULL, NULL, NULL, 0);
                gsym(rsym);
                o(0xC3C9); /* leave, ret */
                wi32(a, (-loc + 3) & -4);  /* align local size to word &
                                              save local variables */
// FIXME ljw not needed (reset label stack)
//                 sym_pop(&label_stack, NULL); /* reset label stack */
                sym_pop(local_stack, NULL);
                funcname = mk_c_string(""); /* for safety */
                func_vt = VT_VOID; /* for safety */
                break;
            } else {
                if (b & VT_TYPEDEF) {
                    /* save typedefed type  */
                    /* XXX: test storage specifiers ? */
                    sym_push(ri32(v), t | VT_TYPEDEF, 0);
                } else if ((t & VT_BTYPE) == VT_FUNC) {
                    /* external function definition */
                    external_sym(ri32(v), t);
                } else {
                    /* not lvalue if array */
                    if ((t & VT_ARRAY) == 0) {
                        t = t | VT_LVAL;
                    }
                    if (b & VT_EXTERN) {
                        /* external variable */
                        external_sym(ri32(v), t);
                    } else {
                        u = l;
                        if (t & VT_STATIC)
                            u = VT_CONST;
                        u = u | t;
                        has_init = (tok == mk_char('='));
                        if (has_init)
                            next();
                        addr = decl_initializer_alloc(u, has_init);
                        while(1){
                        if (l == VT_CONST) {
                            /* global scope: see if already defined */
                            sym = sym_find(ri32(v));
                            if (sym == 0) {
                                sym_push(ri32(v), u, addr);
                                break;
                            }
                            if (is_compatible_types(ri32(sym+Sym_t_o), u) == 0)
                                err();
//                                error("incompatible types for redefinition of '%s'", get_tok_str(v, NULL));
                            if ((ri32(sym+Sym_t_o) & VT_FORWARD) == 0)
                                err();
//                                error("redefinition of '%s'", get_tok_str(v, NULL));
                            greloc_patch(sym, addr);
                        } else {
                            sym_push(ri32(v), u, addr);
                        }
                        break;
                        }
                    }
                }
                if (tok != mk_char(',')) {
                    skip(mk_char(';'));
                    break;
                }
                next();
            }
         }
     }
     }
    leave(0);
}

/* put all global symbols in the extern stack and do all the
   resolving which can be done without using external symbols from DLLs */
/* XXX: could try to verify types, but would not to save them in
   extern_stack too */
// void resolve_global_syms(void)
function resolve_global_syms() {
//     Sym *s, *s1, *ext_sym;
    var s;
    var s1;
    var ext_sym;
//     Reloc **p;
    var p;

    enter();

    s = ri32(global_stack+SymStack_top_o);
//     while (s != NULL) {
    while (s != NULL) {
//         s1 = s->prev;
        s1 = ri32(s+Sym_prev_o);
        /* do not save static or typedefed symbols or types */
        if (((ri32(s+Sym_t_o) & (VT_STATIC | VT_TYPEDEF)) == 0) &&
            ((ri32(s+Sym_v_o) & (SYM_FIELD | SYM_STRUCT)) == 0) &&
            ((ri32(s+Sym_v_o) < SYM_FIRST_ANOM) != 0)) {
            ext_sym = sym_find1(extern_stack, ri32(s+Sym_v_o));
            if (ext_sym == 0) {
                /* if the symbol do not exist, we simply save it */
                sym_push1(extern_stack, ri32(s+Sym_v_o), ri32(s+Sym_t_o), ri32(s+Sym_c_o));
            } else if (ri32(ext_sym+Sym_t_o) & VT_FORWARD) {
err();
//                 /* external symbol already exists, but only as forward
//                    definition */
//                 if (!(s->t & VT_FORWARD)) {
//                     /* s is not forward, so we can relocate all symbols */
//                     greloc_patch(ext_sym, s->c);
//                 } else {
//                     /* the two symbols are forward: merge them */
//                     p = (Reloc **)&ext_sym->c;
//                     while (*p != NULL)
//                         p = &(*p)->next;
//                     *p = (Reloc *)s->c;
//                 }
            } else {
err();
//                 /* external symbol already exists and is defined :
//                    patch all references to it */
//                 if (!(s->t & VT_FORWARD))
//                     error("'%s' defined twice", get_tok_str(s->v, NULL));
//                 greloc_patch(s, ext_sym->c);
            }
        }
        s = s1;
    }
    leave(0);
}

/* compile a C file. Return non zero if errors. */
// int tcc_compile_file(const char *filename1)
function tcc_compile_file(filename1) {
//     Sym *define_start;
    var define_start;

    enter();

    print("filename: "+mk_js_string(ri32(filename1)));

    filename = ri32(filename1);

    line_num = 1;
    funcname = mk_c_string("");
    file = v_fopen(filename, mk_c_string("r"));
    if (file == 0) {
        puts("file not found");
        err();
//        error("file '%s' not found", filename);
    }
    include_stack_ptr = include_stack;
// FIXME ljw not needed:
//     ifdef_stack_ptr = ifdef_stack;

//     vtop = vstack - 1;
    vtop = vstack - SValue_size;
    anon_sym = SYM_FIRST_ANOM;

    define_start = ri32(define_stack+SymStack_top_o);
    inp();
    ch = mk_char('\n'); /* needed to parse correctly first preprocessor command */
    next();
    decl(VT_CONST);
    if (tok != -1)
        expect("declaration");
    v_fclose(file);

    /* reset define stack, but leave -Dsymbols (may be incorrect if
       they are undefined) */
    sym_pop(define_stack, define_start);

    resolve_global_syms();

    sym_pop(global_stack, NULL);

    return leave(0);
}

// void resolve_extern_syms(void)
function resolve_extern_syms() {
//     Sym *s, *s1;
    var s;
    var s1;
//     char *str;
    var str;
//     int addr;
    var addr;
    var count;

    enter();

    /* HACK RELOC */
    reloc_global=1;
    s = ri32(extern_stack+SymStack_top_o);
    while (s != NULL) {
        s1 = ri32(s+Sym_prev_o);
        if (ri32(s+Sym_t_o) & VT_FORWARD) {
            /* if there is at least one relocation to do, then find it
               and patch it */
            if (ri32(s+Sym_c_o)) {
//                 str = get_tok_str(s->v, NULL);
                str = get_tok_str(ri32(s+Sym_v_o), NULL);
                print("resolve_extern_syms str: "+mk_js_string(str)); /* dbg log */
                // FIXME ljw dummy address
                addr=0x12345678;

// FIXME ljw dlsym not needed
//                 addr = (int)dlsym(NULL, str);
//                 if (!addr)
//                     error("unresolved external reference '%s'", str);
//                 count=greloc_patch(s, addr);
                count=greloc_patch(s, addr);
                if(reloc){
                    //   printf("resolve_extern_syms: %s %d\n",str,count);
                    print("resolve_extern_syms: "+mk_js_string(str)+" count: "+count); /* dbg log */
                    v_strcpy(global_relocs_table,str);
                    global_relocs_table=global_relocs_table+v_strlen(str)+1;
                    wi32(global_relocs_table,count);
                    global_relocs_table=global_relocs_table+4;
                }
            }
        }
        s = s1;
    }
    reloc_global=0;
    leave(0);
}

// int show_help(void)
//     printf("tcc version 0.9.2 - Tiny C Compiler - Copyright (C) 2001 Fabrice Bellard\n"
//            "usage: tcc [-Idir] [-Dsym] [-llib] [-i infile] infile [infile_args...]\n");
//     return 1;
// }

// void gen_obj(int e){
function gen_obj(e){
  var f;
  var text_len;
  var data_len;
  var reloc_len;
  var global_reloc_len;
  var global_reloc_table_len;
  var prog_rel;
  var data_rel;
  var entrypoint;
  var m0;
  var m1;
  var m2;
  var m3;
  var m4;
  var i;

  enter();

  print("Generating object file\n");

  text_len=v_alloca(4);
  wi32(text_len,ind-prog);

  data_len=v_alloca(4);
  wi32(data_len,glo-glo_base);

  reloc_len=v_alloca(4);
  wi32(reloc_len,relocs-relocs_base);

  global_reloc_len=v_alloca(4);
  wi32(global_reloc_len,global_relocs-global_relocs_base);

  global_reloc_table_len=v_alloca(4);
  wi32(global_reloc_table_len,global_relocs_table-global_relocs_table_base);

  entrypoint=v_alloca(4);
  wi32(entrypoint,e-prog);

  m0=v_alloca(4);
/* js_to_c compiled code cannot handle unsigned ints that are too big to be
   represented by an int32 (below were 0xDEADBE00,01,02...) */
//  wi32(m0,0xDEADBE00);
  wi32(m0,-559038976);

  m1=v_alloca(4);
//  wi32(m1,0xDEADBE01);
  wi32(m1,-559038975);

  m2=v_alloca(4);
//  wi32(m2,0xDEADBE02);
  wi32(m2,-559038974);

  m3=v_alloca(4);
//  wi32(m3,0xDEADBE03);
  wi32(m3,-559038973);

  m4=v_alloca(4);
//  wi32(m4,0xDEADBE04);
  wi32(m4,-559038972);

  f = v_fopen(mk_c_string("tcc_boot.o"), mk_c_string("wb"));
  v_fwrite(entrypoint,1,4,f);
  v_fwrite(text_len,1,4,f);
  v_fwrite(data_len,1,4,f);
  v_fwrite(reloc_len,1,4,f);
  v_fwrite(global_reloc_len,1,4,f);
  v_fwrite(global_reloc_table_len,1,4,f);
  v_fwrite(m0,1,4,f);
  v_fwrite(global_relocs_table_base,1,ri32(global_reloc_table_len),f);

  prog_rel=v_malloc(ri32(text_len));
  data_rel=v_malloc(ri32(data_len));

  v_memcpy(prog_rel,prog,ri32(text_len));
  v_memcpy(data_rel,glo_base,ri32(data_len));

  v_fwrite(m1,1,4,f);
  v_fwrite(relocs_base,1,ri32(reloc_len),f);

  v_fwrite(m2,1,4,f);
  v_fwrite(data_rel,1,ri32(data_len),f);

  v_fwrite(m3,1,4,f);
  v_fwrite(global_relocs_base,1,ri32(global_reloc_len),f);

  for(i=0;i<ri32(reloc_len);i=i+12){
    wi32(prog_rel+ri32(relocs_base+i),0); 
  }
  for(i=0;i<ri32(global_reloc_len);i=i+8){
    wi32(prog_rel+ri32(global_relocs_base+i+4),0); 
  }

  v_fwrite(m4,1,4,f);
  v_fwrite(prog_rel,1,ri32(text_len),f);
  v_fclose(f);
  leave(0);
}

var prog_rel;
var data_rel;

// int main(int argc, char **argv)
function tcc_main(argc,argv){
//     Sym *s;
    var s;
//     int (*t)();
    var t;
//     char *p, *r, *outfile;
    var p;
    var r;
    var outfile;

    var optind;
    var c;
    var loader;

    init_globals();
    init_i386_gen();

    enter();

     puts("tcc 1_7 start");

    s=v_alloca(4);
    t=v_alloca(4);
    outfile=v_alloca(4);

    wi32(include_paths,mk_c_string("../tcc_1_7/"));
    nb_include_paths = 1;

    /* add all tokens */
    tok_ident = TOK_IDENT;
    p = get_tok_list_str();

    while (ri8(p)) {
        r = p;
        while (c=ri8(r)){r=r+1;}
        r=r+1;
        tok_alloc(p, r - p - 1);
        p = r;
    }
    /* standard defines */
    define_symbol(mk_c_string("__STDC__"));
    define_symbol(mk_c_string("__i386__"));
    /* tiny C specific defines */
    define_symbol(mk_c_string("__TINYC__"));

    glo=v_malloc(DATA_SIZE);
    glo_base=glo;
    print("glo: "+to_hex(glo));
    v_memset(glo, 0, DATA_SIZE);
    prog=v_malloc(TEXT_SIZE);
    ind = prog;
    print("prog: "+to_hex(prog));

    optind = 1;
    outfile = NULL;

    loader=0;
    while (1) {
        if (optind >= argc) {
err();
//         return show_help();
         }
         r = ri32(argv+(optind*4));
         if (ri8(r) != mk_char('-')) {
             break;
         }
         optind=optind+1;
         if (ri8(r+1) == mk_char('I')) {
err();
//             if (nb_include_paths >= INCLUDE_PATHS_MAX)
//                 error("too many include paths");
//             include_paths[nb_include_paths++] = r + 2;
         } else if (ri8(r+1) == mk_char('D')) {
err();
//             define_symbol(r + 2);
         } else if (ri8(r+1) == mk_char('i')) {
err();
//             if (optind >= argc)
//                 return show_help();
//             tcc_compile_file(argv[optind++]);
         } else if (ri8(r+1) == mk_char('r')) {
             reloc=1;
         } else if (ri8(r+1) == mk_char('R')) {
err();
//             t=(int (*)())load_obj();
//             loader=1;
         } else {
err();
//             fprintf(stderr, "invalid option -- '%s'\n", r);
//             exit(1);
//         }
         }
    }

    if(reloc){
        global_relocs=v_malloc(64*1024);
        global_relocs_base=global_relocs;

        print("global_relocs "+to_hex(global_relocs)); /* dbg log */

        global_relocs_table=v_malloc(64*1024);
        global_relocs_table_base=global_relocs_table;

        relocs=v_malloc(64*1024);
        relocs_base=relocs;
    }

    tcc_compile_file(argv+(4*optind));
    puts("tcc 1_7 compile done");

    resolve_extern_syms();
    s = sym_find1(extern_stack, TOK_MAIN);
    if ((s == 0) || (ri32(s+Sym_t_o) & VT_FORWARD))
        error("main() not defined");

    print("main: "+to_hex(ri32(s+Sym_c_o))); /* dbg log */

    if(reloc){
        gen_obj(ri32(s+Sym_c_o));
    }

    return leave(0);
}
