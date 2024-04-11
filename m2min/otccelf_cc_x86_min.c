/* this is otccelf and support files rolled in to one file */

/*
  Obfuscated Tiny C Compiler with ELF output

  Copyright (C) 2001-2003 Fabrice Bellard

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product and its documentation 
     *is* required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* Copyright (C) 2016 Jeremiah Orians
 * This file is part of M2-Planet.
 *
 * M2-Planet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * M2-Planet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with M2-Planet.  If not, see <http://www.gnu.org/licenses/>.
 */

int stdin;
int stdout;
int stderr;
int EOF;
int NULL;
int EXIT_FAILURE;
int EXIT_SUCCESS;
int TRUE;
int FALSE;

int int_size;

void puts_num(int x);

int add(int a, int b){
/*	return a + b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"add_eax,ebx"
		"ret"
	);
}

int sub(int a, int b){
/*	return a - b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"sub_ebx,eax"
		"mov_eax,ebx"
		"ret"
	);
}

int mul(int a, int b){
/*	return a * b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"imul_ebx"
		"ret"
	);
}

int shl(int a, int b){
/*	return a << b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"mov_ecx,eax"
		"mov_eax,ebx"
		"sal_eax,cl"
		"ret"
	);
}

int shr(int a, int b){
/*	return a >> b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"mov_ecx,eax"
		"mov_eax,ebx"
		"sar_eax,cl"
		"ret"
	);
}

int lt(int a, int b){
/*	return a < b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"cmp"
		"setl_al"
		"movzx_eax,al"
		"ret"
	);
}

int gt(int a, int b){
/*	return a > b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"cmp"
		"setg_al"
		"movzx_eax,al"
		"ret"
	);
}

int lte(int a, int b){
/*	return a <= b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"cmp"
		"setle_al"
		"movzx_eax,al"
		"ret"
	);
}

int gte(int a, int b){
/*	return a >= b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"cmp"
		"setge_al"
		"movzx_eax,al"
		"ret"
	);
}

int and(int a, int b){
/*	return a & b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"and_eax,ebx"
		"ret"
	);
}

int or(int a, int b){
/*	return a | b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"or_eax,ebx"
		"ret"
	);
}

int ri8(int o) {
/*
  char *h = 0;
  return h[o] & 0xFF;
*/
	asm(
		"mov_eax, %0"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-12"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"pop_ebx"
		"add_eax,ebx"
		"movsx_eax,BYTE_PTR_[eax]"
		"push_eax"
		"mov_eax, %0xFF"
		"pop_ebx"
		"and_eax,ebx"
		"pop_ebx"
		"ret"
	);
}

int eq(int a, int b){
/*	return a == b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"cmp"
		"sete_al"
		"movzx_eax,al"
		"ret"
	);
}

int neq(int a, int b){
/*	return a != b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"cmp"
		"setne_al"
		"movzx_eax,al"
		"ret"
	);
}

int xor(int a, int b){
/*	return a ^ b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"xor_eax,ebx"
		"ret"
	);
}

int wi8(int o,int v) {
/*
  char *h = 0;
  h[o]=v;
  return;
*/
	asm(
		"mov_eax, %0"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-16"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"pop_ebx"
		"add_eax,ebx"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"mov_[ebx],al"
		"pop_ebx"
		"ret"
	);
}

int div(int a, int b){
/*	return a / b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"xchg_ebx,eax"
		"cdq"
		"idiv_ebx"
		"ret"
	);
}

int mod(int a, int b){
/*	return a % b; */
	asm(
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"pop_ebx"
		"xchg_ebx,eax"
		"cdq"
		"idiv_ebx"
		"mov_eax,edx"
		"ret"
	);
}

int wi32(int o, int v) {
  wi8(o, and(v, 0xFF));
  v = shr(v, 8);
  wi8(add(o, 1), and(v, 0xFF));
  v = shr(v, 8);
  wi8(add(o, 2), and(v, 0xFF));
  v = shr(v, 8);
  wi8(add(o, 3), and(v, 0xFF));
}

int ri32(int o) {
  return or(or(and(ri8(o), 255),
		shl(and(ri8(add(o, 1)), 255), 8)),
		or(shl(and(ri8(add(o, 2)), 255), 16),
		shl(and(ri8(add(o, 3)), 255), 24)));
}

int fgetc(int f)
{
	asm("mov_eax, %3"
	    "lea_ebx,[esp+DWORD] %4"
	    "mov_ebx,[ebx]"
	    "push_ebx"
	    "mov_ecx,esp"
	    "mov_edx, %1"
	    "int !0x80"
	    "test_eax,eax"
	    "pop_eax"
	    "jne %FUNCTION_fgetc_Done"
	    "mov_eax, %-1"
	    ":FUNCTION_fgetc_Done");
}

