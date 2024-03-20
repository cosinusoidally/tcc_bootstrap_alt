wi8(o, v){
  *(char*)o = (v & 255);
}

ri8(o){
 return *(char*)o;
}

int v_esp, v_ebp;

init_c(){
/* dummy */
  v_esp=123;
  v_ebp=345;
}

#define function 
#define var int
