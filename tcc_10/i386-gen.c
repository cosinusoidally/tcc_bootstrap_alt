/*
 *  X86 code generator for TCC
 * 
 *  Copyright (c) 2001, 2002 Fabrice Bellard
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

/* number of available registers */
#define NB_REGS             4

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#define RC_INT     0x0001 /* generic integer register */
#define RC_FLOAT   0x0002 /* generic float register */
#define RC_EAX     0x0004
#define RC_ST0     0x0008 
#define RC_ECX     0x0010
#define RC_EDX     0x0020
#define RC_IRET    RC_EAX /* function return: integer register */
#define RC_LRET    RC_EDX /* function return: second integer register */
#define RC_FRET    RC_ST0 /* function return: float register */

/* pretty names for the registers */
enum {
    REG_EAX = 0,
    REG_ECX,
    REG_EDX,
    REG_ST0,
};

int reg_classes[NB_REGS] = {
    /* eax */ RC_INT | RC_EAX,
    /* ecx */ RC_INT | RC_ECX,
    /* edx */ RC_INT | RC_EDX,
    /* st0 */ RC_FLOAT | RC_ST0,
};

/* return registers for function */
#define REG_IRET REG_EAX /* single word int return register */
#define REG_LRET REG_EDX /* second word return register (for long long) */
#define REG_FRET REG_ST0 /* float return register */

/* defined if function parameters must be evaluated in reverse order */
#define INVERT_FUNC_PARAMS

/* defined if structures are passed as pointers. Otherwise structures
   are directly pushed on stack. */
//#define FUNC_STRUCT_PARAM_AS_PTR

/* pointer size, in bytes */
#define PTR_SIZE 4

/* long double size and alignment, in bytes */
#define LDOUBLE_SIZE  12
#define LDOUBLE_ALIGN 4

/* relocation type for 32 bit data relocation */
#define R_DATA_32 R_386_32

/* function call context */
typedef struct GFuncContext {
    int args_size;
    int func_call; /* func call type (FUNC_STDCALL or FUNC_CDECL) */
} GFuncContext;

/******************************************************/

static unsigned long func_sub_sp_offset;
static unsigned long func_bound_offset;
static int func_ret_sub;

/* XXX: make it faster ? */
void g(int c)
{
    int ind1;
    ind1 = ind + 1;
    if (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind] = c;
    ind = ind1;
}

void o(int c)
{
    while (c) {
        g(c);
        c = c / 256;
    }
}

void gen_le32(int c)
{
    g(c);
    g(c >> 8);
    g(c >> 16);
    g(c >> 24);
}

/* output a symbol and patch all calls to it */
void gsym_addr(int t, int a)
{
    int n, *ptr;
    while (t) {
        ptr = (int *)(cur_text_section->data + t);
        n = *ptr; /* next value */
        *ptr = a - t - 4;
        t = n;
    }
}

void gsym(int t)
{
    gsym_addr(t, ind);
}

/* psym is used to put an instruction with a data field which is a
   reference to a symbol. It is in fact the same as oad ! */
#define psym oad