void fputc(int s, int f)
{
	asm("mov_eax, %4"
	    "lea_ebx,[esp+DWORD] %4"
	    "mov_ebx,[ebx]"
	    "lea_ecx,[esp+DWORD] %8"
	    "mov_edx, %1"
	    "int !0x80");
}

void fputs(int si, int f)
{
/*
	char *s;
	s = si;
	while(0 != s[0])
	{
		fputc(s[0], f);
		s = s + 1;
	}
*/
/* annoyingly changing the above to use ri8 would segfault hence asm: */
	asm(
		":WHILE_fputs_0"
		"mov_eax, %0"
		"push_eax        #_common_recursion"
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax        #_common_recursion"
		"mov_eax, %0"
		"pop_ebx # _common_recursion"
		"add_eax,ebx"
		"movsx_eax,BYTE_PTR_[eax]"
		"pop_ebx # _common_recursion"
		"cmp"
		"setne_al"
		"movzx_eax,al"
		"test_eax,eax"
		"je %END_WHILE_fputs_0"
		"# THEN_while_fputs_0"
		"push_edi        # Prevent overwriting in recursion"
		"push_ebp        # Protect the old base pointer"
		"mov_edi,esp     # Copy new base pointer"
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax        #_common_recursion"
		"mov_eax, %0"
		"pop_ebx # _common_recursion"
		"add_eax,ebx"
		"movsx_eax,BYTE_PTR_[eax]"
		"push_eax        #_process_expression1"
		"lea_eax,[ebp+DWORD] %-8"
		"mov_eax,[eax]"
		"push_eax        #_process_expression2"
		"mov_ebp,edi"
		"call %FUNCTION_fputc"
		"pop_ebx # _process_expression_locals"
		"pop_ebx # _process_expression_locals"
		"pop_ebp # Restore old base pointer"
		"pop_edi # Prevent overwrite"
		"lea_eax,[ebp+DWORD] %-4"
		"push_eax        #_common_recursion"
		"lea_eax,[ebp+DWORD] %-4"
		"mov_eax,[eax]"
		"push_eax        #_common_recursion"
		"mov_eax, %1"
		"pop_ebx # _common_recursion"
		"add_eax,ebx"
		"pop_ebx # _common_recursion"
		"mov_[ebx],eax"
		"jmp %WHILE_fputs_0"
		":END_WHILE_fputs_0"
		"ret"
	);
}

int open(int name, int flag, int mode)
{
	asm("lea_ebx,[esp+DWORD] %12"
	    "mov_ebx,[ebx]"
	    "lea_ecx,[esp+DWORD] %8"
	    "mov_ecx,[ecx]"
	    "lea_edx,[esp+DWORD] %4"
	    "mov_edx,[edx]"
	    "mov_eax, %5"
	    "int !0x80");
}

int fopen(int filename, int mode)
{
	int f;
	if(eq('w', ri8(mode)))
	{ /* 577 is O_WRONLY|O_CREAT|O_TRUNC, 384 is 600 in octal */
		f = open(filename, 577 , 384);
	}
	else
	{ /* Everything else is a read */
		f = open(filename, 0, 0);
	}

	/* Negative numbers are error codes */
	if(gt(0, f))
	{
		return 0;
	}
	return f;
}

int close(int fd)
{
	asm("lea_ebx,[esp+DWORD] %4"
	    "mov_ebx,[ebx]"
	    "mov_eax, %6"
	    "int !0x80");
}

int fclose(int stream)
{
	int error = close(stream);
	return error;
}

int brk(int addr)
{
	asm("mov_eax,[esp+DWORD] %4"
	    "push_eax"
	    "mov_eax, %45"
	    "pop_ebx"
	    "int !0x80");
}

int _malloc_ptr;
int _brk_ptr;

int malloc(int size)
{
	if(eq(NULL, _brk_ptr))
	{
		_brk_ptr = brk(0);
		_malloc_ptr = _brk_ptr;
	}

	if(lt(_brk_ptr, add(_malloc_ptr, size)))
	{
		_brk_ptr = brk(add(_malloc_ptr, size));
		if(eq(-1, _brk_ptr)) return 0;
	}

	int old_malloc = _malloc_ptr;
	_malloc_ptr = add(_malloc_ptr, size);
	return old_malloc;
}

int strlen(int str )
{
	int i = 0;
	while(neq(0, ri8(add(str, i)))) { i = add(i, 1); }
	return i;
}

int memset(int ptr, int value, int num)
{
	int s;
	for(s = ptr; lt(0, num); num = sub(num, 1))
	{
		wi8(s, value);
		s = add(s, 1);
	}
}

int calloc(int count, int size)
{
	int ret = malloc(mul(count, size));
	if(eq(NULL, ret)) return NULL;
	memset(ret, 0, mul(count, size));
	return ret;
}

