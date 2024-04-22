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

int FALSE;
int TRUE;
int NULL;

int EXIT_SUCCESS;
int EXIT_FAILURE;

int EOF;

void copy_string(char* target, char* source, int max);
int in_set(int c, char* s);
int match(char* a, char* b);
void reset_hold_string();

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

/* The core functions */
void initialize_types();
struct token_list* read_all_tokens(int a, struct token_list* current);
struct token_list* reverse_list(struct token_list* head);

void program();
void recursive_output(struct token_list* i, int out);

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

/* Globals */
int input;
struct token_list* token;
int line;
char* file;

/* Global lists */
struct token_list* global_symbol_list;
struct token_list* global_function_list;

/* Core lists for this file */
struct token_list* function;

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

struct token_list* emit(char *s, struct token_list* head);

void expression();
void advance();
void statement();

void skip(char* str) {
/* dummy impl should check and abort if doesn't match */
	global_token = global_token->next;
}

/* advance to next token */
void advance() {
	global_token = global_token->next;
}

char *token_string(struct token_list* a) {
	return a->s;
}

char *global_token_string(void) {
	return token_string(global_token);
}

char global_token_char0() {
	return (token_string(global_token))[0];
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

void new_token(char* s, int size) {
	struct token_list* current = calloc(1, sizeof(struct token_list));

	/* More efficiently allocate memory for string */
	current->s = calloc(size, sizeof(char));
	copy_string(current->s, s, MAX_STRING);

	current->prev = token;
	current->next = token;
	current->linenumber = line;
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
	} else if(in_set(c, "=")) {
		c = preserve_keyword(c, "=");
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
/* eat newlines here */
		goto reset;
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

struct token_list* read_all_tokens(int a, struct token_list* current) {
	input  = a;
	line = 1;
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

	exit(EXIT_FAILURE + 1);
}

char* parse_string(char* string) {
	string_index = 0;

collect_regular_string_reset:
	if(string[0] == '\\') {
		hold_string[string_index] = escape_lookup(string);
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

struct token_list* sym_declare(char *s, struct token_list* list) {
	struct token_list* a = calloc(1, sizeof(struct token_list));
	a->next = list;
	a->s = s;
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

	if(global_token_char0() != ')') {
		expression();
		emit_out("push_eax\t#_process_expression1\n");
		passed = 1;

		while(global_token_char0() == ',') {
			advance();
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


	emit_out("lea_eax,[ebp+DWORD] %");
	emit_out(int2str(a->depth, 10, TRUE));
	emit_out("\n");

	if(!match("=", global_token_string())) {
		emit_out(load_value());
	}

}

void function_load(struct token_list* a) {
	if(match("(", global_token_string())) {
		function_call(a->s);
		return;
	}
}

void global_load(struct token_list* a) {
	emit_out("mov_eax, &GLOBAL_");
	emit_out(a->s);
	emit_out("\n");

	if(match("=", global_token_string())) return;

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
		strings_list = emit(parse_string(global_token_string()), strings_list);
		advance();
	}
}

void primary_expr_char() {
	emit_out("mov_eax, %");
	emit_out(int2str(escape_lookup(global_token_string() + 1), 10, TRUE));
	emit_out("\n");
	advance();
}

void primary_expr_number() {
	emit_out("mov_eax, %");
	emit_out(global_token_string());
	emit_out("\n");
	advance();
}

void primary_expr_variable() {
	struct token_list* a;
	char* s = global_token_string();
	advance();

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

	exit(EXIT_FAILURE + 2);
}

void expression() {
	if(global_token_char0() == '(') {
		advance();
		expression();
		skip(")");
	} else if(global_token_char0() == '\'') {
		primary_expr_char();
	} else if(global_token_char0() == '"') {
		primary_expr_string();
	} else if(in_set(global_token_char0(), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")) {
		primary_expr_variable();
	} else if(in_set(global_token_char0(), "0123456789")) {
		primary_expr_number();
	} else {
		exit(1);
	}

	if(match("=", global_token_string())) {
		char* store = "";
		store = store_value();
		emit_out("push_eax\t#_common_recursion\n");
		advance();
		expression();
		emit_out("pop_ebx\t# _common_recursion\n");
		emit_out(store);
	}
}

/* Process local variable */
void collect_local() {
	advance();
	struct token_list* a = sym_declare(global_token_string(), function->locals);
	if((NULL == function->arguments) && (NULL == function->locals)) {
		a->depth = -(register_size * 2);
	} else if(NULL == function->locals) {
		a->depth = function->arguments->depth - (register_size * 2);
	} else {
		a->depth = function->locals->depth - register_size;
	}

	function->locals = a;

	emit_out("# Defining local ");
	emit_out(global_token_string());
	emit_out("\n");

	advance();

	if(match("=", global_token_string())) {
		advance();
		expression();
	}

	skip(";");

	emit_out("push_eax\t#");
	emit_out(a->s);
	emit_out("\n");
}

/* Evaluate if statements */
void process_if() {
	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	emit_out("# IF_");
	uniqueID_out(function->s, number_string);

	advance();
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

	if(match("else", global_token_string())) {
		advance();
		statement();
	}
	emit_out(":_END_IF_");
	uniqueID_out(function->s, number_string);
}

/* Process Assembly statements */
void process_asm() {
	advance();
	skip("(");
	while('"' == global_token_char0()) {
		emit_out((global_token_string() + 1));
		emit_out("\n");
		advance();
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

	advance();
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
	advance();
	if(global_token_char0() != ';') expression();

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
	advance();

	emit_out("jmp %");

	emit_out(break_target_head);
	emit_out(break_target_func);
	emit_out("_");
	emit_out(break_target_num);
	emit_out("\n");
	skip(";");
}

void recursive_statement() {
	advance();
	struct token_list* frame = function->locals;

	while(!match("}", global_token_string())) {
		statement();
	}
	advance();

	/* Clean up any locals added */

	if( !match("ret\n", output_list->s)) {
		struct token_list* i;
		for(i = function->locals; frame != i; i = i->next) {
			emit_out( "pop_ebx\t# _recursive_statement_locals\n");
		}
	}
	function->locals = frame;
}

void statement() {
	if(global_token_char0() == '{') {
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
void collect_arguments() {
	advance();
	struct token_list* a;

	while(!match(")", global_token_string())) {
		advance();
		if(global_token_char0() != ',') {
			/* deal with foo(int a, char b) */
			a = sym_declare(global_token_string(), function->arguments);
			if(NULL == function->arguments) {
				a->depth = -register_size;
			} else {
				a->depth = function->arguments->depth - register_size;
			}

			advance();
			function->arguments = a;
		}

		/* ignore trailing comma (needed for foo(bar(), 1); expressions*/
		if(global_token_char0() == ',') {
			advance();
		}

	}
	advance();
}

void declare_function() {
	current_count = 0;
	function = sym_declare(global_token->prev->s, global_function_list);

	/* allow previously defined functions to be looked up */
	global_function_list = function;
	collect_arguments();

	/* If just a prototype don't waste time */
	if(global_token_char0() == ';') {
		advance();
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

new_type:
	/* Deal with garbage input */
	if (NULL == global_token) {
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
		globals_list = emit(global_token->prev->s, globals_list);

		i = 1;
		globals_list = emit("\n", globals_list);
		while(i != 0) {
			globals_list = emit("NULL\n", globals_list);
			i = i - 1;
		}
		advance();
		goto new_type;
	}

	/* Deal with global functions */
	if(match("(", global_token_string())) {
		declare_function();
		goto new_type;
	}

	/* Everything else is just an error */
	exit(EXIT_FAILURE + 3);
}

void recursive_output(struct token_list* head, int out) {
	struct token_list* i = reverse_list(head);
	while(neq(NULL, i)) {
		fputs(i->s, out);
		i = i->next;
	}
}

void initialize_globals() {
	NULL = 0;
	register_size = 4;
	FALSE = 0;
	TRUE = 1;
	MAX_STRING = 4096;

	EXIT_SUCCESS = 0;
	EXIT_FAILURE = 1;

	EOF = sub(0, 1);
}

int main(int argc, int argv) {
	int in;
	int destination_file;
	int name;

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