/* instruction + 4 bytes data. Return the address of the data */
int oad(int c, int s)
{
    int ind1;

    o(c);
    ind1 = ind + 4;
    if (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    *(int *)(cur_text_section->data + ind) = s;
    s = ind;
    ind = ind1;
    return s;
}

/* output constant with relocation if 'r & VT_SYM' is true */
void gen_addr32(int r, int c)
{
    if (!(r & VT_SYM)) {
        gen_le32(c);
    } else {
        greloc(cur_text_section, 
               (Sym *)c, ind, R_386_32);
        gen_le32(0);
    }
}

/* generate a modrm reference. 'op_reg' contains the addtionnal 3
   opcode bits */
void gen_modrm(int op_reg, int r, int c)
{
    op_reg = op_reg << 3;
    if ((r & VT_VALMASK) == VT_CONST) {
        /* constant memory reference */
        o(0x05 | op_reg);
        gen_addr32(r, c);
    } else if ((r & VT_VALMASK) == VT_LOCAL) {
        /* currently, we use only ebp as base */
        if (c == (char)c) {
            /* short reference */
            o(0x45 | op_reg);
            g(c);
        } else {
            oad(0x85 | op_reg, c);
        }
    } else {
        g(0x00 | op_reg | (r & VT_VALMASK));
    }
}


/* load 'r' from value 'sv' */
void load(int r, SValue *sv)
{
    int v, t, ft, fc, fr;
    SValue v1;

    fr = sv->r;
    ft = sv->t;
    fc = sv->c.ul;

    v = fr & VT_VALMASK;
    if (fr & VT_LVAL) {
        if (v == VT_LLOCAL) {
            v1.t = VT_INT;
            v1.r = VT_LOCAL | VT_LVAL;
            v1.c.ul = fc;
            load(r, &v1);
            fr = r;
        }
        if ((ft & VT_BTYPE) == VT_FLOAT) {
            o(0xd9); /* flds */
            r = 0;
        } else if ((ft & VT_BTYPE) == VT_DOUBLE) {
            o(0xdd); /* fldl */
            r = 0;
        } else if ((ft & VT_BTYPE) == VT_LDOUBLE) {
            o(0xdb); /* fldt */
            r = 5;
        } else if ((ft & VT_TYPE) == VT_BYTE) {
            o(0xbe0f);   /* movsbl */
        } else if ((ft & VT_TYPE) == (VT_BYTE | VT_UNSIGNED)) {
            o(0xb60f);   /* movzbl */
        } else if ((ft & VT_TYPE) == VT_SHORT) {
            o(0xbf0f);   /* movswl */
        } else if ((ft & VT_TYPE) == (VT_SHORT | VT_UNSIGNED)) {
            o(0xb70f);   /* movzwl */
        } else {
            o(0x8b);     /* movl */
        }
        gen_modrm(r, fr, fc);
    } else {
        if (v == VT_CONST) {
            o(0xb8 + r); /* mov $xx, r */
            gen_addr32(fr, fc);
        } else if (v == VT_LOCAL) {
            o(0x8d); /* lea xxx(%ebp), r */
            gen_modrm(r, VT_LOCAL, fc);
        } else if (v == VT_CMP) {
            oad(0xb8 + r, 0); /* mov $0, r */
            o(0x0f); /* setxx %br */
            o(fc);
            o(0xc0 + r);
        } else if (v == VT_JMP || v == VT_JMPI) {
            t = v & 1;
            oad(0xb8 + r, t); /* mov $1, r */
            oad(0xe9, 5); /* jmp after */
            gsym(fc);
            oad(0xb8 + r, t ^ 1); /* mov $0, r */
        } else if (v != r) {
            o(0x89);
            o(0xc0 + r + v * 8); /* mov v, r */
        }
    }
}

/* store register 'r' in lvalue 'v' */
void store(int r, SValue *v)
{
    int fr, bt, ft, fc;

    ft = v->t;
    fc = v->c.ul;
    fr = v->r & VT_VALMASK;
    bt = ft & VT_BTYPE;
    /* XXX: incorrect if float reg to reg */
    if (bt == VT_FLOAT) {
        o(0xd9); /* fsts */
        r = 2;
    } else if (bt == VT_DOUBLE) {
        o(0xdd); /* fstpl */
        r = 2;
    } else if (bt == VT_LDOUBLE) {
        o(0xc0d9); /* fld %st(0) */
        o(0xdb); /* fstpt */
        r = 7;
    } else {
        if (bt == VT_SHORT)
            o(0x66);
        if (bt == VT_BYTE)
            o(0x88);
        else
            o(0x89);
    }
    if (fr == VT_CONST ||
        fr == VT_LOCAL ||
        (v->r & VT_LVAL)) {
        gen_modrm(r, v->r, fc);
    } else if (fr != r) {
        o(0xc0 + fr + r * 8); /* mov r, fr */
    }
}

/* start function call and return function call context */
void gfunc_start(GFuncContext *c, int func_call)
{
    c->args_size = 0;
    c->func_call = func_call;
}

/* push function parameter which is in (vtop->t, vtop->c). Stack entry
   is then popped. */
void gfunc_param(GFuncContext *c)
{
    int size, align, r;

    if ((vtop->t & VT_BTYPE) == VT_STRUCT) {
        size = type_size(vtop->t, &align);
        /* align to stack align size */
        size = (size + 3) & ~3;
        /* allocate the necessary size on stack */
        oad(0xec81, size); /* sub $xxx, %esp */
        /* generate structure store */
        r = get_reg(RC_INT);
        o(0x89); /* mov %esp, r */
        o(0xe0 + r);
        vset(vtop->t, r | VT_LVAL, 0);
        vswap();
        vstore();
        c->args_size += size;
    } else if (is_float(vtop->t)) {
        gv(RC_FLOAT); /* only one float register */
        if ((vtop->t & VT_BTYPE) == VT_FLOAT)
            size = 4;
        else if ((vtop->t & VT_BTYPE) == VT_DOUBLE)
            size = 8;
        else
            size = 12;
        oad(0xec81, size); /* sub $xxx, %esp */
        if (size == 12)
            o(0x7cdb);
        else
            o(0x5cd9 + size - 4); /* fstp[s|l] 0(%esp) */
        g(0x24);
        g(0x00);
        c->args_size += size;
    } else {
        /* simple type (currently always same size) */
        /* XXX: implicit cast ? */
        r = gv(RC_INT);
        if ((vtop->t & VT_BTYPE) == VT_LLONG) {
            size = 8;
            o(0x50 + vtop->r2); /* push r */
        } else {
            size = 4;
        }
        o(0x50 + r); /* push r */
        c->args_size += size;
    }
    vtop--;
}

static void gadd_sp(int val)
{
    if (val == (char)val) {
        o(0xc483);
        g(val);
    } else {
        oad(0xc481, val); /* add $xxx, %esp */
    }
}

/* generate function call with address in (vtop->t, vtop->c) and free function
   context. Stack entry is popped */
void gfunc_call(GFuncContext *c)
{
    int r;
    if ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        /* constant case */
        if (vtop->r & VT_SYM) {
            /* relocation case */
            greloc(cur_text_section, vtop->c.sym, 
                   ind + 1, R_386_PC32);
            oad(0xe8, -4);
        } else {
            oad(0xe8, vtop->c.ul - ind - 5);
        }
    } else {
        /* otherwise, indirect call */
        r = gv(RC_INT);
        o(0xff); /* call *r */
        o(0xd0 + r);
    }
    if (c->args_size && c->func_call == FUNC_CDECL)
        gadd_sp(c->args_size);
    vtop--;
}