int free(int l)
{
	return;
}

void exit(int value)
{
	asm("pop_ebx"
	    "pop_ebx"
	    "mov_eax, %1"
	    "int !0x80");
}

void require(int bool, int error)
{
	if(eq(bool, 0))
	{
		fputs(error, stderr);
		exit(EXIT_FAILURE);
	}
}


int match(int a, int b)
{
	int c;
	if(and(eq(NULL, a), eq(NULL, b))) return TRUE;
	if(eq(NULL, a)) return FALSE;
	if(eq(NULL, b)) return FALSE;

	int i = -1;
	while(1)
	{
		if(and(neq(0, ri8(add(a, i))), neq(0, ri8(add(b, i))))){
			break;
		}
		i = add(i, 1);
		if(neq(ri8(add(a, i)), ri8(add(b, i))))
		{
			return FALSE;
		}
	}
	return TRUE;
}


int in_set(int c, int s)
{
	/* NULL set is always false */
	if(eq(NULL, s)) return FALSE;

	while(neq(0, ri8(s)))
	{
		if(eq(c, ri8(s))) return TRUE;
		s = add(s, 1);
	}
	return FALSE;
}

/* INTERNAL ONLY */
int __index_number(int s, int c)
{
	int i = 0;
	c = and(c, 255);
	while(neq(ri8(add(s, i)), c))
	{
		i = add(i, 1);
		if(eq(0, ri8(add(s, i)))) return -1;
	}
	return i;
}

/* INTERNAL ONLY */
int __toupper(int c)
{
	if(in_set(c, "abcdefghijklmnopqrstuvwxyz")) { return and(c, 0xDF); }
	return c;
}

/* INTERNAL ONLY */
int __set_reader(int set, int mult, int input)
{
	int n = 0;
	int i = 0;
	int hold;
	int negative_p = FALSE;

	if(eq(ri8(input), '-'))
	{
		negative_p = TRUE;
		i = add(i, 1);
	}

	while(in_set(ri8(add(input, i)), set))
	{
		n = mul(n, mult);
		hold = __index_number(set, __toupper(ri8(add(input, i))));

		/* Input managed to change between in_set and index_number */
		if(eq(-1, hold)) return 0;
		n = add(n, hold);
		i = add(i, 1);
	}

	/* loop exited before NULL and thus invalid input */
	if(neq(0, ri8(add(input, i)))) return 0;

	if(negative_p)
	{
		n = sub(0, n);
	}

	return n;
}

int strtoint(int a)
{
	int result = 0;
	/* If NULL string */
	if(eq(0, ri8(a)))
	{
		result = 0;
	}
	/* Deal with binary */
	else if (and(eq('0', ri8(a)), eq('b', ri8(add(a, 1)))))
	{
		result = __set_reader("01", 2, add(a, 2));
	}
	/* Deal with hex */
	else if (and(eq('0', ri8(a)), eq('x', ri8(add(a, 1)))))
	{
		result = __set_reader("0123456789ABCDEFabcdef", 16, add(a, 2));
	}
	/* Deal with octal */
	else if(eq('0', ri8(a)))
	{
		result = __set_reader("01234567", 8, add(a, 1));
	}
	/* Deal with decimal */
	else
	{
		result = __set_reader("0123456789", 10, a);
	}

	/* Deal with sign extension for 64bit hosts */
	if(neq(0, and(0x80000000, result))) {
		result = or(shl(0xFFFFFFFF, 31), result);
	}
	return result;
}

int int2str(int x, int base, int signed_p)
{
	int table;
	int cond;
	require(lt(1, base), "int2str doesn't support a base less than 2\n");
	require(gt(37, base), "int2str doesn't support a base more than 36\n");
	/* Be overly conservative and save space for 32binary digits and padding null */
	int p = calloc(34, 1);
	/* if calloc fails return null to let calling code deal with it */
	if(eq(NULL, p)) return p;

	p = add(p, 32);
	int i;
	int sign_p = FALSE;
	table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if(and(and(signed_p, eq(10, base)), neq(0, and(x, 0x80000000))))
	{
		/* Truncate to 31bits */
		i = and(-x, 0x7FFFFFFF);
		if(eq(0, i)) return "-2147483648";
		sign_p = TRUE;
	} /* Truncate to 32bits */
	else i = and(x, or(0x7FFFFFFF, shl(1, 31)));

	while(1)
	{
		if(lt(0, i)){
			break;
		}
		wi8(p, ri8(add(table, mod(i, base))));
		p = sub(p, 1);
		i = div(i, base);
	}

	if(sign_p)
	{
		wi8(p, '-');
		p = sub(p, 1);
	}

	return add(p, 1);
}


int expr(void);
int decl(int l);

