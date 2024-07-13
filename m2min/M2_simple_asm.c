/* This is a heavily cut down version of M2-Planet
 * modifications (C) 2024 Liam Wilson (under same license)
 * original authors
 * Copyright (C) 2016 Jeremiah Orians
 * Copyright (C) 2018 Jan (janneke) Nieuwenhuizen <janneke@gnu.org>
 * Copyright (C) 2020 deesix <deesix@tuta.io>
 * Copyright (C) 2021 Andrius Štikonas <andrius@stikonas.eu>
 * Copyright (C) 2021 Sanne Wouda
 * Copyright (C) 2022 Jan (janneke) Nieuwenhuizen <janneke@gnu.org>
 * This file is part of M2-Planet.
 *
 * M2-Planet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * M2-Planet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with M2-Planet.  If not, see <http://www.gnu.org/licenses/>.
 */

int FALSE;
int TRUE;
int NULL;

int EXIT_SUCCESS;
int EXIT_FAILURE;

int EOF;

int quote_string;

/* init support code */
int init_support();

/* delare primitives */
int add(int a, int b);
int and(int a, int b);
int div(int a, int b);
int eq(int a, int b);
int gt(int a, int b);
int gte(int a, int b);
int lt(int a, int b);
int lte(int a, int b);
int mod(int a, int b);
int mul(int a, int b);
int neq(int a, int b);
int or(int a, int b);
int shl(int a, int b);
int shr(int a, int b);
int sub(int a, int b);

/* declare base memory operations */
int ri8(int o);
int wi8(int o,int v);

/* libc functions */
int calloc(int count, int size);
int fgetc(int f);
int free(int l);
int exit(int value);
int fputs(int si, int f);
int fopen(int filename, int mode);
int fclose(int stream);

/* misc functions */
int copy_string(int target, int source, int max);
int in_set(int c, int s);
int match(int a, int b);
int reset_hold_string();

/* token_list structure for reference */

/*
struct token_list {
	struct token_list* next;
	union {
		struct token_list* locals;
		struct token_list* prev;
	};
	char* s;
	union {
		struct token_list* arguments;
		int depth;
		int linenumber;
	};
};
*/

int token_list_next_offset;
int token_list_locals_offset;
int token_list_prev_offset;
int token_list_s_offset;
int token_list_arguments_offset;
int token_list_depth_offset;
int token_list_linenumber_offset;

int token_list_layout_init(){
	token_list_next_offset = 0;
	token_list_locals_offset = 4;
	token_list_prev_offset = 4;
	token_list_s_offset = 8;
	token_list_arguments_offset = 12;
	token_list_depth_offset = 12;
	token_list_linenumber_offset = 12;
}

int sizeof_token_list;

/* The core functions */
int read_all_tokens(int a, int current);
int reverse_list(int head);

int program();
int recursive_output(int i, int out);

/* What we are currently working on */
int global_token;

/* Output reorder collections*/
int output_list;
int strings_list;
int globals_list;

/* Make our string collection more efficient */
int hold_string;
int string_index;

/* Our Target Architecture */
int register_size;

int MAX_STRING;

/* Globals */
int input;
int token;
int line;
int file;

/* Global lists */
int global_symbol_list;
int global_function_list;

/* Core lists for this file */
int function;

int break_target_head;
int break_target_func;
int break_target_num;
int break_frame;
int current_count;

int indent;
int no_indent;

int int2str(int x, int base, int signed_p);
int parse_string(int string);
int escape_lookup(int c);

int emit(int s, int head);
int emit_out(int s);

int expression();
int advance();
int statement();

int ri32(int o) {
        return or(or(and(ri8(o), 255),
                shl(and(ri8(add(o, 1)), 255), 8)),
                or(shl(and(ri8(add(o, 2)), 255), 16),
                shl(and(ri8(add(o, 3)), 255), 24)));
}

int wi32(int o, int v) {
  wi8(o, and(v, 0xFF));
  v = shr(v, 8);
  wi8(add(o, 1), and(v, 0xFF));
  v = shr(v, 8);
  wi8(add(o, 2), and(v, 0xFF));
  v = shr(v, 8);
  wi8(add(o, 3), and(v, 0xFF));
}