/* generate function prolog of type 't' */
void gfunc_prolog(int t)
{
    int addr, align, size, u, func_call;
    Sym *sym;

    sym = sym_find((unsigned)t >> VT_STRUCT_SHIFT);
    func_call = sym->r;
    addr = 8;
    /* if the function returns a structure, then add an
       implicit pointer parameter */
    func_vt = sym->t;
    if ((func_vt & VT_BTYPE) == VT_STRUCT) {
        func_vc = addr;
        addr += 4;
    }
    /* define parameters */
    while ((sym = sym->next) != NULL) {
        u = sym->t;
        sym_push(sym->v & ~SYM_FIELD, u,
                 VT_LOCAL | VT_LVAL, addr);
        size = type_size(u, &align);
        size = (size + 3) & ~3;
#ifdef FUNC_STRUCT_PARAM_AS_PTR
        /* structs are passed as pointer */
        if ((u & VT_BTYPE) == VT_STRUCT) {
            size = 4;
        }
#endif
        addr += size;
    }
    func_ret_sub = 0;
    /* pascal type call ? */
    if (func_call == FUNC_STDCALL)
        func_ret_sub = addr - 8;
    o(0xe58955); /* push   %ebp, mov    %esp, %ebp */
    func_sub_sp_offset = oad(0xec81, 0); /* sub $xxx, %esp */
    /* leave some room for bound checking code */
    if (do_bounds_check) {
        oad(0xb8, 0); /* lbound section pointer */
        oad(0xb8, 0); /* call to function */
        func_bound_offset = lbounds_section->data_offset;
    }
}