int puts(int a){
  fputs(a, stdout);
  fputs("\n", stdout);
}

int v_esp; int v_ebp; int v_stack_size; int v_stack;

int init_c(void){
  puts("init_c called");
  v_stack_size = mul(64, 1024);
  v_stack=calloc(1,v_stack_size);
  v_esp = sub(add(v_stack, v_stack_size), 4);
  v_ebp=v_esp;
  int_size = 1;
}

int mk_char(int c){
  return c;
}

int mk_c_string(int s){
  return s;
}

int dummy(void){
  puts("dummy called");
}

int v_alloca(int x) {
  v_esp = sub(v_esp, x);
  return v_esp;
}

int enter(void) {
/* FIXME detect overflow */
  v_esp = sub(v_esp, 4);
  wi32(v_esp,v_ebp);
  v_ebp=v_esp;
}

int leave(int x) {
  v_esp=v_ebp;
  v_ebp=ri32(v_esp);
  v_esp = add(v_esp, 4);
  return x;
}

int isspace(int c){
  if(or(or(eq(c, mk_char(' ')), eq(c, mk_char('\n'))), eq(c, mk_char('\t')))){
    return 1;
  } else {
    return 0;
  }
}

int isdigit(int c){
  int r;
  c = and(c, 0xFF);
/*  print("isdigit:"+c); */
  r = sub(c, mk_char('0'));
  return and(lt(r, 10), gte(r, 0));
}

int isalnum(int c){
  int r; int t;
  c = and(c, 0xFF);
  t = sub(or(c, 32), mk_char('a'));
  r = or(and(lt(t, 26), gte(t, 0)), isdigit(c));
/*  print("isalnum:"+c+" "+r+ " "+String.fromCharCode(c)); */
  return r;

}

int strtol(int a, int b, int c){
  puts("strtol:");
  puts(a);
  return strtoint(a);
}

int strncmp (int a,int  b, int size) {
  if (eq(size, 0))
    return 0;

  while (and(and(neq(ri8(a), 0), neq(ri8(b), 0)),
             and(eq(ri8(a), ri8(b)), gt(size, 1))))
    {
      size = sub(size, 1);
      a = add(a, 1);
      b = add(b, 1);
    }

  return sub(ri8(a), ri8(b));
}

int strstr(int haystack, int needle){
/*
  puts("strstr");
  puts("haystack: "+haystack);
  puts("needle:"+needle);
*/
  int lh; int ln; int o; int r;
  o=0;
  if(or(eq(haystack, 0), eq(needle, 0))){
    return 0;
  }
  if(eq(ri8(needle), 0)){
    return haystack;
  }
  if(eq(ri8(haystack), 0)){
    return 0;
  }
  lh=strlen(haystack);
  ln=strlen(needle);
  if(gt(ln, lh)) {
    return 0;
  }
  while(lt(o, lh)) {
/*    puts("o: "+o); */
    r = strncmp(needle, add(haystack, o), ln);
/*    puts("r: "+r); */
    if(eq(r, 0)) {
      return add(haystack, o);
    }
    o = add(o, 1);
  }
  return 0;
}

int memcpy(int dest, int src, int c) {
  int i;
  for(i=0;lt(i, c);i = add(i, 1)){
    wi8(add(dest, i),ri8(add(src, i)));
  }
}

int strcpy(int dest, int src){
  int c; int p;
  p = dest;
  c = 0;
  while(1){
    c=ri8(src);
    if(eq(c, 0)){
      break;
    }
    src = add(src, 1);
    wi8(dest,c);
    dest = add(dest, 1);
  }
  return p;
}


int fwrite(int ptr,int size, int nitems, int stream) {
  int c;
  int t = mul(size, nitems);
  c = ptr;
  while(gt(t, 0)){
    fputc(ri8(c),stream);
    t = sub(t, 1);
    c = add(c, 1);
  }
}

int printf(int a){
  puts("printf not impl");
  exit(1);
}

void puts_num(int x){
  int s;
  s=int2str(x,10,0);
  puts(s);
  free(s);
  return;
}
int tok; int tokc; int tokl; int ch; int vars; int rsym; int prog; int ind;
int loc; int glo; int file; int sym_stk; int dstk; int dptr; int dch;
int last_id; int data; int text; int data_offset;

int ALLOC_SIZE;

int ELF_BASE;
int PHDR_OFFSET; int INTERP_OFFSET; int INTERP_SIZE; int DYNAMIC_OFFSET;
int DYNAMIC_SIZE;
int ELFSTART_SIZE; int STARTUP_SIZE; int DYNSTR_BASE;