int set_s(int t,int v) {
	wi32(add(t, token_list_s_offset), v);
}

int get_s(int t) {
	return ri32(add(t, token_list_s_offset));
}

int set_prev(int t,int v) {
	wi32(add(t, token_list_prev_offset), v);
}

int get_prev(int t) {
	return ri32(add(t, token_list_prev_offset));
}

int set_next(int t,int v) {
	wi32(add(t, token_list_next_offset), v);
}

int get_next(int t) {
	return ri32(add(t, token_list_next_offset));
}

int set_linenumber(int t,int v) {
	wi32(add(t, token_list_linenumber_offset), v);
}

int get_linenumber(int t) {
	return ri32(add(t, token_list_linenumber_offset));
}

int set_depth(int t,int v) {
	wi32(add(t, token_list_depth_offset), v);
}

int get_depth(int t) {
	return ri32(add(t, token_list_depth_offset));
}

int set_locals(int t,int v) {
	wi32(add(t, token_list_locals_offset), v);
}

int get_locals(int t) {
	return ri32(add(t, token_list_locals_offset));
}

int set_arguments(int t,int v) {
	wi32(add(t, token_list_arguments_offset), v);
}

int get_arguments(int t) {
	return ri32(add(t, token_list_arguments_offset));
}

int increase_indent() {
	indent = add(indent, 2);
}

int decrease_indent() {
	indent = sub(indent, 2);
	if(lt(indent,0)) {
		indent = 0;
	}
}

int indented_emit_out(int s) {
	int c;
	c = 0;
	if(no_indent) {
		no_indent = 0;
	} else {
		while(lt(c, indent)) {
			emit_out(" ");
			c = add(c, 1);
		}
	}
	emit_out(s);
}

int skip(int str) {
/* dummy impl should check and abort if doesn't match */
	global_token = get_next(global_token);
}

/* advance to next token */
int advance() {
	global_token = get_next(global_token);
}

int token_string(int a) {
	return get_s(a);
}

int global_token_string() {
	return token_string(global_token);
}

int global_token_char0() {
	return ri8(token_string(global_token));
}

int to_hex_digit(int a) {
	a = and(15, a);
	if(gt(a, 9)) {
		a = add(sub(a,10), 65);
	} else {
		a = add(a, 48);
	}
	return a;
}

int to_hex_le(int a) {
	int o;
	int i;
	i = 0;
	o = calloc(17,1);
	while(lt(i, 4)) {
		wi8(add(o, add(mul(i,2), 1)), to_hex_digit(a));
		wi8(add(o, mul(i, 2)), to_hex_digit(shr(a,4)));
		a = shr(a, 8);
		i = add(i, 1);
	}
	return o;
}

int match(int a, int b) {
	int i;
	if(and(eq(NULL, a), eq(NULL, b))) {
		return TRUE;
	}
	if(eq(NULL, a)) {
		return FALSE;
	}
	if(eq(NULL, b)) {
		return FALSE;
	}

	i = sub(0, 1);
	while(1) {
		i = add(i, 1);
		if(neq(ri8(add(a, i)), ri8(add(b, i)))) {
			return FALSE;
		}
		if(eq(0,and(neq(0, ri8(add(a,i))), neq(0, ri8(add(b, i)))))){
			break;
		}
	}
	return TRUE;
}

int in_set(int c, int s) {
	/* NULL set is always false */
	if(eq(NULL, s)) {
		return FALSE;
	}

	while(neq(0, ri8(s))) {
		if(eq(c, ri8(s))) {
			return TRUE;
		}
		s = add(s, 1);
	}
	return FALSE;
}

