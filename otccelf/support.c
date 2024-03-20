wi8(o, v){
  *(char*)o = (v & 255);
}

ri8(o){
 return *(char*)o;
}

int v_esp, v_ebp, v_stack_size, v_stack;

init_c(){
  v_stack_size=64*1024;
  v_stack=calloc(1,v_stack_size);
  v_esp=v_stack+v_stack_size-4;
  v_ebp=v_esp;
}

mk_char(c){
  return c;
}

mk_c_string(s){
  return s;
}

#define function 
#define var int