int TOK_STR_SIZE;
int TOK_IDENT; int TOK_INT; int TOK_IF; int TOK_ELSE; int TOK_WHILE;
int TOK_BREAK; int TOK_RETURN;
int TOK_FOR; int TOK_DEFINE; int TOK_MAIN; int TOK_DUMMY; int TOK_NUM;
int LOCAL; int SYM_FORWARD;
int SYM_DEFINE; int TAG_TOK; int TAG_MACRO;

int pdef(int t){
  wi8(dstk, t);
  dstk = add(dstk, 1);
}

int inp (void){
  if(neq(dptr, 0)){
    ch = ri8(dptr);
    dptr = add(dptr, 1);
    if(eq(ch, TAG_MACRO)){
      dptr=0;
      ch=dch;
    }
  } else {
    ch=fgetc(file);
  }
}

int isid (void){
  return or(isalnum(ch), eq(ch, mk_char('_')));
}

int getq (void){
  if(eq(ch, mk_char('\\'))){
    inp ();
    if(eq(ch, mk_char('n'))) {
      ch = mk_char('\n');
    }
  }
}

int next(void){
  int t; int l; int a;
  while(or(neq(isspace(ch), 0), eq(ch, mk_char('#')))){
    if(eq(ch, mk_char('#'))){
      inp ();
      next();
      if(eq(tok, TOK_DEFINE)){
        next();
        pdef(TAG_TOK);
        wi32(tok, 1);
        wi32(add(tok, 4), dstk);
      }
      while(neq(ch, mk_char('\n'))){
        pdef(ch);
        inp ();
      }
      pdef(ch);
      pdef(TAG_MACRO);
    }
    inp ();
  }
  tokl=0;
  tok=ch;
  if(neq(isid (), 0)){
    pdef(TAG_TOK);
    last_id=dstk;
    while( isid ()){
      pdef(ch);
      inp ();
    }
    if(neq(isdigit(tok), 0)){
      tokc = strtol(last_id,0,0);
      tok = TOK_NUM;
    } else{
      wi8(dstk, TAG_TOK);
      tok = sub(strstr(sym_stk, sub(last_id, 1)), sym_stk);
      wi8(dstk, 0);
      tok = add((mul(tok, 8)), TOK_IDENT);
      if(gt(tok, TOK_DEFINE)){
        tok = add(vars, tok);
        if(eq(ri32(tok), SYM_DEFINE)){
          dptr = ri32(add(tok, 4));
          dch=ch;
          inp ();
          next();
        }
      }
    }
  } else{
    inp ();
    if(eq(tok, mk_char('\''))){
      tok = TOK_NUM;
      getq ();
      tokc=ch;
      inp ();
      inp ();
    } else if(and(eq(tok, mk_char('/')), eq(ch, mk_char('*')))){
      inp ();
      while(neq(ch, 0)){
        while(neq(ch, mk_char('*'))) {
          inp ();
        }
        inp ();
        if(eq(ch, mk_char('/'))) {
          ch=0;
        }
      }
      inp ();
      next();
    } else{
      t=mk_c_string("++#m--%am*@R<^1c/@%[_[H3c%@%[_[H3c+@.B#d-@%:_^BKd<<Z/03e>>`/03e<=0f>=/f<@.f>@1f==&g!='g&&k||#l&@.BCh^@.BSi|@.B+j~@/%Yd!@&d*@b");
      while( l = ri8(t)){
        t = add(t, 1);
        a = ri8(t);
        t = add(t, 1);
        tokc=0;
        while(lt(tokl = sub(ri8(t), mk_char('b')), 0)) {
          t = add(t, 1);
          tokc = add(add((mul(tokc, 64)), tokl), 64);
        }
        t = add(t, 1);
        if(and(eq(l, tok), (or(eq(a, ch), eq(a, mk_char('@')))))){
          if(eq(a, ch)){
            inp ();
            tok = TOK_DUMMY;
          }
          break;
        }
      }
      t = add(t, 1);
    }
  }
}

int o(int n){
  while(and(neq(n, 0), neq(n, (-1)))){
    wi8(ind, n);
    ind = add(ind, 1);
    n = shr(n, 8);
  }
}

int put32(int t, int n){
  wi8(t, n);     t = add(t, 1);
  wi8(t, shr(n, 8)); t = add(t, 1);
  wi8(t, shr(n, 16)); t = add(t, 1);
  wi8(t, shr(n, 24)); t = add(t, 1);
}

int get32(int t){
  int n;
  return or(or(and(ri8(t), 255),
		shl(and(ri8(add(t, 1)), 255), 8)),
		or(shl(and(ri8(add(t, 2)), 255), 16),
		shl(and(ri8(add(t, 3)), 255), 24)));
}

int gsym1(int t, int b){
  int d;
  while(neq(t, 0)){
    d=get32(t);
    if(eq(ri8(sub(t, 1)), 5)){
      if(and(gte(b, data), (lt(b, glo))))
        put32(t, add(b, data_offset));
      else
        put32(t, add(add(sub(b, prog), text), data_offset));
    }
    else{
      put32(t, sub(sub(b, t), 4));
    }
    t = d;
  }
}