int int2str(int x, int base, int signed_p) {
	int p;
	int i;
	int sign_p;
	int table;
	/* Be overly conservative and save space for 32binary digits and padding null */
	p = calloc(34, 1);
	/* if calloc fails return null to let calling code deal with it */
	if(eq(NULL, p)) {
		return p;
	}

	p = add(p, 32);
	sign_p = FALSE;
	table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if(and(and(signed_p, eq(10, base)), neq(0, (and(x, 0x80000000))))) {
		/* Truncate to 31bits */
		i = and(sub(0, x), 0x7FFFFFFF);
		if(eq(0, i)) {
			return "-2147483648";
		}
		sign_p = TRUE;
	} else {
		/* Truncate to 32bits */
		i = and(x, or(0x7FFFFFFF, shl(1, 31)));
	}

	while(1) {
		wi8(p, ri8(add(table, mod(i, base))));
		p = sub(p, 1);
		i = div(i, base);
		if(gte(0, i)) {
			break;
		}
	}

	if(sign_p) {
		wi8(p, '-');
		p = sub(p, 1);
	}

	return add(p, 1);
}

int grab_byte() {
	int c;
	c = fgetc(input);
	if(eq(10, c)) {
		line = add(line, 1);
	}
	return c;
}

int clearWhiteSpace(int c) {
	if(or(eq(32, c), eq(9, c))) {
		return clearWhiteSpace(grab_byte());
	}
	return c;
}

int consume_byte(int c) {
	wi8(add(hold_string, string_index), c);
	string_index = add(string_index, 1);
	return grab_byte();
}

int preserve_string(int c) {
	int frequent;
	int escape;

	frequent = c;
	escape = FALSE;
	while(1) {
		if(and(eq(0, escape), eq('\\', c))) {
			escape = TRUE;
		}
		else escape = FALSE;
		c = consume_byte(c);
		if(eq(0,or(escape, (neq(c, frequent))))){
			break;
		}
	}
	return grab_byte();
}

int copy_string(int target, int source, int max) {
	int i;
	i = 0;
	while(neq(0, ri8(add(source, i)))) {
		wi8(add(target, i), ri8(add(source, i)));
		i = add(i, 1);
		if(eq(i, max)) {
			break;
		}
	}
}

int preserve_keyword(int c, int S) {
	while(in_set(c, S)) {
		c = consume_byte(c);
	}
	return c;
}

int reset_hold_string() {
	int i;
	int o;
	i = 0;
	while(lt(i, MAX_STRING)) {
		o = add(hold_string, i);
		if(eq(ri8(o), 0)) {
			break;
		}
		wi8(o, 0);
		i = add(i, 1);
	}
	string_index = 0;
}

int new_token(int s, int size) {
	int current;
	current = calloc(1, sizeof_token_list);

	/* More efficiently allocate memory for string */
	set_s(current, calloc(size, 1));
	copy_string(get_s(current), s, MAX_STRING);

	set_prev(current, token);
	set_next(current, token);
	set_linenumber(current, line);
	token = current;
}

