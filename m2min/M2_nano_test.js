var acc;

var a0;
var a1;

function add(){
/* this is not currently correct, just here to test out asm */
/* what it should do is acc = a0 + a1; */
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

function f3(){
  acc='a';
}

function bar(){
  acc="Hello world";
}

function foo(){
  add();
}

function main(){
  a0=20;a1=21;foo();
}