int gsym(int t){
  gsym1(t, ind);
}

int oad(int n, int t){
  o( n);
  put32(ind, t);
  t = ind;
  ind = add(ind, 4);
  return t;
}

int li(int t){
  oad(184, t);
}

int gjmp(int t){
  return oad(233, t);
}

int gtst(int l, int t){
  o( 1032325);
  return oad(add(132, l), t);
}

int gcmp(int t){
  o( 49465);
  li(0);
  o( 15);
  o( add(t, 144));
  o( 192);
}

int gmov(int l, int t){
  int d;
  o( add(l, 131));
  d = ri32(t);
  if(and(neq(d, 0), (lt(d, LOCAL)))) {
    oad(133,d);
  } else {
    t = add(t, 4);
    wi32(t, oad(5,ri32(t)));
  }
}

int unary(int l){
  int n; int t; int a; int c;
  n=1;
  if(eq(tok, mk_char('\"'))){
    li(add(glo, data_offset));
    while(neq(ch, mk_char('\"'))){
      getq ();
      wi8(glo, ch);
      glo = add(glo, 1);
      inp ();
    }
    wi8(glo, 0);
    glo=and(add(glo, 4), -4);
    inp ();
    next();
  } else {
    c=tokl;
    a=tokc;
    t=tok;
    next();
    if(eq(t, TOK_NUM)){
      li(a);
    }
    else if(eq(c, 2)){
      unary(0);
      oad(185,0);
      if(eq(t, mk_char('!'))) {
        gcmp(a);
      } else {
        o( a);
      }
    }
    else if(eq(t, mk_char('('))){
      expr();
      next();
    }
    else if(eq(t, mk_char('*'))){
      next();
      t=tok;
      next();
      next();
      if(eq(tok, mk_char('*'))){
        next();
        next();
        next();
        next();
        t=0;
      }
      next();
      unary(0);
      if(eq(tok, mk_char('='))){
        next();
        o( 80);
        expr();
        o( 89);
        o( add(392, eq(t, TOK_INT)));
      } else if( t){
        if(eq(t, TOK_INT)) {
          o( 139);
        } else {
          o( 48655);
        }
        ind = add(ind, 1);
      }
    } else if(eq(t, mk_char('&'))){
      gmov(10,tok);
      next();
    }
    else{
      n=0;
      if(and(eq(tok, mk_char('=')), neq(l, 0))){
        next();
        expr ();
        gmov(6,t);
      } else if(neq(tok, mk_char('('))){
        gmov(8,t);
        if(eq(tokl, 11)){
          gmov(0,t);
          o( tokc);
          next();
        }
      }
    }
  }
  if(eq(tok, mk_char('('))){
    if( n)o( 80);
    a=oad(60545,0);
    next();
    l=0;
    while(neq(tok, mk_char(')'))){
      expr();
      oad(2393225,l);
      if(eq(tok, mk_char(','))) {
        next();
      }
      l = add(l, 4);
    }
    put32(a,l);
    next();
    if( n){
      oad(2397439,l);
      l = add(l, 4);
    }
    else{
      t = add(t, 4);
      wi32(t, oad(232,ri32(t)));
    }
    if( l) oad(50305,l);
  }
}

int sum(int l){
  int t; int n; int a;
  if(eq(l, 1)) {
    unary(1);
  } else{
    l = sub(l, 1);
    sum(l);
    a=0;
    while(eq(l, tokl)){
      n=tok;
      t=tokc;
      next();
      if(gt(l, 8)){
        a=gtst(t,a);
        sum(l);
      }
      else{
        o( 80);
        sum(l);
        o( 89);
        if(or(eq(l, 4), eq(l, 5))){
          gcmp(t);
        }
        else{
          o( t);
          if(eq(n, mk_char('%'))) {
            o( 146);
          }
        }
      }
    }
    if(and(neq(a, 0), gt(l, 8))){
      a=gtst(t,a);
      li(xor(t, 1));
      gjmp(5);
      gsym(a);
      li(t);
    }
  }
}

int expr(void){
  sum(11);
}

int test_expr(void){
  expr();
  return gtst(0,0);
}

