/* primitives */

int add(int a, int b) {
	return a + b;
}

int and(int a, int b) {
	return a & b;
}

/* need to use underscore I think, something to do with symbols */
int _div(int a, int b) {
	return a / b;
}

int eq(int a, int b) {
	return a == b;
}

int gt(int a, int b) {
	return a > b;
}

int gte(int a, int b) {
	return a >= b;
}

int lt(int a, int b) {
	return a < b;
}

int lte(int a, int b) {
	return a <= b;
}

int mod(int a, int b) {
	return a % b;
}

int mul(int a, int b) {
	return a * b;
}

int neq(int a, int b) {
	return a != b;
}

int or(int a, int b) {
	return a | b;
}

int shl(int a, int b) {
	return a << b;
}

int shr(int a, int b) {
	return a >> b;
}

int sub(int a, int b) {
	return a - b;
}

/* memory functions */

int ri8(int o) {
	char *h = 0;
	return h[o] & 0xFF;
}

int wi8(int o,int v) {
	char *h = 0;
	h[o]=v;
	return;
}

int fopen(char *pathname, char *mode);

int v_fopen(pathname, mode) {
  int r;
  printf("v_fopen pathname: %s mode: %s\n", pathname, mode);
  r = fopen(pathname, mode);
  return r;
}

#include <sys/mman.h>

char *heap;
int heap_size = 16 * 1024 * 1024;

int _brk_ptr;
int _malloc_ptr;

int v_malloc(size) {
        int old_malloc;

        if(lt(_brk_ptr, add(_malloc_ptr, size))) {
                _brk_ptr = add(_malloc_ptr, size);
                if(eq(sub(0,1), _brk_ptr)) return 0;
        }

        old_malloc = _malloc_ptr;
        _malloc_ptr = add(_malloc_ptr, size);
        return old_malloc;
}

int v_memset(ptr, value, num) {
        int s;
        s = ptr;
        while(lt(0, num))
        {
                wi8(s, value);
                s = add(s, 1);
                num = sub(num, 1);
        }
}

int v_calloc(count, size) {
        int ret;
        ret = v_malloc(mul(count, size));
        if(eq(0, ret)) {
                return 0;
        }
        v_memset(ret, 0, mul(count, size));
        return ret;
}

int v_free(l) {
        return 0;
}

int mks(char *s){
  int i;
  int r;
  int len;
  len = strlen(s);
  r = v_malloc(add(len, 1));
  for(i =0; lt(i, len); i = add(i, 1)){
    wi8(add(r, i), s[i]);
  }
  wi8(add(r, i), 0);
  return r;
}

int v_init_support() {
  /* dummy */
}

int init_support() {
  puts("init_support called");
  heap = mmap(
        0,
        heap_size,
        PROT_READ | PROT_WRITE, // permissions
        MAP_32BIT | MAP_PRIVATE | MAP_ANONYMOUS, // flags
        -1, // file descriptor
        0 // offset
  );
  if (heap == MAP_FAILED) {
      perror("mmap");
  } else {
      printf("%p\n", heap);
  }
  if(heap >= (void *)0xFFFFFFFF) {
      puts("error heap above 32 bit address");
      exit(1);
  }
  _brk_ptr = heap + 128*1024;
  _malloc_ptr = _brk_ptr;
}

int mkc(c) {
  return c;
}

int main(int argc, char **argv) {
  int r;
  int i;
  int v_argv;
  int v_arg;

  init_support();

  /* virtual arguments */
  v_argv = v_calloc(mul(4,argc),1);
  v_arg = v_argv;
  for(i = 0 ; i < argc ; i = i + 1) {
    wi32(v_arg, mks(argv[i]));
    v_arg = v_arg + 4;
  }
  v_arg = v_argv;
  for(i = 0 ; i < argc ; i = i + 1) {
    printf("arg: %s\n",ri32(v_arg));
    v_arg = v_arg + 4;
  }
  printf("v_argv: %x\n",v_argv);
  printf("main function called\n");
  r = v_main(argc, v_argv);
  return r;
}
