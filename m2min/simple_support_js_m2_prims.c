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

int fputc(int s, int f)
{
	asm("mov_eax, %4"
	    "lea_ebx,[esp+DWORD] %4"
	    "mov_ebx,[ebx]"
	    "lea_ecx,[esp+DWORD] %8"
	    "mov_edx, %1"
	    "int !0x80");
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

int close(int fd)
{
	asm("lea_ebx,[esp+DWORD] %4"
	    "mov_ebx,[ebx]"
	    "mov_eax, %6"
	    "int !0x80");
}

int brk(int addr)
{
	asm("mov_eax,[esp+DWORD] %4"
	    "push_eax"
	    "mov_eax, %45"
	    "pop_ebx"
	    "int !0x80");
}

int exit(int value)
{
	asm("pop_ebx"
	    "pop_ebx"
	    "mov_eax, %1"
	    "int !0x80");
}