int block(int l){
  int a ; int n; int t;
  enter();
  a = v_alloca(4);
  if(eq(tok, TOK_IF)){
    next();
    next();
    wi32(a, test_expr());
    next();
    block(l);
    if(eq(tok, TOK_ELSE)){
      next();
      n=gjmp(0);
      gsym(ri32(a));
      block(l);
      gsym(n);
    }
    else{
      gsym(ri32(a));
    }
  }
  else if(or(eq(tok, TOK_WHILE), eq(tok, TOK_FOR))){
    t=tok;
    next();
    next();
    if(eq(t, TOK_WHILE)){
      n=ind;
      wi32(a, test_expr());
    }
    else{
      if(neq(tok, mk_char(';'))) {
        expr ();
      }
      next();
      n=ind;
      wi32(a, 0);
      if(neq(tok, mk_char(';'))) {
        wi32(a, test_expr());
      }
      next();
      if(neq(tok, mk_char(')'))){
        t=gjmp(0);
        expr ();
        gjmp(sub(sub(n, ind), 5));
        gsym(t);
        n = add(t, 4);
      }
    }
    next();
    block(a);
    gjmp(sub(sub(n, ind), 5));
    gsym(ri32(a));
  }
  else if(eq(tok, mk_char('{'))){
    next();
    decl(1);
    while(neq(tok, mk_char('}'))) {
      block(l);
    }
    next();
  }
  else{
    if(eq(tok, TOK_RETURN)){
      next();
      if(neq(tok, mk_char(';'))) {
        expr ();
      }
      rsym=gjmp(rsym);
    } else if(eq(tok, TOK_BREAK)){
      next();
      wi32(l, gjmp(ri32(l)));
    } else if(neq(tok, mk_char(';'))) {
      expr ();
    }
    next();
  }
  leave(0);
}

int decl(int l){
  int a;
  while(or(eq(tok, TOK_INT), (and(neq(tok, (-1)), eq(l, 0))))){
    if(eq(tok, TOK_INT)){
      next();
      while(neq(tok, mk_char(';'))){
        if( l){
          loc = add(loc, 4);
          wi32(tok, -loc);
        }
        else{
          wi32(tok, glo);
          glo = add(glo, 4);
        }
        next();
        if(eq(tok, mk_char(','))) {
          next();
        }
      }
      next();
    }
    else{
      wi32(tok, ind);
      next();
      next();
      a=8;
      while(neq(tok, mk_char(')'))){
        wi32(tok, a);
        a = add(a, 4);
        next();
        if(eq(tok, mk_char(','))) {
          next();
        }
      }
      next();
      rsym=loc=0;
      o( 15042901);
      a=oad(60545,0);
      block(0);
      gsym(rsym);
      o( 50121);
      put32(a,loc);
    }
  }
}

int gle32(int n){
  put32(glo,n);
  glo = add(glo, 4);
}

int gphdr1(int n, int t){
  gle32( n);
  n = add(n, ELF_BASE);
  gle32( n);
  gle32( n);
  gle32( t);
  gle32( t);
}

int elf_reloc(int l){
  int t; int a; int n; int p; int b; int c;
  p=0;
  t=sym_stk;
  while( 1){
    t = add(t, 1);
    a=t;
    while(and(neq(ri8(t), TAG_TOK), (lt(t, dstk)))) {
      t = add(t, 1);
    }
    if(eq(t, dstk)) { break; }
    tok = sub(add(add(vars, (mul(sub(a, sym_stk), 8))), TOK_IDENT), 8);
    b = ri32(tok);
    n = ri32(add(tok, 4));
    if( and(neq(n, 0), neq(b, 1))){
      if(eq(b, 0)){
        if(eq(l, 0)){
          memcpy(glo,a,sub(t, a));
          glo = add(sub(add(glo, t), a), 1);
        } else if(eq(l, 1)){
          gle32( add(p, DYNSTR_BASE));
          gle32( 0);
          gle32( 0);
          gle32( 16);
          p = add(sub(add(p, t), a), 1);
        } else{
          p = add(p, 1);
          while( n){
            a=get32(n);
            c = neq(ri8(sub(n, 1)), 5);
            put32(n, mul((-c), 4));
            gle32( add(sub(n, prog), add(text,data_offset)));
            gle32( add(mul(p, 256),add(c, 1)));
            n=a;
          }
        }
      } else if(eq(l, 0)){
        gsym1(n,b);
      }
    }
  }
}

