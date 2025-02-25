/* primitives */

int add(int a, int b) {
	return a + b;
}

int and(int a, int b) {
	return a & b;
}

int div(int a, int b) {
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

#include <sys/mman.h>
#include <stdio.h>

char *heap;
int heap_size = 16 * 1024 * 1024;

int init_support() {
  puts("init_support called");
  heap = mmap(
        NULL,
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
}

int mks(s) {
  return s;
}

int mkc(c) {
  return c;
}

int main(int argc, char **argv) {
  int r;
  printf("main function called\n");
  r = v_main(argc, argv);
  return r;
}
