var acc;

var a0;
var a1;

var cond;

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

var c;
var d;

function my_loop(){
  c=1;
  while(c) {
    a0=c; a1=10; gt();
/* this is nonesense, but just testing if else codegen */
    if(acc){
      c=0;
      d=20;
    } else {
      a0=2;a1=20; gt();
      if(acc){
        c=10;
        d=20;
      } else {
        d=40;
      }
    }
    a0=1; a1=c; add(); c=acc;
  }
}

function f3(){
  acc='a';
}

function bar(){
  acc="Hello world";
  acc="Hello\n";
}

function foo(){
  add();
}

function main(){
  a0=20;a1=21;foo();
}
