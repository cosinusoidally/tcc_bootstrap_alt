/*
 *  TCC - Tiny C Compiler
 * 
 *  Copyright (c) 2001 Fabrice Bellard
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
    
    glo = (int)mmap(NULL, DATA_SIZE,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS,
                -1, 0);
    glo_base=glo;
    printf("glo: %x %x\n",glo,glo_base);
    memset((void *)glo, 0, DATA_SIZE);
    prog = (int)mmap(NULL, TEXT_SIZE,
                PROT_EXEC | PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS,
                -1, 0);
    ind = prog;
    printf("prog: %x \n",prog);

    optind = 1;
    outfile = NULL;
int loader=0;
    while (1) {
printf("argc %d\n",argc);
        if (optind >= argc) {
        return show_help();
        }
        r = argv[optind];
        if (r[0] != '-')
            break;
        optind++;
        if (r[1] == 'I') {
            if (nb_include_paths >= INCLUDE_PATHS_MAX)
                error("too many include paths");
            include_paths[nb_include_paths++] = r + 2;
        } else if (r[1] == 'D') {
            define_symbol(r + 2);
        } else if (r[1] == 'i') {
            if (optind >= argc)
                return show_help();
            tcc_compile_file(argv[optind++]);
        } else if (r[1] == 'r') {
            reloc=1;
        } else if (r[1] == 'R') {
            t=(int (*)())load_obj();
            loader=1;
        } else {
            fprintf(stderr, "invalid option -- '%s'\n", r);
            exit(1);
        }
    }

if(loader){
  printf("running loader\n");
  return (*t)(argc - optind, argv + optind);
}
if(reloc){
global_relocs=(int)malloc(64*1024);
global_relocs_base=global_relocs;

printf("global_relocs %d\n",global_relocs);

global_relocs_table=(int)malloc(64*1024);
global_relocs_table_base=global_relocs_table;

relocs=(int)malloc(64*1024);
relocs_base=relocs;

};

    tcc_compile_file(argv[optind]);
    puts("tcc 1_7 compile done");

    resolve_extern_syms();
    s = sym_find1(&extern_stack, TOK_MAIN);
    if (!s || (s->t & VT_FORWARD))
        error("main() not defined");

if(reloc){
  gen_obj(s->c);
}

    t = (int (*)())s->c;
    return (*t)(argc - optind, argv + optind);
}