/* generate function epilog */
void gfunc_epilog(void)
{
#ifdef CONFIG_TCC_BCHECK
    if (do_bounds_check && func_bound_offset != lbounds_section->data_offset) {
        int saved_ind;
        int *bounds_ptr;
        Sym *sym, *sym_data;
        /* add end of table info */
        bounds_ptr = section_ptr_add(lbounds_section, sizeof(int));
        *bounds_ptr = 0;
        /* generate bound local allocation */
        saved_ind = ind;
        ind = func_sub_sp_offset + 4;
        sym_data = get_sym_ref(char_pointer_type, lbounds_section, 
                               func_bound_offset, lbounds_section->data_offset);
        greloc(cur_text_section, sym_data,
               ind + 1, R_386_32);
        oad(0xb8, 0); /* mov %eax, xxx */
        sym = external_sym(TOK___bound_local_new, func_old_type, 0);
        greloc(cur_text_section, sym, 
               ind + 1, R_386_PC32);
        oad(0xe8, -4);
        ind = saved_ind;
        /* generate bound check local freeing */
        o(0x5250); /* save returned value, if any */
        greloc(cur_text_section, sym_data,
               ind + 1, R_386_32);
        oad(0xb8, 0); /* mov %eax, xxx */
        sym = external_sym(TOK___bound_local_delete, func_old_type, 0);
        greloc(cur_text_section, sym, 
               ind + 1, R_386_PC32);
        oad(0xe8, -4);
        o(0x585a); /* restore returned value, if any */
    }
#endif
    o(0xc9); /* leave */
    if (func_ret_sub == 0) {
        o(0xc3); /* ret */
    } else {
        o(0xc2); /* ret n */
        g(func_ret_sub);
        g(func_ret_sub >> 8);
    }
    /* align local size to word & save local variables */
    *(int *)(cur_text_section->data + func_sub_sp_offset) = (-loc + 3) & -4; 
}

/* generate a jump to a label */
int gjmp(int t)
{
    return psym(0xe9, t);
}

/* generate a jump to a fixed address */
void gjmp_addr(int a)
{
    oad(0xe9, a - ind - 5);
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
int gtst(int inv, int t)
{
    int v, *p;
    v = vtop->r & VT_VALMASK;
    if (v == VT_CMP) {
        /* fast case : can jump directly since flags are set */
        g(0x0f);
        t = psym((vtop->c.i - 16) ^ inv, t);
    } else if (v == VT_JMP || v == VT_JMPI) {
        /* && or || optimization */
        if ((v & 1) == inv) {
            /* insert vtop->c jump list in t */
            p = &vtop->c.i;
            while (*p != 0)
                p = (int *)(cur_text_section->data + *p);
            *p = t;
            t = vtop->c.i;
        } else {
            t = gjmp(t);
            gsym(vtop->c.i);
        }
    } else {
        if (is_float(vtop->t)) {
            vpushi(0);
            gen_op(TOK_NE);
        }
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant jmp optimization */
            if ((vtop->c.i != 0) != inv) 
                t = gjmp(t);
        } else {
            v = gv(RC_INT);
            o(0x85);
            o(0xc0 + v * 9);
            g(0x0f);
            t = psym(0x85 ^ inv, t);
        }
    }
    vtop--;
    return t;
}

