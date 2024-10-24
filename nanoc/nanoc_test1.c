int acc;
int a1;
int ip;

int next_block_;

/* these are blocks */
int test(){
  acc = 123;
  a1 = 1000;
  add();
  ip = next_block_; /* this would call the next block */
}

int next_block(){
  acc = 0;
  end();
}
