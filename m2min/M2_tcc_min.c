/* This is a heavily cut down version of M2-Planet
 * mofications (C) 2024 Liam Wilson (under same license)
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gcc_req.h"

#define FALSE 0
#define TRUE 1

void copy_string(char* target, char* source, int max);
int in_set(int c, char* s);
int match(char* a, char* b);
void reset_hold_string();

struct type {
	struct type* next;
	int size;
	struct type* type;
	char* name;
};

struct token_list {
	struct token_list* next;
	union {
		struct token_list* locals;
		struct token_list* prev;
	};
	char* s;
	union {
		struct type* type;
		char* filename;
	};
	union {
		struct token_list* arguments;
		int depth;
		int linenumber;
	};
};

/* The core functions */
void initialize_types();
struct token_list* read_all_tokens(FILE* a, struct token_list* current, char* filename);
struct token_list* reverse_list(struct token_list* head);

void eat_newline_tokens();
void program();
void recursive_output(struct token_list* i, FILE* out);

/* What types we have */
struct type* global_types;
struct type* prim_types;

/* What we are currently working on */
struct token_list* global_token;

/* Output reorder collections*/
struct token_list* output_list;
struct token_list* strings_list;
struct token_list* globals_list;

/* Make our string collection more efficient */
char* hold_string;
int string_index;

/* Our Target Architecture */
int register_size;

int MAX_STRING;
struct type* integer;

/* Globals */
FILE* input;
struct token_list* token;
int line;
char* file;

void line_error_token(struct token_list* list);
struct token_list* eat_token(struct token_list* head);

/* point where we are currently modifying the global_token list */
struct token_list* macro_token;

/* Global lists */
struct token_list* global_symbol_list;
struct token_list* global_function_list;
struct token_list* global_constant_list;

/* Core lists for this file */
struct token_list* function;

/* What we are currently working on */
struct type* current_target;
char* break_target_head;
char* break_target_func;
char* break_target_num;
struct token_list* break_frame;
int current_count;

/* Imported functions */
char* int2str(int x, int base, int signed_p);
char* parse_string(char* string);
int escape_lookup(char* c);
struct token_list* reverse_list(struct token_list* head);
struct type* add_primitive(struct type* a);

struct token_list* emit(char *s, struct token_list* head);
int member_size;

void expression();
void primary_expr();
struct type* type_name();
void statement();


void skip(char* str) {
/* dummy impl should check and abort if doesn't match */
	global_token = global_token->next;
}

int match(char* a, char* b) {
	if((NULL == a) && (NULL == b)) return TRUE;
	if(NULL == a) return FALSE;
	if(NULL == b) return FALSE;

	int i = -1;
	do {
		i = i + 1;
		if(a[i] != b[i]) {
			return FALSE;
		}
	} while((0 != a[i]) && (0 !=b[i]));
	return TRUE;
}

int in_set(int c, char* s) {
	/* NULL set is always false */
	if(NULL == s) return FALSE;

	while(0 != s[0]) {
		if(c == s[0]) return TRUE;
		s = s + 1;
	}
	return FALSE;
}

char* int2str(int x, int base, int signed_p) {
	/* Be overly conservative and save space for 32binary digits and padding null */
	char* p = calloc(34, sizeof(char));
	/* if calloc fails return null to let calling code deal with it */
	if(NULL == p) return p;

	p = p + 32;
	unsigned i;
	int sign_p = FALSE;
	char* table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if(signed_p && (10 == base) && (0 != (x & 0x80000000))) {
		/* Truncate to 31bits */
		i = -x & 0x7FFFFFFF;
		if(0 == i) return "-2147483648";
		sign_p = TRUE;
	} else {
		/* Truncate to 32bits */
		i = x & (0x7FFFFFFF | (1 << 31));
	}

	do {
		p[0] = table[i % base];
		p = p - 1;
		i = i / base;
	} while(0 < i);

	if(sign_p) {
		p[0] = '-';
		p = p - 1;
	}

	return p + 1;
}

int grab_byte() {
	int c = fgetc(input);
	if(10 == c) line = line + 1;
	return c;
}

