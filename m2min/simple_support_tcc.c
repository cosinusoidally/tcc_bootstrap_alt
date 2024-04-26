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