/* generate an integer binary operation */
void gen_opi(int op)
{
    int r, fr, opc, c;

    switch(op) {
    case '+':
    case TOK_ADDC1: /* add with carry generation */
        opc = 0;
    gen_op8:
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant case */
            vswap();
            r = gv(RC_INT);
            vswap();
            c = vtop->c.i;
            if (c == (char)c) {
                /* XXX: generate inc and dec for smaller code ? */
                o(0x83);
                o(0xc0 | (opc << 3) | r);
                g(c);
            } else {
                o(0x81);
                oad(0xc0 | (opc << 3) | r, c);
            }
        } else {
            gv2(RC_INT, RC_INT);
            r = vtop[-1].r;
            fr = vtop[0].r;
            o((opc << 3) | 0x01);
            o(0xc0 + r + fr * 8); 
        }
        vtop--;
        if (op >= TOK_ULT && op <= TOK_GT) {
            vtop--;
            vset(VT_INT, VT_CMP, op);
        }
        break;
    case '-':
    case TOK_SUBC1: /* sub with carry generation */
        opc = 5;
        goto gen_op8;
    case TOK_ADDC2: /* add with carry use */
        opc = 2;
        goto gen_op8;
    case TOK_SUBC2: /* sub with carry use */
        opc = 3;
        goto gen_op8;
    case '&':
        opc = 4;
        goto gen_op8;
    case '^':
        opc = 6;
        goto gen_op8;
    case '|':
        opc = 1;
        goto gen_op8;
    case '*':
        gv2(RC_INT, RC_INT);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        o(0xaf0f); /* imul fr, r */
        o(0xc0 + fr + r * 8);
        break;
    case TOK_SHL:
        opc = 4;
        goto gen_shift;
    case TOK_SHR:
        opc = 5;
        goto gen_shift;
    case TOK_SAR:
        opc = 7;
    gen_shift:
        opc = 0xc0 | (opc << 3);
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant case */
            vswap();
            r = gv(RC_INT);
            vswap();
            c = vtop->c.i & 0x1f;
            o(0xc1); /* shl/shr/sar $xxx, r */
            o(opc | r);
            g(c);
        } else {
            /* we generate the shift in ecx */
            gv2(RC_INT, RC_ECX);
            r = vtop[-1].r;
            o(0xd3); /* shl/shr/sar %cl, r */
            o(opc | r);
        }
        vtop--;
        break;
    case '/':
    case TOK_UDIV:
    case TOK_PDIV:
    case '%':
    case TOK_UMOD:
    case TOK_UMULL:
        /* first operand must be in eax */
        /* XXX: need better constraint for second operand */
        gv2(RC_EAX, RC_ECX);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        save_reg(REG_EDX);
        if (op == TOK_UMULL) {
            o(0xf7); /* mul fr */
            o(0xe0 + fr);
            vtop->r2 = REG_EDX;
            r = REG_EAX;
        } else {
            if (op == TOK_UDIV || op == TOK_UMOD) {
                o(0xf7d231); /* xor %edx, %edx, div fr, %eax */
                o(0xf0 + fr);
            } else {
                o(0xf799); /* cltd, idiv fr, %eax */
                o(0xf8 + fr);
            }
            if (op == '%' || op == TOK_UMOD)
                r = REG_EDX;
            else
                r = REG_EAX;
        }
        vtop->r = r;
        break;
    default:
        opc = 7;
        goto gen_op8;
    }
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranted to have the same floating point type */
/* XXX: need to use ST1 too */
void gen_opf(int op)
{
    int a, ft, fc, swapped, r;

    /* convert constants to memory references */
    if ((vtop[-1].r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        vswap();
        gv(RC_FLOAT);
        vswap();
    }
    if ((vtop[0].r & (VT_VALMASK | VT_LVAL)) == VT_CONST)
        gv(RC_FLOAT);

    /* must put at least one value in the floating point register */
    if ((vtop[-1].r & VT_LVAL) &&
        (vtop[0].r & VT_LVAL)) {
        vswap();
        gv(RC_FLOAT);
        vswap();
    }
    swapped = 0;
    /* swap the stack if needed so that t1 is the register and t2 is
       the memory reference */
    if (vtop[-1].r & VT_LVAL) {
        vswap();
        swapped = 1;
    }
    if (op >= TOK_ULT && op <= TOK_GT) {
        /* load on stack second operand */
        load(REG_ST0, vtop);
        save_reg(REG_EAX); /* eax is used by FP comparison code */
        if (op == TOK_GE || op == TOK_GT)
            swapped = !swapped;
        else if (op == TOK_EQ || op == TOK_NE)
            swapped = 0;
        if (swapped)
            o(0xc9d9); /* fxch %st(1) */
        o(0xe9da); /* fucompp */
        o(0xe0df); /* fnstsw %ax */
        if (op == TOK_EQ) {
            o(0x45e480); /* and $0x45, %ah */
            o(0x40fC80); /* cmp $0x40, %ah */
        } else if (op == TOK_NE) {
            o(0x45e480); /* and $0x45, %ah */
            o(0x40f480); /* xor $0x40, %ah */
            op = TOK_NE;
        } else if (op == TOK_GE || op == TOK_LE) {
            o(0x05c4f6); /* test $0x05, %ah */
            op = TOK_EQ;
        } else {
            o(0x45c4f6); /* test $0x45, %ah */
            op = TOK_EQ;
        }
        vtop--;
        vtop->r = VT_CMP;
        vtop->c.i = op;
    } else {
        /* no memory reference possible for long double operations */
        if ((vtop->t & VT_BTYPE) == VT_LDOUBLE) {
            load(REG_ST0, vtop);
            swapped = !swapped;
        }
        
        switch(op) {
        default:
        case '+':
            a = 0;
            break;
        case '-':
            a = 4;
            if (swapped)
                a++;
            break;
        case '*':
            a = 1;
            break;
        case '/':
            a = 6;
            if (swapped)
                a++;
            break;
        }
        ft = vtop->t;
        fc = vtop->c.ul;
        if ((ft & VT_BTYPE) == VT_LDOUBLE) {
            o(0xde); /* fxxxp %st, %st(1) */
            o(0xc1 + (a << 3));
        } else {
            /* if saved lvalue, then we must reload it */
            r = vtop->r;
            if ((r & VT_VALMASK) == VT_LLOCAL) {
                SValue v1;
                r = get_reg(RC_INT);
                v1.t = VT_INT;
                v1.r = VT_LOCAL | VT_LVAL;
                v1.c.ul = fc;
                load(r, &v1);
                fc = 0;
            }

            if ((ft & VT_BTYPE) == VT_DOUBLE)
                o(0xdc);
            else
                o(0xd8);
            gen_modrm(a, r, fc);
        }
        vtop--;
    }
}

/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
void gen_cvt_itof(int t)
{
    save_reg(REG_ST0);
    gv(RC_INT);
    if ((vtop->t & VT_BTYPE) == VT_LLONG) {
        /* signed long long to float/double/long double (unsigned case
           is handled generically) */
        o(0x50 + vtop->r2); /* push r2 */
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x242cdf); /* fildll (%esp) */
        o(0x08c483); /* add $8, %esp */
    } else if ((vtop->t & (VT_BTYPE | VT_UNSIGNED)) == 
               (VT_INT | VT_UNSIGNED)) {
        /* unsigned int to float/double/long double */
        o(0x6a); /* push $0 */
        g(0x00);
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x242cdf); /* fildll (%esp) */
        o(0x08c483); /* add $8, %esp */
    } else {
        /* int to float/double/long double */
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x2404db); /* fildl (%esp) */
        o(0x04c483); /* add $4, %esp */
    }
    vtop->r = REG_ST0;
}

