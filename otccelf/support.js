puts=print;

function printf(s) {
  print("printf: "+s);
}

function init_c(){
/* dummy */
}

function mk_char(c){
  return c.charCodeAt(0);
}

function mk_argc_argv(s){
  return {argc:0, argv:0};
}