int clearWhiteSpace(int c) {
	if((32 == c) || (9 == c)) return clearWhiteSpace(grab_byte());
	return c;
}

int consume_byte(int c) {
	hold_string[string_index] = c;
	string_index = string_index + 1;
	return grab_byte();
}

int preserve_string(int c) {
	int frequent = c;
	int escape = FALSE;
	do {
		if(!escape && '\\' == c ) escape = TRUE;
		else escape = FALSE;
		c = consume_byte(c);
	} while(escape || (c != frequent));
	return grab_byte();
}

void copy_string(char* target, char* source, int max) {
	int i = 0;
	while(0 != source[i]) {
		target[i] = source[i];
		i = i + 1;
		if(i == max) break;
	}
}

int preserve_keyword(int c, char* S) {
	while(in_set(c, S)) {
		c = consume_byte(c);
	}
	return c;
}

void reset_hold_string() {
	int i = MAX_STRING;
	while(0 <= i) {
		hold_string[i] = 0;
		i = i - 1;
	}
	string_index = 0;
}

/* note if this is the first token in the list, head needs fixing up */
struct token_list* eat_token(struct token_list* token) {
	if(NULL != token->prev) {
		token->prev->next = token->next;
	}

	/* update backlinks */
	if(NULL != token->next) {
		token->next->prev = token->prev;
	}

	return token->next;
}

void new_token(char* s, int size) {
	struct token_list* current = calloc(1, sizeof(struct token_list));

	/* More efficiently allocate memory for string */
	current->s = calloc(size, sizeof(char));
	copy_string(current->s, s, MAX_STRING);

	current->prev = token;
	current->next = token;
	current->linenumber = line;
	current->filename = file;
	token = current;
}

