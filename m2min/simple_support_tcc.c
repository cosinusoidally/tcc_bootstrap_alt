/* primitives */

int add(int a, int b) {
	return a + b;
}

int and(int a, int b) {
	return a & b;
}

int eq(int a, int b) {
	return a == b;
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

int sub(int a, int b) {
	return a - b;
}

/* memory functions */

int ri8(int o) {
	char *h = 0;
	return h[o] & 0xFF;
}

int ri32(int o) {
	return or(or(and(ri8(o), 255),
		shl(and(ri8(add(o, 1)), 255), 8)),
		or(shl(and(ri8(add(o, 2)), 255), 16),
		shl(and(ri8(add(o, 3)), 255), 24)));
}

int wi8(int o,int v) {
	char *h = 0;
	h[o]=v;
	return;
}
