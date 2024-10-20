int acc;
int a1;
int ip;

int next_block_;

/* these are blocks */
function test(){
  acc = 1;
  a1 = 2;
  add();
  ip = next_block_; /* this would call the next block */
}

function next_block(){
  acc = 0;
  end();
}
