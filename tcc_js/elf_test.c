int *a="Hello world";

int foo(void){
  puts("foo called");
  return 1;
}

int main(void) {
  puts("elf test");
  puts(a);
  return 0;
}
