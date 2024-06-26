function wi32(o, v) {
  wi8(o,v&0xFF);
  v=v>>8;
  wi8(o+1,v&0xFF);
  v=v>>8;
  wi8(o+2,v&0xFF);
  v=v>>8;
  wi8(o+3,v&0xFF);
}

function ri32(o) {
  return (ri8(o)&255)       | (ri8(o+1)&255)<<8 |
         (ri8(o+2)&255)<<16 | (ri8(o+3)&255)<<24;
}

function v_alloca(x) {
  v_esp=v_esp-x;
  return v_esp;
}

function enter() {
/* FIXME detect overflow */
  printf("enter esp: 0x%x ebp: 0x%x\n", v_esp, v_ebp);
  v_esp=v_esp-4;
  wi32(v_esp,v_ebp);
  v_ebp=v_esp;
}

function leave(x) {
  v_esp=v_ebp;
  v_ebp=ri32(v_esp);
  v_esp=v_esp+4;
  printf("leave esp: 0x%x ebp: 0x%x\n", v_esp, v_ebp);
  return x;
}