int elf_out(int c){
  int glo_saved; int dynstr; int dynstr_size; int dynsym; int hash; int rel;
  int n; int t; int text_size;
  text=glo;
  text_size = sub(ind, prog);
  ind=prog;
  o( 5264472);
  t = ri32(add(vars, TOK_MAIN));
  oad(232,sub(sub(t, ind), 5));
  o( 50057);
  li(1);
  o( 32973);
  glo = add(glo, text_size);
  dynstr=glo;
  glo = add(glo, 1);
  glo = add(strcpy(glo,mk_c_string("libc.so.6")), 10);
  glo = add(strcpy(glo,mk_c_string("libdl.so.2")), 11);
  elf_reloc(0);
  dynstr_size = sub(glo, dynstr);
  glo = and(add(glo, 3), -4);
  dynsym=glo;
  gle32( 0);
  gle32( 0);
  gle32( 0);
  gle32( 0);
  elf_reloc(1);
  hash=glo;
  n = div(sub(glo, dynsym), 16);
  gle32( 1);
  gle32( n);
  gle32( 1);
  gle32( 0);
  t=2;
  while( lt(t, n)) {
    gle32( t);
    t = add(t, 1);
  }
  gle32( 0);
  rel=glo;
  elf_reloc(2);
  memcpy(text,prog,text_size);
  glo_saved=glo;
  glo=data;
  gle32( 1179403647);
  gle32( 65793);
  gle32( 0);
  gle32( 0);
  gle32( 196610);
  gle32( 1);
  gle32( add(text, data_offset));
  gle32( PHDR_OFFSET);
  gle32( 0);
  gle32( 0);
  gle32( 2097204);
  gle32( 3);
  gle32( 3);
  gphdr1(INTERP_OFFSET, INTERP_SIZE);
  gle32( 4);
  gle32( 1);
  gle32( 1);
  gphdr1(0, sub(glo_saved, data));
  gle32( 7);
  gle32( 4096);
  gle32( 2);
  gphdr1(DYNAMIC_OFFSET, DYNAMIC_SIZE);
  gle32( 6);
  gle32( 4);
  glo = add(strcpy(glo,mk_c_string("/lib/ld-linux.so.2")), 20);
  gle32( 1);
  gle32( 1);
  gle32( 1);
  gle32( 11);
  gle32( 4);
  gle32( add(hash, data_offset));
  gle32( 6);
  gle32( add(dynsym, data_offset));
  gle32( 5);
  gle32( add(dynstr, data_offset));
  gle32( 10);
  gle32( dynstr_size);
  gle32( 11);
  gle32( 16);
  gle32( 17);
  gle32( add(rel, data_offset));
  gle32( 18);
  gle32( sub(glo_saved, rel));
  gle32( 19);
  gle32( 8);
  gle32( 0);
  gle32( 0);
  t=fopen(c,mk_c_string("wb"));
  fwrite(data,1,sub(glo_saved, data),t);
  fclose(t);
}

int init_globals(void){
  ALLOC_SIZE = 99999;

  ELF_BASE = 0x08048000;
  PHDR_OFFSET = 0x30;

  INTERP_OFFSET = 0x90;
  INTERP_SIZE =  0x13;

  DYNAMIC_OFFSET = add(INTERP_OFFSET, add(INTERP_SIZE, 1));
  DYNAMIC_SIZE =  mul(11, 8);

  ELFSTART_SIZE = add(DYNAMIC_OFFSET, DYNAMIC_SIZE);
  STARTUP_SIZE =  17;

  DYNSTR_BASE  =  22;

  TOK_STR_SIZE = 48;
  TOK_IDENT  =  0x100;
  TOK_INT    =  0x100;
  TOK_IF     =  0x120;
  TOK_ELSE   =  0x138;
  TOK_WHILE  =  0x160;
  TOK_BREAK  =  0x190;
  TOK_RETURN =  0x1C0;
  TOK_FOR    =  0x1F8;
  TOK_DEFINE =  0x218;
  TOK_MAIN   =  0x250;

  TOK_DUMMY  = 1;
  TOK_NUM    = 2;

  LOCAL = 0x200;

  SYM_FORWARD = 0;
  SYM_DEFINE  = 1;

  TAG_TOK   =  mk_char(' ');
  TAG_MACRO =  2;

  TRUE = 1;
  FALSE = 0;
  stdin = 0;
  stdout = 1;
  stderr = 2;
  EOF = -1;
  NULL = 0;
  EXIT_FAILURE = 1;
  EXIT_SUCCESS = 0;
}


int main(int n,int t){
  puts("otccelf start");
  init_c();
  init_globals();
  if( lt(n, 3)){
    printf("usage: otccelf file.c outfile\n");
    return 0;
  }
  dstk=add(strcpy(sym_stk = calloc(1, ALLOC_SIZE),
       mk_c_string(" int if else while break return for define main ")), TOK_STR_SIZE);
  glo = data = calloc(1, ALLOC_SIZE);
  ind = prog = calloc(1, ALLOC_SIZE);
  vars = calloc(1, ALLOC_SIZE);

  fputs("ind: ",stdout); puts_num(ind);

  t = add(t, 4);
  file=fopen(ri32(t), mk_c_string("r"));

  data_offset = sub(ELF_BASE, data);
  glo = add(glo, ELFSTART_SIZE);
  ind = add(ind, STARTUP_SIZE);

  inp();
  next();
  decl(0);
  t = add(t, 4);
  elf_out(ri32(t));
  puts("otccelf complete");


  return 0;
}