int get_token(int c) {
	struct token_list* current = calloc(1, sizeof(struct token_list));

reset:
	reset_hold_string();
	string_index = 0;

	c = clearWhiteSpace(c);
	if(c == EOF) {
		free(current);
		return c;
	} else if(in_set(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")) {
		c = preserve_keyword(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
	} else if(in_set(c, "<=>|&!^%")) {
		c = preserve_keyword(c, "<=>|&!^%");
	} else if(in_set(c, "'\"")) {
		c = preserve_string(c);
	} else if(c == '/') {
		c = consume_byte(c);
		if(c == '*') {
			c = grab_byte();
			while(c != '/') {
				while(c != '*') {
					c = grab_byte();
				}
				c = grab_byte();
			}
			c = grab_byte();
			goto reset;
		}
	} else if (c == '\n') {
		c = consume_byte(c);
	} else if(c == '-') {
		c = consume_byte(c);
	} else {
		c = consume_byte(c);
	}

	new_token(hold_string, string_index + 2);
	return c;
}

struct token_list* reverse_list(struct token_list* head) {
	struct token_list* root = NULL;
	struct token_list* next;
	while(NULL != head) {
		next = head->next;
		head->next = root;
		root = head;
		head = next;
	}
	return root;
}

struct token_list* read_all_tokens(FILE* a, struct token_list* current, char* filename) {
	input  = a;
	line = 1;
	file = filename;
	token = current;
	int ch = grab_byte();
	while(EOF != ch) {
		ch = get_token(ch);
	}

	return token;
}

/* Lookup escape values */
int escape_lookup(char* c) {
	if('\\' != c[0]) return c[0];

	if(c[1] == '0') return 0;
	else if(c[1] == 'a') return 7;
	else if(c[1] == 'b') return 8;
	else if(c[1] == 't') return 9;
	else if(c[1] == 'n') return 10;
	else if(c[1] == 'v') return 11;
	else if(c[1] == 'f') return 12;
	else if(c[1] == 'r') return 13;
	else if(c[1] == 'e') return 27;
	else if(c[1] == '"') return 34;
	else if(c[1] == '\'') return 39;
	else if(c[1] == '\\') return 92;

	fputs("Unknown escape received: ", stderr);
	fputs(c, stderr);
	fputs(" Unable to process\n", stderr);
	exit(EXIT_FAILURE);
}

/* Deal with human strings */
char* collect_regular_string(char* string) {
	string_index = 0;

collect_regular_string_reset:
	if(string[0] == '\\') {
		hold_string[string_index] = escape_lookup(string);
		if (string[1] == 'x') string = string + 2;
		string = string + 2;
	} else {
		hold_string[string_index] = string[0];
		string = string + 1;
	}

	string_index = string_index + 1;
	if(string[0] != 0) goto collect_regular_string_reset;

	hold_string[string_index] = '"';
	hold_string[string_index + 1] = '\n';
	char* message = calloc(string_index + 3, sizeof(char));
	copy_string(message, hold_string, string_index + 2);
	reset_hold_string();
	return message;
}

/* Parse string to deal with hex characters*/
char* parse_string(char* string) {
	/* the string */
	return collect_regular_string(string);
}

/* enable easy primitive creation */
struct type* new_primitive(char* name0, char* name1, char* name2, int size, int sign) {
	struct type* r = calloc(1, sizeof(struct type));
	r->name = name0;
	r->size = size;
	r->type = r;

	return r;
}

/* Initialize default types */
void initialize_types() {
	struct type* r = calloc(1, sizeof(struct type));
	register_size = 4;
	r->name = "int";
	r->size = register_size;
	r->type = r;

	/* Define int */
	integer = r;
	prim_types = integer;

	global_types = prim_types;
}

struct type* lookup_type(char* s, struct type* start) {
	struct type* i;
	for(i = start; NULL != i; i = i->next) {
		if(match(i->name, s)) {
			return i;
		}
	}
	return NULL;
}

struct type* type_name() {
	struct type* ret;
	ret = lookup_type(global_token->s, global_types);
	global_token = global_token->next;
	return ret;
}

struct token_list* emit(char *s, struct token_list* head) {
	struct token_list* t = calloc(1, sizeof(struct token_list));
	t->next = head;
	t->s = s;
	return t;
}

void emit_out(char* s) {
	output_list = emit(s, output_list);
}

struct token_list* uniqueID(char* s, struct token_list* l, char* num) {
	l = emit("\n", emit(num, emit("_", emit(s, l))));
	return l;
}

void uniqueID_out(char* s, char* num) {
	output_list = uniqueID(s, output_list, num);
}

struct token_list* sym_declare(char *s, struct type* t, struct token_list* list) {
	struct token_list* a = calloc(1, sizeof(struct token_list));
	a->next = list;
	a->s = s;
	a->type = t;
	return a;
}

struct token_list* sym_lookup(char *s, struct token_list* symbol_list) {
	struct token_list* i;
	for(i = symbol_list; NULL != i; i = i->next) {
		if(match(i->s, s)) return i;
	}
	return NULL;
}

void function_call(char* s) {
	skip("(");
	int passed = 0;

	emit_out("push_edi\t# Prevent overwriting in recursion\n");
	emit_out("push_ebp\t# Protect the old base pointer\n");
	emit_out("mov_edi,esp\t# Copy new base pointer\n");

	if(global_token->s[0] != ')') {
		expression();
		emit_out("push_eax\t#_process_expression1\n");
		passed = 1;

		while(global_token->s[0] == ',') {
			global_token = global_token->next;
			expression();
			emit_out("push_eax\t#_process_expression2\n");
			passed = passed + 1;
		}
	}

	skip(")");

	emit_out("mov_ebp,edi\n");
	emit_out("call %FUNCTION_");
	emit_out(s);
	emit_out("\n");

	for(; passed > 0; passed = passed - 1) {
		emit_out("pop_ebx\t# _process_expression_locals\n");
	}

	emit_out("pop_ebp\t# Restore old base pointer\n");
	emit_out("pop_edi\t# Prevent overwrite\n");
}

char* load_value() {
	return "mov_eax,[eax]\n";
}

char* store_value() {
	return "mov_[ebx],eax\n";
}

void variable_load(struct token_list* a) {

	current_target = a->type;

	emit_out("lea_eax,[ebp+DWORD] %");
	emit_out(int2str(a->depth, 10, TRUE));
	emit_out("\n");

	if(!match("=", global_token->s)) {
		emit_out(load_value());
	}

}

void function_load(struct token_list* a) {
	if(match("(", global_token->s)) {
		function_call(a->s);
		return;
	}
}

void global_load(struct token_list* a) {
	current_target = a->type;
	emit_out("mov_eax, &GLOBAL_");
	emit_out(a->s);
	emit_out("\n");

	if(match("=", global_token->s)) return;

	emit_out(load_value());
}

void primary_expr_string() {
	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;
	emit_out("mov_eax, &STRING_");
	uniqueID_out(function->s, number_string);

	/* The target */
	strings_list = emit(":STRING_", strings_list);
	strings_list = uniqueID(function->s, strings_list, number_string);

	/* Parse the string */
	if('"' != global_token->next->s[0]) {
		strings_list = emit(parse_string(global_token->s), strings_list);
		global_token = global_token->next;
	}
}

void primary_expr_char() {
	emit_out("mov_eax, %");
	emit_out(int2str(escape_lookup(global_token->s + 1), 10, TRUE));
	emit_out("\n");
	global_token = global_token->next;
}

void primary_expr_number() {
	emit_out("mov_eax, %");
	emit_out(global_token->s);
	emit_out("\n");
	global_token = global_token->next;
}

void primary_expr_variable() {
	int num_dereference = 0;
	char* s = global_token->s;
	global_token = global_token->next;
	struct token_list* a = sym_lookup(s, global_constant_list);

	a = sym_lookup(s, function->locals);
	if(NULL != a) {
		variable_load(a);
		return;
	}

	a = sym_lookup(s, function->arguments);
	if(NULL != a) {
		variable_load(a);
		return;
	}

	a = sym_lookup(s, global_function_list);
	if(NULL != a) {
		function_load(a);
		return;
	}

	a = sym_lookup(s, global_symbol_list);
	if(NULL != a) {
		global_load(a);
		return;
	}

	fputs(s ,stderr);
	fputs(" is not a defined symbol\n", stderr);
	exit(EXIT_FAILURE);
}

void common_recursion(FUNCTION f) {
	emit_out("push_eax\t#_common_recursion\n");

	struct type* last_type = current_target;
	global_token = global_token->next;
	f();

	emit_out("pop_ebx\t# _common_recursion\n");
}

void primary_expr() {
	if('-' == global_token->s[0]) {
		emit_out("mov_eax, %0\n");
		common_recursion(primary_expr);
		emit_out("sub_ebx,eax\nmov_eax,ebx\n");
	} else if(global_token->s[0] == '(') {
		global_token = global_token->next;
		expression();
		skip(")");
	} else if(global_token->s[0] == '\'') {
		primary_expr_char();
	} else if(global_token->s[0] == '"') {
		primary_expr_string();
	} else if(in_set(global_token->s[0], "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")) {
		primary_expr_variable();
	} else if(in_set(global_token->s[0], "0123456789")) {
		primary_expr_number();
	} else {
		exit(1);
	}
}

void expression() {
	primary_expr();
	if(match("=", global_token->s)) {
		char* store = "";
		store = store_value();
		common_recursion(expression);
		emit_out(store);
		current_target = integer;
	}
}

/* Process local variable */
void collect_local() {
	struct type* type_size = type_name();
	struct token_list* a = sym_declare(global_token->s, type_size, function->locals);
	if(match("main", function->s) && (NULL == function->locals)) {
		a->depth = -20;
	} else if((NULL == function->arguments) && (NULL == function->locals)) {
		a->depth = -8;
	} else if(NULL == function->locals) {
		a->depth = function->arguments->depth - 8;
	} else {
		a->depth = function->locals->depth - register_size;
	}

	function->locals = a;

	emit_out("# Defining local ");
	emit_out(global_token->s);
	emit_out("\n");

	global_token = global_token->next;

	if(match("=", global_token->s)) {
		global_token = global_token->next;
		expression();
	}

	skip(";");

	unsigned i = (a->type->size + register_size - 1) / register_size;
	while(i != 0) {
		emit_out("push_eax\t#");
		emit_out(a->s);
		emit_out("\n");
		i = i - 1;
	}
}

/* Evaluate if statements */
void process_if() {
	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	emit_out("# IF_");
	uniqueID_out(function->s, number_string);

	global_token = global_token->next;
	skip("(");
	expression();

	emit_out("test_eax,eax\nje %ELSE_");

	uniqueID_out(function->s, number_string);

	skip(")");
	statement();

	emit_out("jmp %_END_IF_");

	uniqueID_out(function->s, number_string);

	emit_out(":ELSE_");
	uniqueID_out(function->s, number_string);

	if(match("else", global_token->s)) {
		global_token = global_token->next;
		statement();
	}
	emit_out(":_END_IF_");
	uniqueID_out(function->s, number_string);
}

/* Process Assembly statements */
void process_asm() {
	global_token = global_token->next;
	skip("(");
	while('"' == global_token->s[0]) {
		emit_out((global_token->s + 1));
		emit_out("\n");
		global_token = global_token->next;
	}
	skip(")");
	skip(";");
}

/* Process while loops */
void process_while() {
	struct token_list* nested_locals = break_frame;
	char* nested_break_head = break_target_head;
	char* nested_break_func = break_target_func;
	char* nested_break_num = break_target_num;

	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	break_target_head = "END_WHILE_";
	break_target_num = number_string;
	break_frame = function->locals;
	break_target_func = function->s;

	emit_out(":WHILE_");
	uniqueID_out(function->s, number_string);

	global_token = global_token->next;
	skip("(");
	expression();

	emit_out("test_eax,eax\nje %END_WHILE_");

	uniqueID_out(function->s, number_string);

	emit_out("# THEN_while_");
	uniqueID_out(function->s, number_string);

	skip(")");
	statement();

	emit_out("jmp %WHILE_");

	uniqueID_out(function->s, number_string);

	emit_out(":END_WHILE_");
	uniqueID_out(function->s, number_string);

	break_target_head = nested_break_head;
	break_target_func = nested_break_func;
	break_target_num = nested_break_num;
	break_frame = nested_locals;
}

/* Ensure that functions return */
void return_result() {
	global_token = global_token->next;
	if(global_token->s[0] != ';') expression();

	skip(";");

	struct token_list* i;
	unsigned size_local_var;
	for(i = function->locals; NULL != i; i = i->next) {
		emit_out("pop_ebx\t# _return_result_locals\n");
	}

	emit_out("ret\n");
}

void process_break() {
	struct token_list* i = function->locals;
	global_token = global_token->next;

	emit_out("jmp %");

	emit_out(break_target_head);
	emit_out(break_target_func);
	emit_out("_");
	emit_out(break_target_num);
	emit_out("\n");
	skip(";");
}

void recursive_statement() {
	global_token = global_token->next;
	struct token_list* frame = function->locals;

	while(!match("}", global_token->s)) {
		statement();
	}
	global_token = global_token->next;

	/* Clean up any locals added */

	if( !match("ret\n", output_list->s)) {
		struct token_list* i;
		for(i = function->locals; frame != i; i = i->next) {
			emit_out( "pop_ebx\t# _recursive_statement_locals\n");
		}
	}
	function->locals = frame;
}

struct type* lookup_type(char* s, struct type* start);
void statement() {
	/* Always an integer until told otherwise */
	current_target = integer;

	if(global_token->s[0] == '{') {
		recursive_statement();
	} else if((NULL != lookup_type(global_token->s, prim_types))) {
		collect_local();
	} else if(match("if", global_token->s)) {
		process_if();
	} else if(match("while", global_token->s)) {
		process_while();
	} else if(match("asm", global_token->s)) {
		process_asm();
	} else if(match("return", global_token->s)) {
		return_result();
	} else if(match("break", global_token->s)) {
		process_break();
	} else {
		expression();
		skip(";");
	}
}

/* Collect function arguments */
void collect_arguments() {
	global_token = global_token->next;
	struct type* type_size;
	struct token_list* a;

	while(!match(")", global_token->s)) {
		type_size = type_name();
		if(global_token->s[0] == ')') {
			/* foo(int,char,void) doesn't need anything done */
			continue;
		} else if(global_token->s[0] != ',') {
			/* deal with foo(int a, char b) */
			a = sym_declare(global_token->s, type_size, function->arguments);
			if(NULL == function->arguments) {
				a->depth = -4;
			} else {
				a->depth = function->arguments->depth - register_size;
			}

			global_token = global_token->next;
			function->arguments = a;
		}

		/* ignore trailing comma (needed for foo(bar(), 1); expressions*/
		if(global_token->s[0] == ',') {
			global_token = global_token->next;
		}

	}
	global_token = global_token->next;
}

void declare_function() {
	current_count = 0;
	function = sym_declare(global_token->prev->s, NULL, global_function_list);

	/* allow previously defined functions to be looked up */
	global_function_list = function;
	collect_arguments();

	/* If just a prototype don't waste time */
	if(global_token->s[0] == ';') {
		global_token = global_token->next;
	} else {
		emit_out("# Defining function ");
		emit_out(function->s);
		emit_out("\n");
		emit_out(":FUNCTION_");
		emit_out(function->s);
		emit_out("\n");
		statement();

		/* Prevent duplicate RETURNS */
		if(!match("ret\n", output_list->s)) emit_out("ret\n");
	}
}

void program() {
	unsigned i;
	function = NULL;
	struct type* type_size;

new_type:
	/* Deal with garbage input */
	if (NULL == global_token) return;

	type_size = type_name();
	/* Deal with case of struct definitions */
	if(NULL == type_size) goto new_type;

	/* Add to global symbol table */
	global_symbol_list = sym_declare(global_token->s, type_size, global_symbol_list);
	global_token = global_token->next;

	/* Deal with global variables */
	if(match(";", global_token->s)) {
		/* Ensure enough bytes are allocated to store global variable.
		   In some cases it allocates too much but that is harmless. */
		globals_list = emit(":GLOBAL_", globals_list);
		globals_list = emit(global_token->prev->s, globals_list);

		i = 1;
		globals_list = emit("\n", globals_list);
		while(i != 0) {
			globals_list = emit("NULL\n", globals_list);
			i = i - 1;
		}
		global_token = global_token->next;
		goto new_type;
	}

	/* Deal with global functions */
	if(match("(", global_token->s)) {
		declare_function();
		goto new_type;
	}

	/* Everything else is just an error */
	fputs("Received ", stderr);
	fputs(global_token->s, stderr);
	fputs(" in program\n", stderr);
	exit(EXIT_FAILURE);
}

void recursive_output(struct token_list* head, FILE* out) {
	struct token_list* i = reverse_list(head);
	while(NULL != i) {
		fputs(i->s, out);
		i = i->next;
	}
}

void eat_current_token() {
	int update_global_token = FALSE;
	if (macro_token == global_token) {
		update_global_token = TRUE;
	}

	macro_token = eat_token(macro_token);

	if(update_global_token) {
		global_token = macro_token;
	}
}

void eat_newline_tokens() {
	macro_token = global_token;

	while(TRUE) {
		if(NULL == macro_token) return;

		if(match("\n", macro_token->s)) {
			eat_current_token();
		} else {
			macro_token = macro_token->next;
		}
	}
}

int main(int argc, char** argv) {
	MAX_STRING = 4096;
	FILE* in;
	FILE* destination_file;
	char* name;
	char* hold;
	int env=0;
	char* val;

	int i = 1;
	hold_string = calloc(MAX_STRING + 4, sizeof(char));

	name = argv[i];

	in = fopen(name, "r");
	global_token = read_all_tokens(in, global_token, name);
	fclose(in);
	i = i + 1;

	destination_file = fopen(argv[i], "w");
	i = i + 1;

	env = env + 1;

	global_token = reverse_list(global_token);

	/* the main parser doesn't know how to handle newline tokens */
	eat_newline_tokens();

	initialize_types();
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