/* convert fp to int 't' type */
/* XXX: handle long long case */
void gen_cvt_ftoi(int t)
{
    int r, r2, size;
    Sym *sym;

    gv(RC_FLOAT);
    if (t != VT_INT)
        size = 8;
    else 
        size = 4;
    
    o(0x2dd9); /* ldcw xxx */
    sym = external_sym(TOK___tcc_int_fpu_control, 
                       VT_SHORT | VT_UNSIGNED, VT_LVAL);
    greloc(cur_text_section, sym, 
           ind, R_386_32);
    gen_le32(0);
    
    oad(0xec81, size); /* sub $xxx, %esp */
    if (size == 4)
        o(0x1cdb); /* fistpl */
    else
        o(0x3cdf); /* fistpll */
    o(0x24);
    o(0x2dd9); /* ldcw xxx */
    sym = external_sym(TOK___tcc_fpu_control, 
                       VT_SHORT | VT_UNSIGNED, VT_LVAL);
    greloc(cur_text_section, sym, 
           ind, R_386_32);
    gen_le32(0);

    r = get_reg(RC_INT);
    o(0x58 + r); /* pop r */
    if (size == 8) {
        if (t == VT_LLONG) {
            vtop->r = r; /* mark reg as used */
            r2 = get_reg(RC_INT);
            o(0x58 + r2); /* pop r2 */
            vtop->r2 = r2;
        } else {
            o(0x04c483); /* add $4, %esp */
        }
    }
    vtop->r = r;
}

