var acc;
var a1;
var ip;

var next_block_;

/* these are blocks */
function test(){
  acc = 123;
  a1 = 1000;
  add();
  ip = next_block_; /* this would call the next block */
}

function next_block(){
  acc = 0;
  end();
}