int get_token(int c) {
	int current;
	int reset;
	current = calloc(1, sizeof_token_list);

	reset = 1;

	while(eq(reset, 1)) {
		reset = 0;
		reset_hold_string();
		string_index = 0;

		c = clearWhiteSpace(c);
		if(eq(c, EOF)) {
			free(current);
			return c;
		} else if(in_set(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")) {
			c = preserve_keyword(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
		} else if(in_set(c, "=")) {
			c = preserve_keyword(c, "=");
		} else if(in_set(c, quote_string)) {
			c = preserve_string(c);
		} else if(eq(c, '/')) {
			c = consume_byte(c);
			if(eq(c, '*')) {
				c = grab_byte();
				while(neq(c, '/')) {
					while(neq(c, '*')) {
						c = grab_byte();
					}
					c = grab_byte();
				}
				c = grab_byte();
				reset = 1;
			}
		} else if (eq(c, '\n')) {
			/* eat newlines here */
			c = consume_byte(c);
			reset = 1;
		} else {
			c = consume_byte(c);
		}
	}

	new_token(hold_string, add(string_index, 2));
	return c;
}

int reverse_list(int head) {
	int root;
	int next;

	root = NULL;

	while(neq(NULL, head)) {
		next = get_next(head);
		set_next(head, root);
		root = head;
		head = next;
	}
	return root;
}

int read_all_tokens(int a, int current) {
	int ch;
	input  = a;
	line = 1;
	token = current;
	ch = grab_byte();
	while(neq(EOF, ch)) {
		ch = get_token(ch);
	}
	return token;
}

/* Lookup escape values */
int escape_lookup(int c) {
	int c1;
	c1 = ri8(add(c,1));
	if(neq('\\', ri8(c))) {
		return ri8(c);
	}

	if(eq(c1, '0')) {
		return 0;
	} else if(eq(c1, 'a')) {
		return 7;
	} else if(eq(c1, 'b')) {
		return 8;
	} else if(eq(c1, 't')) {
		return 9;
	} else if(eq(c1, 'n')) {
		return 10;
	} else if(eq(c1, 'v')) {
		return 11;
	} else if(eq(c1, 'f')) {
		return 12;
	} else if(eq(c1, 'r')) {
		return 13;
	} else if(eq(c1, 'e')) {
		return 27;
	} else if(eq(c1, '"')) {
		return 34;
	} else if(eq(c1, '\'')) {
		return 39;
	} else if(eq(c1, '\\')) {
		return 92;
	}

	exit(add(EXIT_FAILURE, 1));
}

int parse_string(int string) {
	int collect_regular_string_reset;
	int message;

	string_index = 0;
	collect_regular_string_reset = 1;

	while(eq(collect_regular_string_reset, 1)) {
		collect_regular_string_reset = 0;
		if(eq(ri8(string),'\\')) {
			wi8(add(hold_string, string_index), escape_lookup(string));
			string = add(string, 2);
		} else {
			wi8(add(hold_string, string_index), ri8(string));
			string = add(string, 1);
		}

		string_index = add(string_index, 1);
		if(neq(ri8(string), 0)) {
			collect_regular_string_reset = 1;
		}
	}

	wi8(add(hold_string, string_index), '"');
	wi8(add(add(hold_string, string_index), 1), '\n');
	message = calloc(add(string_index, 3), 1);
	copy_string(message, hold_string, add(string_index, 2));
	reset_hold_string();
	return message;
}

int emit(int s, int head) {
	int t;
	t = calloc(1, sizeof_token_list);
	set_next(t, head);
	set_s(t, s);
	return t;
}

int emit_out(int s) {
	output_list = emit(s, output_list);
}

int uniqueID(int s, int l, int num) {
	l = emit("\n", emit(num, emit("_", emit(s, l))));
	return l;
}

int uniqueID_out(int s, int num) {
	output_list = uniqueID(s, output_list, num);
}

int sym_declare(int s, int list) {
	int a;
	a = calloc(1, sizeof_token_list);
	set_next(a, list);
	set_s(a, s);
	return a;
}

int sym_lookup(int s, int symbol_list) {
	int i;

	i = symbol_list;
	while(neq(NULL, i)) {
		if(match(get_s(i), s)) {
			return i;
		}
		i = get_next(i);
	}
	return NULL;
}

int function_call(int s) {
	int passed;
	passed = 0;
	skip("(");

	indented_emit_out("(");
	increase_indent();

	if(neq(global_token_char0(), ')')) {
		emit_out(" "); no_indent = 1;
		expression();
		indented_emit_out("push_arg\n");
		passed = 1;

		while(eq(global_token_char0(), ',')) {
			advance();
			expression();
			indented_emit_out("push_arg\n");
			passed = add(passed, 1);
		}
	} else {
		emit_out(" "); no_indent = 1;
	}

	skip(")");

	indented_emit_out("do_call %FUNCTION_");
	emit_out(s);
	emit_out(" ");


	if(neq(0, passed)) {
		emit_out("cleanup_args_bytes %");
		emit_out(int2str(mul(passed, register_size), 10, TRUE));
		emit_out("\n");
	} else {
		no_indent = 1;
	}

	decrease_indent();
	indented_emit_out(")\n");
}

int load_value() {
	no_indent = 1;
	return "load ";
}

int variable_load(int a, int is_arg) {
	indented_emit_out("local ");
	if(eq(is_arg, TRUE)) {
		emit_out("ARG_");
	} else {
		emit_out("LOCAL_");
	}
	emit_out(get_s(a));
	emit_out(" ");

	if(eq(0,match("=", global_token_string()))) {
		emit_out(load_value());
	}

}

int function_load(int a) {
	if(match("(", global_token_string())) {
		function_call(get_s(a));
		return;
	}
}

int global_load(int a) {
	indented_emit_out("global &GLOBAL_");
	emit_out(get_s(a));
	emit_out(" ");

	if(match("=", global_token_string())) {
		return;
	}
	emit_out(load_value());
}

int primary_expr_string() {
	int number_string;
	number_string = int2str(current_count, 10, TRUE);
	current_count = add(current_count, 1);
	indented_emit_out("constant &STRING_");
	uniqueID_out(get_s(function), number_string);

	/* The target */
	strings_list = emit(":STRING_", strings_list);
	strings_list = uniqueID(get_s(function), strings_list, number_string);

	/* Parse the string */
	if(neq('"', ri8(get_s(get_next(global_token))))) {
		strings_list = emit(parse_string(global_token_string()), strings_list);
		advance();
	}
}

int primary_expr_char() {
	indented_emit_out("constant %");
	emit_out(int2str(escape_lookup(add(global_token_string(), 1)), 10, TRUE));
	emit_out(" "); no_indent = 1;
	advance();
}

int primary_expr_number() {
	indented_emit_out("constant %");
	emit_out(global_token_string());
	emit_out(" "); no_indent = 1;
	advance();
}

int primary_expr_variable() {
	int a;
	int s;

	s = global_token_string();
	advance();

	a = sym_lookup(s, get_locals(function));
	if(neq(NULL, a)) {
		variable_load(a, FALSE);
		return;
	}

	a = sym_lookup(s, get_arguments(function));
	if(neq(NULL, a)) {
		variable_load(a, TRUE);
		return;
	}

	a = sym_lookup(s, global_function_list);
	if(neq(NULL, a)) {
		function_load(a);
		return;
	}

	a = sym_lookup(s, global_symbol_list);
	if(neq(NULL, a)) {
		global_load(a);
		return;
	}

	exit(add(EXIT_FAILURE, 2));
}

int expression() {
	if(eq(global_token_char0(), '(')) {
		advance();
		expression();
		skip(")");
	} else if(eq(global_token_char0(), '\'')) {
		primary_expr_char();
	} else if(eq(global_token_char0(), '"')) {
		primary_expr_string();
	} else if(in_set(global_token_char0(), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")) {
		primary_expr_variable();
	} else if(in_set(global_token_char0(), "0123456789")) {
		primary_expr_number();
	} else {
		exit(1);
	}

	if(match("=", global_token_string())) {
		emit_out("push_address\n");
		advance();
		expression();
		indented_emit_out("store\n");
	}
}

/* Process local variable */
int collect_local() {
	int a;

	advance();
	a = sym_declare(global_token_string(), get_locals(function));

        if(and(match("main", get_s(function)), eq(NULL, get_locals(function)))) {
                set_depth(a, sub(0, 20));
        } else if(and(eq(NULL, get_arguments(function)),
			eq(NULL, get_locals(function)))) {
		set_depth(a, sub(0, mul(register_size, 2)));
	} else if(eq(NULL, get_locals(function))) {
		set_depth(a, sub(get_depth(get_arguments(function)),
					mul(register_size, 2)));
	} else {
		set_depth(a, sub(get_depth(get_locals(function)),
						register_size));
	}

	set_locals(function, a);

	indented_emit_out("DEFINE LOCAL_");
	emit_out(global_token_string());
	emit_out(" ");
	emit_out(to_hex_le(get_depth(a)));
	emit_out("\n");

	advance();

	if(match("=", global_token_string())) {
		advance();
		expression();
	}

	skip(";");

	indented_emit_out("reserve_stack_slot\n");
}

/* Evaluate if statements */
int process_if() {
	int number_string;
	number_string = int2str(current_count, 10, TRUE);
	current_count = add(current_count, 1);

	emit_out("# IF_");
	uniqueID_out(get_s(function), number_string);

	advance();
	skip("(");
	expression();

	indented_emit_out("jump_false %ELSE_");

	uniqueID_out(get_s(function), number_string);

	skip(")");
	statement();

	indented_emit_out("jump %_END_IF_");

	uniqueID_out(get_s(function), number_string);

	emit_out(":ELSE_");
	uniqueID_out(get_s(function), number_string);

	if(match("else", global_token_string())) {
		advance();
		statement();
	}
	emit_out(":_END_IF_");
	uniqueID_out(get_s(function), number_string);
}

/* Process Assembly statements */
int process_asm() {
	advance();
	skip("(");
	while(eq('"', global_token_char0())) {
		emit_out(add(global_token_string(), 1));
		emit_out("\n");
		advance();
	}
	skip(")");
	skip(";");
}

/* Process while loops */
int process_while() {
	int nested_locals;
	int nested_break_head;
	int nested_break_func;
	int nested_break_num;
	int number_string;

	nested_locals = break_frame;
	nested_break_head = break_target_head;
	nested_break_func = break_target_func;
	nested_break_num = break_target_num;
	number_string = int2str(current_count, 10, TRUE);

	current_count = add(current_count, 1);

	break_target_head = "END_WHILE_";
	break_target_num = number_string;
	break_frame = get_locals(function);
	break_target_func = get_s(function);

	emit_out(":WHILE_");
	uniqueID_out(get_s(function), number_string);

	advance();
	skip("(");
	expression();

	indented_emit_out("jump_false %END_WHILE_");

	uniqueID_out(get_s(function), number_string);

	emit_out("# THEN_while_");
	uniqueID_out(get_s(function), number_string);

	skip(")");
	statement();

	indented_emit_out("jump %WHILE_");

	uniqueID_out(get_s(function), number_string);

	emit_out(":END_WHILE_");
	uniqueID_out(get_s(function), number_string);

	break_target_head = nested_break_head;
	break_target_func = nested_break_func;
	break_target_num = nested_break_num;
	break_frame = nested_locals;
}

/* Ensure that functions return */
int return_result() {
	int i;
	int size_local_var;
	int c;
	c = 0;

	advance();
	if(neq(global_token_char0(), ';')) {
		expression();
	}
	skip(";");
	i = get_locals(function);
	while(neq(NULL, i)) {
		i = get_next(i);
		c = add(c, 1);
	}
	if(neq(0, c)) {
		indented_emit_out("cleanup_locals_bytes %");
		emit_out(int2str(mul(c, register_size), 10, TRUE));
		emit_out(" ");
		no_indent = 1;
	}
	indented_emit_out("ret\n");
}

int process_break() {
	int i;
	i = get_locals(function);

	advance();
	indented_emit_out("jump %");
	emit_out(break_target_head);
	emit_out(break_target_func);
	emit_out("_");
	emit_out(break_target_num);
	emit_out("\n");
	skip(";");
}

void recursive_statement() {
	int frame;
	int i;
	int c;
	c = 0;

	advance();
	frame = get_locals(function);
	while(eq(0, match("}", global_token_string()))) {
		statement();
	}
	advance();

	/* Clean up any locals added */
	if(eq(0, match("ret\n", get_s(output_list)))) {
		i = get_locals(function);
		while(neq(frame,i)) {
			c = add(c, 1);
			i = get_next(i);
		}
		if(neq(0, c)) {
			indented_emit_out("cleanup_locals_bytes %");
			emit_out(int2str(mul(c, register_size), 10, TRUE));
			emit_out(" ");
			no_indent = 1;
		}
	}
	set_locals(function, frame);
}

int statement() {
	if(eq(global_token_char0(), '{')) {
		recursive_statement();
	} else if(match("int", global_token_string())) {
		collect_local();
	} else if(match("if", global_token_string())) {
		process_if();
	} else if(match("while", global_token_string())) {
		process_while();
	} else if(match("asm", global_token_string())) {
		process_asm();
	} else if(match("return", global_token_string())) {
		return_result();
	} else if(match("break", global_token_string())) {
		process_break();
	} else {
		expression();
		skip(";");
	}
}

/* Collect function arguments */
int collect_arguments() {
	int a;

	advance();
	while(eq(0, match(")", global_token_string()))) {
		advance();
		if(neq(global_token_char0(), ',')) {
			/* deal with foo(int a, char b) */
			a = sym_declare(global_token_string(),
						get_arguments(function));
			if(eq(NULL, get_arguments(function))) {
				set_depth(a, sub(0, register_size));
			} else {
				set_depth(a, sub(get_depth(
						get_arguments(function)),
						register_size));
			}
			advance();

			set_arguments(function, a);
		}

		/* ignore trailing comma (needed for foo(bar(), 1); expressions*/
		if(eq(global_token_char0(), ',')) {
			advance();
		}

	}
	advance();
}

int declare_function() {
	int a;
	current_count = 0;
	function = sym_declare(get_s(get_prev(global_token)),
				global_function_list);

	/* allow previously defined functions to be looked up */
	global_function_list = function;
	collect_arguments();

	/* If just a prototype don't waste time */
	if(eq(global_token_char0(), ';')) {
		advance();
	} else {
		emit_out(":FUNCTION_");
		emit_out(get_s(function));
		increase_indent();
		emit_out("\n");

		a = get_arguments(function);
		while(neq(0, a)) {
			indented_emit_out("DEFINE ARG_");
			emit_out(get_s(a));
			emit_out(" ");
			emit_out(to_hex_le(get_depth(a)));
			emit_out("\n");
			a = get_next(a);
		}
		statement();

		/* Prevent duplicate RETURNS */
		if(eq(0, match("ret\n", get_s(output_list)))) {
			indented_emit_out("ret\n");
		}
		decrease_indent();
	}
}

int program() {
	int i;
	int new_type;
	function = NULL;

	new_type = 1;
	while(eq(new_type, 1)) {
		new_type = 0;
		/* Deal with garbage input */
		if (eq(NULL, global_token)) {
			return;
		}

		advance();

		/* Add to global symbol table */
		global_symbol_list = sym_declare(global_token_string(), global_symbol_list);
		advance();

		/* Deal with global variables */
		if(match(";", global_token_string())) {
			/* Ensure enough bytes are allocated to store global variable.
			   In some cases it allocates too much but that is harmless. */
			globals_list = emit(":GLOBAL_", globals_list);
			globals_list = emit(get_s(get_prev(global_token)),
						globals_list);

			i = 1;
			globals_list = emit("\n", globals_list);
			while(neq(i, 0)) {
				globals_list = emit("NULL\n", globals_list);
				i = sub(i, 1);
			}
			advance();
			new_type = 1;
		}

		/* Deal with global functions */
		if(match("(", global_token_string())) {
			declare_function();
			new_type = 1;
		}
	}

	/* Everything else is just an error */
	exit(add(EXIT_FAILURE, 3));
}

int recursive_output(int head, int out) {
	int i;
	i = reverse_list(head);
	while(neq(NULL, i)) {
		fputs(get_s(i), out);
		i = get_next(i);
	}
}

int initialize_globals() {
	NULL = 0;
	register_size = 4;
	FALSE = 0;
	TRUE = 1;
	MAX_STRING = 4096;

	EXIT_SUCCESS = 0;
	EXIT_FAILURE = 1;

	EOF = sub(0, 1);

	sizeof_token_list = mul(register_size, 4);

	token_list_layout_init();

	quote_string=calloc(1, 16); /* round up */
	wi8(quote_string, '\'');
	wi8(add(quote_string, 1), '"');
	wi8(add(quote_string, 2), 0);

	indent = 0;
	no_indent = 0;
}

int main(int argc, int argv) {
	int in;
	int destination_file;
	int name;

	init_support();
	initialize_globals();

	hold_string = calloc(add(MAX_STRING, 4), 1);

	name = ri32(add(argv, 4));

	in = fopen(name, "r");
	global_token = read_all_tokens(in, global_token);
	fclose(in);

	destination_file = fopen(ri32(add(argv,mul(4, 2))), "w");

	global_token = reverse_list(global_token);

	reset_hold_string();
	output_list = NULL;
	program();

	/* Output the program we have compiled */
	fputs("\n# Core program\n", destination_file);
	recursive_output(output_list, destination_file);
	fputs("\n# Program global variables\n", destination_file);
	recursive_output(globals_list, destination_file);
	fputs("\n# Program strings\n", destination_file);
	recursive_output(strings_list, destination_file);
	fputs("\n:ELF_end\n", destination_file);

	fclose(destination_file);
	return EXIT_SUCCESS;
}