/* convert from one floating point type to another */
void gen_cvt_ftof(int t)
{
    /* all we have to do on i386 is to put the float in a register */
    gv(RC_FLOAT);
}

/* bound check support functions */
#ifdef CONFIG_TCC_BCHECK

/* generate a bounded pointer addition */
void gen_bounded_ptr_add(void)
{
    Sym *sym;

    /* prepare fast i386 function call (args in eax and edx) */
    gv2(RC_EAX, RC_EDX);
    /* save all temporary registers */
    vtop -= 2;
    save_regs(0);
    /* do a fast function call */
    sym = external_sym(TOK___bound_ptr_add, func_old_type, 0);
    greloc(cur_text_section, sym, 
           ind + 1, R_386_PC32);
    oad(0xe8, -4);
    /* returned pointer is in eax */
    vtop++;
    vtop->r = REG_EAX | VT_BOUNDED;
    /* address of bounding function call point */
    vtop->c.ul = (cur_text_section->reloc->data_offset - sizeof(Elf32_Rel)); 
}

/* patch pointer addition in vtop so that pointer dereferencing is
   also tested */
void gen_bounded_ptr_deref(void)
{
    int func;
    int size, align;
    Elf32_Rel *rel;
    Sym *sym;

    size = 0;
    /* XXX: put that code in generic part of tcc */
    if (!is_float(vtop->t)) {
        if (vtop->r & VT_LVAL_BYTE)
            size = 1;
        else if (vtop->r & VT_LVAL_SHORT)
            size = 2;
    }
    if (!size)
        size = type_size(vtop->t, &align);
    switch(size) {
    case  1: func = TOK___bound_ptr_indir1; break;
    case  2: func = TOK___bound_ptr_indir2; break;
    case  4: func = TOK___bound_ptr_indir4; break;
    case  8: func = TOK___bound_ptr_indir8; break;
    case 12: func = TOK___bound_ptr_indir12; break;
    case 16: func = TOK___bound_ptr_indir16; break;
    default:
        error("unhandled size when derefencing bounded pointer");
        func = 0;
        break;
    }

    /* patch relocation */
    /* XXX: find a better solution ? */
    rel = (Elf32_Rel *)(cur_text_section->reloc->data + vtop->c.ul);
    sym = external_sym(func, func_old_type, 0);
    if (!sym->c)
        put_extern_sym(sym, NULL, 0, 0);
    rel->r_info = ELF32_R_INFO(sym->c, ELF32_R_TYPE(rel->r_info));
}
#endif

/* end of X86 code generator */
/*************************************************************/

