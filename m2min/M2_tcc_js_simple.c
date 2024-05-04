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

#include "gcc_req.h"

int FALSE;
int TRUE;
int NULL;

int EXIT_SUCCESS;
int EXIT_FAILURE;

int EOF;

/* init support code */
int init_support();

struct type
{
	struct type* next;
	int size;
	int offset;
	int is_signed;
	struct type* indirect;
	struct type* members;
	struct type* type;
	char* name;
};

struct token_list
{
	struct token_list* next;
	union
	{
		struct token_list* locals;
		struct token_list* prev;
	};
	char* s;
	union
	{
		struct type* type;
		char* filename;
	};
	union
	{
		struct token_list* arguments;
		int depth;
		int linenumber;
	};
};

/* The core functions */
int initialize_types();
struct token_list* reverse_list(struct token_list* head);

struct token_list* remove_line_comment_tokens(struct token_list* head);
struct token_list* remove_preprocessor_directives(struct token_list* head);

void eat_newline_tokens();
void preprocess();
void program();
void recursive_output(struct token_list* i, int out);

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
int input;
struct token_list* token;
int line;
char* file;

struct token_list* eat_token(struct token_list* head);

/* point where we are currently modifying the global_token list */
struct token_list* macro_token;


int require(int bool, int error) {
	if(eq(0, bool)) {
		fputs(error, stderr);
		exit(EXIT_FAILURE);
	}
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
	int i;
	int p;
	int sign_p;
	int table;
	require(lt(1, base), "int2str doesn't support a base less than 2\n");
	require(gt(37, base), "int2str doesn't support a base more than 36\n");
	/* Be overly conservative and save space for 32binary digits and padding null */
	p = calloc(34, 1);
	/* if calloc fails return null to let calling code deal with it */
	if(eq(NULL, p)) {
		return p;
	}

	p = add(p, 32);
	sign_p = FALSE;
	table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if(and(and(signed_p, (eq(10, base))), neq(0, and(x, 0x80000000)))) {
		/* Truncate to 31bits */
		i = and(sub(0, x), 0x7FFFFFFF);
		if(eq(0, i)) {
			return "-2147483648";
		}
		sign_p = TRUE;
	} /* Truncate to 32bits */
	else {
		i = and(x, or(0x7FFFFFFF, shl(1, 31)));
	}

	while(1) {
		wi8(p, ri8(add(table,  mod(i, base))));
		p = sub(p, 1);
		i = div(i, base);
		if(gte(0, i)) {
			break;
		}
	}

	if(neq(0, sign_p)) {
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
	require(gt(MAX_STRING, string_index), "Token exceeded MAX_STRING char limit\nuse --max-string number to increase\n");
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
		} else {
			escape = FALSE;
		}
		c = consume_byte(c);
		require(neq(EOF, c), "Unterminated string\n");
		if(eq(0, or(escape, neq(c, frequent)))) {
			break;
		}
	}
	return grab_byte();
}


int copy_string(int target, int source, int max)
{
	int i;
	i = 0;
	while(neq(0, ri8(add(source,i)))) {
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

/* note if this is the first token in the list, head needs fixing up */
struct token_list* eat_token(struct token_list* token) {
	if(neq(NULL, token->prev)) {
		token->prev->next = token->next;
	}

	/* update backlinks */
	if(neq(NULL, token->next)) {
		token->next->prev = token->prev;
	}

	return token->next;
}

struct token_list* eat_until_newline(struct token_list* head) {
	while (neq(NULL, head)) {
		if(eq('\n', head->s[0])) {
			return head;
		} else {
			head = eat_token(head);
		}
	}

	return NULL;
}

struct token_list* remove_line_comment_tokens(struct token_list* head) {
	struct token_list* first;

	first = NULL;

	while (neq(NULL, head)) {
		if(match("//", head->s)) {
			head = eat_token(head);
		} else {
			if(eq(NULL, first)) {
				first = head;
			}
			head = head->next;
		}
	}

	return first;
}

struct token_list* remove_preprocessor_directives(struct token_list* head) {
	struct token_list* first;

	first = NULL;

	while (neq(NULL, head)) {
		if(eq('#', head->s[0])) {
			head = eat_until_newline(head);
		} else {
			if(eq(NULL, first)) {
				first = head;
			}
			head = head->next;
		}
	}

	return first;
}

int new_token(int s, int size) {
	struct token_list* current;

	current = calloc(1, sizeof(struct token_list));
	require(neq(NULL, current), "Exhausted memory while getting token\n");

	/* More efficiently allocate memory for string */
	current->s = calloc(size, sizeof(char));
	require(neq(NULL, current->s), "Exhausted memory while trying to copy a token\n");
	copy_string(current->s, s, MAX_STRING);

	current->prev = token;
	current->next = token;
	current->linenumber = line;
	current->filename = file;
	token = current;
}

int get_token(int c) {
	struct token_list* current;

	current = calloc(1, sizeof(struct token_list));
	require(neq(NULL, current), "Exhausted memory while getting token\n");

reset:
	reset_hold_string();
	string_index = 0;

	c = clearWhiteSpace(c);
	if(eq(c, EOF)) {
		free(current);
		return c;
	} else if(eq('#', c)) {
		c = consume_byte(c);
		c = preserve_keyword(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
	} else if(in_set(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")) {
		c = preserve_keyword(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
	} else if(in_set(c, "<=>|&!^%")) {
		c = preserve_keyword(c, "<=>|&!^%");
	} else if(in_set(c, "'\"")) {
		c = preserve_string(c);
	} else if(eq(c, '/')) {
		c = consume_byte(c);
		if(eq(c, '*')) {
			c = grab_byte();
			while(neq(c, '/')) {
				while(neq(c, '*')) {
					c = grab_byte();
					require(neq(EOF, c), "Hit EOF inside of block comment\n");
				}
				c = grab_byte();
				require(neq(EOF, c), "Hit EOF inside of block comment\n");
			}
			c = grab_byte();
			goto reset;
		} else if(eq(c, '/')) {
			c = consume_byte(c);
		}
	} else if (eq(c, '\n')) {
		c = consume_byte(c);
	} else if(eq(c, '*')) {
		c = consume_byte(c);
	} else if(eq(c, '+')) {
		c = consume_byte(c);
	} else if(eq(c, '-')) {
		c = consume_byte(c);
	} else {
		c = consume_byte(c);
	}

	new_token(hold_string, add(string_index, 2));
	return c;
}

struct token_list* reverse_list(struct token_list* head) {
	struct token_list* root;
	struct token_list* next;

	root = NULL;
	while(neq(NULL, head)) {
		next = head->next;
		head->next = root;
		root = head;
		head = next;
	}
	return root;
}

struct token_list* read_all_tokens(int a, struct token_list* current, int filename) {
	int ch;
	input  = a;
	line = 1;
	file = filename;
	token = current;
	ch = grab_byte();
	while(neq(EOF, ch)) {
		ch = get_token(ch);
		require(neq(NULL, token), "Empty files don't need to be compiled\n");
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

	fputs("Unknown escape received: ", stderr);
	fputs(c, stderr);
	fputs(" Unable to process\n", stderr);
	exit(EXIT_FAILURE);
}

/* Deal with human strings */
int collect_regular_string(int string) {
	int message;
	string_index = 0;

collect_regular_string_reset:
	require(gt(sub(MAX_STRING, 3), string_index), "Attempt at parsing regular string exceeds max length\n");
	if(eq(ri8(string), '\\')) {
		wi8(add(hold_string, string_index), escape_lookup(string));
		if (eq(ri8(add(string, 1)), 'x')) {
			string = add(string, 2);
		}
		string = add(string, 2);
	} else {
		wi8(add(hold_string, string_index), ri8(string));
		string = add(string, 1);
	}

	string_index = add(string_index, 1);
	if(neq(ri8(string), 0)) {
		goto collect_regular_string_reset;
	}

	wi8(add(hold_string, string_index), '"');
	wi8(add(hold_string, add(string_index, 1)), '\n');
	message = calloc(add(string_index, 3), 1);
	require(neq(NULL, message), "Exhausted memory while storing regular string\n");
	copy_string(message, hold_string, add(string_index, 2));
	reset_hold_string();
	return message;
}

/* Parse string to deal with hex characters*/
int parse_string(int string) {
	/* the string */
	return collect_regular_string(string);
}

/* enable easy primitive extension */
struct type* add_primitive(struct type* a) {
	struct type* i;
	if(eq(NULL, prim_types)) {
		return a;
	}
	i = prim_types;
	while(neq(NULL, i->next)) {
		i = i->next;
	}
	i->next = a;

	return prim_types;
}

/* enable easy primitive creation */
struct type* new_primitive(int name0, int name1, int name2, int size, int sign) {
	/* Create type** */
	struct type* a;
	struct type* b;
	struct type* r;

	a = calloc(1, sizeof(struct type));
	require(neq(NULL, a), "Exhausted memory while declaring new primitive**\n");
	a->name = name2;
	a->size = register_size;
	a->indirect = a;
	a->is_signed = sign;

	/* Create type* */
	b = calloc(1, sizeof(struct type));
	require(neq(NULL, b), "Exhausted memory while declaring new primitive*\n");
	b->name = name1;
	b->size = register_size;
	b->is_signed = sign;
	b->indirect = a;
	a->type = b;

	r = calloc(1, sizeof(struct type));
	require(neq(NULL, r), "Exhausted memory while declaring new primitive\n");
	r->name = name0;
	r->size = size;
	r->is_signed = sign;
	r->indirect = b;
	r->type = r;
	b->type = r;

	return r;
}

/* Initialize default types */
int initialize_types() {
	struct type* hold;

	register_size = 4;

	/* Define void */
	hold = new_primitive("void", "void*", "void**", register_size, FALSE);
	prim_types = add_primitive(hold);

	/* Define unsigned LONG */
	hold = new_primitive("SCM","SCM*", "SCM**", register_size, FALSE);
	prim_types = add_primitive(hold);

	/* Define LONG */
	hold = new_primitive("long", "long*", "long**", register_size, TRUE);
	prim_types = add_primitive(hold);

	/* Define UNSIGNED */
	hold = new_primitive("unsigned", "unsigned*", "unsigned**", register_size, FALSE);
	prim_types = add_primitive(hold);

	/* Define int */
	integer = new_primitive("int", "int*", "int**", register_size, TRUE);
	prim_types = add_primitive(integer);

	/* Define uint32_t */
	hold = new_primitive("uint32_t", "uint32_t*", "uint32_t**", 4, FALSE);
	prim_types = add_primitive(hold);

	/* Define int32_t */
	hold = new_primitive("int32_t", "int32_t*", "int32_t**", 4, TRUE);
	prim_types = add_primitive(hold);

	/* Define uint16_t */
	hold = new_primitive("uint16_t", "uint16_t*", "uint16_t**", 2, FALSE);
	prim_types = add_primitive(hold);

	/* Define int16_t */
	hold = new_primitive("int16_t", "int16_t*", "int16_t**", 2, TRUE);
	prim_types = add_primitive(hold);

	/* Define uint8_t */
	hold = new_primitive("uint8_t", "uint8_t*", "uint8_t**", 1, FALSE);
	prim_types = add_primitive(hold);

	/* Define int8_t */
	hold = new_primitive("int8_t", "int8_t*", "int8_t**", 1, TRUE);
	prim_types = add_primitive(hold);

	/* Define char */
	hold = new_primitive("char", "char*", "char**", 1, TRUE);
	prim_types = add_primitive(hold);

	/* Define FUNCTION */
	hold = new_primitive("FUNCTION", "FUNCTION*", "FUNCTION**", register_size, FALSE);
	prim_types = add_primitive(hold);

	/* Define FILE */
	hold = new_primitive("FILE", "FILE*", "FILE**", register_size, TRUE);
	prim_types = add_primitive(hold);

	/* Primitives mes.c wanted */
	hold = new_primitive("size_t", "size_t*", "size_t**", register_size, FALSE);
	prim_types = add_primitive(hold);

	hold = new_primitive("ssize_t", "ssize_t*", "ssize_t**", register_size, FALSE);
	prim_types = add_primitive(hold);

	global_types = prim_types;
}

struct type* lookup_type(int s, struct type* start) {
	struct type* i;
	for(i = start; neq(NULL, i); i = i->next)
	{
		if(match(i->name, s)) {
			return i;
		}
	}
	return NULL;
}

struct type* type_name();

int member_size;

struct type* type_name() {
	struct type* ret;

	require(neq(NULL, global_token), "Received EOF instead of type name\n");

	ret = lookup_type(global_token->s, global_types);

	global_token = global_token->next;
	require(neq(NULL, global_token), "unfinished type definition\n");

	while(eq(global_token->s[0], '*')) {
		ret = ret->indirect;
		global_token = global_token->next;
		require(neq(NULL, global_token), "unfinished type definition in indirection\n");
	}

	return ret;
}


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
int Address_of;

struct token_list* reverse_list(struct token_list* head);
struct type* mirror_type(struct type* source, char* name);
struct type* add_primitive(struct type* a);

struct token_list* emit(int s, struct token_list* head) {
	struct token_list* t;
	t = calloc(1, sizeof(struct token_list));
	require(neq(NULL, t), "Exhausted memory while generating token to emit\n");
	t->next = head;
	t->s = s;
	return t;
}

int emit_out(int s) {
	output_list = emit(s, output_list);
}

struct token_list* uniqueID(int s, struct token_list* l, int num) {
	l = emit("\n", emit(num, emit("_", emit(s, l))));
	return l;
}

int uniqueID_out(int s, int num) {
	output_list = uniqueID(s, output_list, num);
}

struct token_list* sym_declare(int s, struct type* t, struct token_list* list) {
	struct token_list* a;
	a = calloc(1, sizeof(struct token_list));
	require(neq(NULL, a), "Exhausted memory while attempting to declare a symbol\n");
	a->next = list;
	a->s = s;
	a->type = t;
	return a;
}

struct token_list* sym_lookup(int s, struct token_list* symbol_list) {
	struct token_list* i;
	for(i = symbol_list; neq(NULL, i); i = i->next)
	{
		if(match(i->s, s)) {
			return i;
		}
	}
	return NULL;
}

int line_error_token(struct token_list *token) {
	if(eq(NULL, token)) {
		fputs("EOF reached inside of line_error\n", stderr);
		fputs("problem at end of file\n", stderr);
		return;
	}
	fputs(token->filename, stderr);
	fputs(":", stderr);
	fputs(int2str(token->linenumber, 10, TRUE), stderr);
	fputs(":", stderr);
}

int line_error() {
	line_error_token(global_token);
}

int require_match(int message, int required) {
	if(eq(NULL, global_token)) {
		line_error();
		fputs("EOF reached inside of require match\n", stderr);
		fputs("problem at end of file\n", stderr);
		fputs(message, stderr);
		exit(EXIT_FAILURE);
	}
	if(eq(0, match(global_token->s, required))) {
		line_error();
		fputs(message, stderr);
		exit(EXIT_FAILURE);
	}
	global_token = global_token->next;
}

int expression();
int function_call(int s, int bool) {
	int passed;
	require_match("ERROR in process_expression_list\nNo ( was found\n", "(");
	require(neq(NULL, global_token), "Improper function call\n");
	passed = 0;

	emit_out("push_edi\t# Prevent overwriting in recursion\n");
	emit_out("push_ebp\t# Protect the old base pointer\n");
	emit_out("mov_edi,esp\t# Copy new base pointer\n");

	if(neq(global_token->s[0], ')')) {
		expression();
		require(neq(NULL, global_token), "incomplete function call, received EOF instead of )\n");
		emit_out("push_eax\t#_process_expression1\n");
		passed = 1;

		while(eq(global_token->s[0], ',')) {
			global_token = global_token->next;
			require(neq(NULL, global_token), "incomplete function call, received EOF instead of argument\n");
			expression();
			emit_out("push_eax\t#_process_expression2\n");
			passed = add(passed, 1);
		}
	}

	require_match("ERROR in process_expression_list\nNo ) was found\n", ")");

	if(eq(TRUE, bool)) {
			emit_out("lea_eax,[ebp+DWORD] %");
			emit_out(s);
			emit_out("\nmov_eax,[eax]\n");
			emit_out("mov_ebp,edi\n");
			emit_out("call_eax\n");
	} else {
			emit_out("mov_ebp,edi\n");
			emit_out("call %FUNCTION_");
			emit_out(s);
			emit_out("\n");
	}

	for(; passed > 0; passed = sub(passed, 1)) {
		emit_out("pop_ebx\t# _process_expression_locals\n");
	}

	emit_out("pop_ebp\t# Restore old base pointer\n");
	emit_out("pop_edi\t# Prevent overwrite\n");
}

int constant_load(struct token_list* a) {
	emit_out("mov_eax, %");
	emit_out(a->arguments->s);
	emit_out("\n");
}

int load_value_signed(int size) {
	if(eq(size, 1)) {
		return "movsx_eax,BYTE_PTR_[eax]\n";
	} else if(eq(size, 2)) {
		return "movsx_eax,WORD_PTR_[eax]\n";
	} else if(eq(size, 4)) {
		return "mov_eax,[eax]\n";
	}
	line_error();
	fputs(" Got unsupported size ", stderr);
	fputs(int2str(size, 10, TRUE), stderr);
	fputs(" when trying to load value.\n", stderr);
	exit(EXIT_FAILURE);
}

int load_value_unsigned(int size) {
	if(eq(size, 1)) {
		return "movzx_eax,BYTE_PTR_[eax]\n";
	} else if(eq(size, 2)) {
		return "movzx_eax,WORD_PTR_[eax]\n";
	} else if(eq(size, 4)) {
		return "mov_eax,[eax]\n";
	}
	fputs(" Got unsupported size ", stderr);
	fputs(int2str(size, 10, TRUE), stderr);
	fputs(" when trying to load value.\n", stderr);
	exit(EXIT_FAILURE);
}

int load_value(int size, int is_signed) {
	if(is_signed) {
		return load_value_signed(size);
	}
	return load_value_unsigned(size);
}

int store_value(int size) {
	if(eq(size, 1)) {
		return "mov_[ebx],al\n";
	} else if(eq(size, 2)) {
		return "mov_[ebx],ax\n";
	} else if(eq(size, 4)) {
		return "mov_[ebx],eax\n";
	}
	/* Should not happen but print error message. */
	fputs("Got unsupported size ", stderr);
	fputs(int2str(size, 10, TRUE), stderr);
	fputs(" when storing number in register.\n", stderr);
	line_error();
	exit(EXIT_FAILURE);
}

int postfix_expr_stub();
int variable_load(struct token_list* a, int num_dereference)
{
	require(neq(NULL, global_token), "incomplete variable load received\n");
	if(and(or(match("FUNCTION", a->type->name), match("FUNCTION*", a->type->name)), match("(", global_token->s))) {
		function_call(int2str(a->depth, 10, TRUE), TRUE);
		return;
	}
	current_target = a->type;

	emit_out("lea_eax,[ebp+DWORD] %");

	emit_out(int2str(a->depth, 10, TRUE));
	emit_out("\n");

	if(eq(0, match("=", global_token->s))) {
		emit_out(load_value(current_target->size, current_target->is_signed));
	}

	while (gt(num_dereference, 0)) {
		current_target = current_target->type;
		emit_out(load_value(current_target->size, current_target->is_signed));
		num_dereference = sub(num_dereference, 1);
	}
}

int function_load(struct token_list* a) {
	require(neq(NULL, global_token), "incomplete function load\n");
	if(match("(", global_token->s)) {
		function_call(a->s, FALSE);
		return;
	}

	emit_out("mov_eax, &FUNCTION_");
	emit_out(a->s);
	emit_out("\n");
}

int global_load(struct token_list* a) {
	current_target = a->type;
	emit_out("mov_eax, &GLOBAL_");
	emit_out(a->s);
	emit_out("\n");

	require(neq(NULL, global_token), "unterminated global load\n");

	if(match("=", global_token->s)) {
		return;
	}

	emit_out(load_value(register_size, current_target->is_signed));
}

/*
 * primary-expr:
 * FAILURE
 * "String"
 * 'Char'
 * [0-9]*
 * [a-z,A-Z]*
 * ( expression )
 */

int primary_expr_failure() {
	require(neq(NULL, global_token), "hit EOF when expecting primary expression\n");
	line_error();
	fputs("Received ", stderr);
	fputs(global_token->s, stderr);
	fputs(" in primary_expr\n", stderr);
	exit(EXIT_FAILURE);
}

int primary_expr_string() {
	char* number_string;
	number_string = int2str(current_count, 10, TRUE);
	current_count = add(current_count, 1);
	emit_out("mov_eax, &STRING_");
	uniqueID_out(function->s, number_string);

	/* The target */
	strings_list = emit(":STRING_", strings_list);
	strings_list = uniqueID(function->s, strings_list, number_string);

	/* catch case of just "foo" from segfaulting */
	require(neq(NULL, global_token->next), "a string by itself is not valid C\n");

	/* Parse the string */
	if(neq('"', global_token->next->s[0])) {
		strings_list = emit(parse_string(global_token->s), strings_list);
		global_token = global_token->next;
	}
}

int primary_expr_char() {
	emit_out("mov_eax, %");
	emit_out(int2str(escape_lookup(add(global_token->s, 1)), 10, TRUE));
	emit_out("\n");
	global_token = global_token->next;
}

int primary_expr_number() {
	emit_out("mov_eax, %");
	emit_out(global_token->s);
	emit_out("\n");
	global_token = global_token->next;
}

void primary_expr_variable()
{
	int num_dereference = 0;
	char* s = global_token->s;
	global_token = global_token->next;
	struct token_list* a = sym_lookup(s, global_constant_list);
	if(NULL != a)
	{
		constant_load(a);
		return;
	}

	a = sym_lookup(s, function->locals);
	if(NULL != a)
	{
		variable_load(a, num_dereference);
		return;
	}

	a = sym_lookup(s, function->arguments);
	if(NULL != a)
	{
		variable_load(a, num_dereference);
		return;
	}

	a = sym_lookup(s, global_function_list);
	if(NULL != a)
	{
		function_load(a);
		return;
	}

	a = sym_lookup(s, global_symbol_list);
	if(NULL != a)
	{
		global_load(a);
		return;
	}

	line_error();
	fputs(s ,stderr);
	fputs(" is not a defined symbol\n", stderr);
	exit(EXIT_FAILURE);
}

void primary_expr();
struct type* promote_type(struct type* a, struct type* b)
{
	require(NULL != b, "impossible case 1 in promote_type\n");
	require(NULL != a, "impossible case 2 in promote_type\n");

	if(a == b) return a;

	struct type* i;
	for(i = global_types; NULL != i; i = i->next)
	{
		if(a->name == i->name) break;
		if(b->name == i->name) break;
		if(a->name == i->indirect->name) break;
		if(b->name == i->indirect->name) break;
		if(a->name == i->indirect->indirect->name) break;
		if(b->name == i->indirect->indirect->name) break;
	}
	require(NULL != i, "impossible case 3 in promote_type\n");
	return i;
}

void common_recursion(FUNCTION f)
{
	emit_out("push_eax\t#_common_recursion\n");

	struct type* last_type = current_target;
	global_token = global_token->next;
	require(NULL != global_token, "Received EOF in common_recursion\n");
	f();
	current_target = promote_type(current_target, last_type);

	emit_out("pop_ebx\t# _common_recursion\n");
}

void general_recursion(FUNCTION f, char* s, char* name, FUNCTION iterate)
{
	require(NULL != global_token, "Received EOF in general_recursion\n");
	if(match(name, global_token->s))
	{
		common_recursion(f);
		emit_out(s);
		iterate();
	}
}

void arithmetic_recursion(FUNCTION f, char* s1, char* s2, char* name, FUNCTION iterate)
{
	require(NULL != global_token, "Received EOF in arithmetic_recursion\n");
	if(match(name, global_token->s))
	{
		common_recursion(f);
		if(NULL == current_target)
		{
			emit_out(s1);
		}
		else if(current_target->is_signed)
		{
			emit_out(s1);
		}
		else
		{
			emit_out(s2);
		}
		iterate();
	}
}


/*
 * postfix-expr:
 *         primary-expr
 *         postfix-expr [ expression ]
 *         postfix-expr ( expression-list-opt )
 *         postfix-expr -> member
 *         postfix-expr . member
 */

void postfix_expr_array()
{
	struct type* array = current_target;
	common_recursion(expression);
	current_target = array;
	require(NULL != current_target, "Arrays only apply to variables\n");

	char* assign = load_value(register_size, current_target->is_signed);

	/* Add support for Ints */
	if(match("char*", current_target->name))
	{
		assign = load_value(1, TRUE);
	}
	else
	{
		emit_out("push_ebx\nmov_ebx, %");
		emit_out(int2str(current_target->type->size, 10, TRUE));
		emit_out("\nmul_ebx\npop_ebx\n");
	}

	emit_out("add_eax,ebx\n");

	require_match("ERROR in postfix_expr\nMissing ]\n", "]");
	require(NULL != global_token, "truncated array expression\n");

	if(match("=", global_token->s) || match(".", global_token->s))
	{
		assign = "";
	}
	if(match("[", global_token->s))
	{
		current_target = current_target->type;
	}

	emit_out(assign);
}

/*
 * unary-expr:
 *         &postfix-expr
 *         - postfix-expr
 *         !postfix-expr
 *         sizeof ( type )
 */
struct type* type_name();
void unary_expr_sizeof()
{
	global_token = global_token->next;
	require(NULL != global_token, "Received EOF when starting sizeof\n");
	require_match("ERROR in unary_expr\nMissing (\n", "(");
	struct type* a = type_name();
	require_match("ERROR in unary_expr\nMissing )\n", ")");

	emit_out("mov_eax, %");
	emit_out(int2str(a->size, 10, TRUE));
	emit_out("\n");
}

int postfix_expr_stub()
{
	require(NULL != global_token, "Unexpected EOF, improperly terminated primary expression\n");
	if(match("[", global_token->s))
	{
		postfix_expr_array();
		postfix_expr_stub();
	}
}

void postfix_expr()
{
	primary_expr();
	postfix_expr_stub();
}

/*
 * additive-expr:
 *         postfix-expr
 *         additive-expr * postfix-expr
 *         additive-expr / postfix-expr
 *         additive-expr % postfix-expr
 *         additive-expr + postfix-expr
 *         additive-expr - postfix-expr
 *         additive-expr << postfix-expr
 *         additive-expr >> postfix-expr
 */
void additive_expr_stub()
{
		arithmetic_recursion(postfix_expr, "add_eax,ebx\n", "add_eax,ebx\n", "+", additive_expr_stub);
		arithmetic_recursion(postfix_expr, "sub_ebx,eax\nmov_eax,ebx\n", "sub_ebx,eax\nmov_eax,ebx\n", "-", additive_expr_stub);
		arithmetic_recursion(postfix_expr, "imul_ebx\n", "mul_ebx\n", "*", additive_expr_stub);
		arithmetic_recursion(postfix_expr, "xchg_ebx,eax\ncdq\nidiv_ebx\n", "xchg_ebx,eax\nmov_edx, %0\ndiv_ebx\n", "/", additive_expr_stub);
		arithmetic_recursion(postfix_expr, "xchg_ebx,eax\ncdq\nidiv_ebx\nmov_eax,edx\n", "xchg_ebx,eax\nmov_edx, %0\ndiv_ebx\nmov_eax,edx\n", "%", additive_expr_stub);
		arithmetic_recursion(postfix_expr, "mov_ecx,eax\nmov_eax,ebx\nsal_eax,cl\n", "mov_ecx,eax\nmov_eax,ebx\nshl_eax,cl\n", "<<", additive_expr_stub);
		arithmetic_recursion(postfix_expr, "mov_ecx,eax\nmov_eax,ebx\nsar_eax,cl\n", "mov_ecx,eax\nmov_eax,ebx\nshr_eax,cl\n", ">>", additive_expr_stub);
}


void additive_expr()
{
	postfix_expr();
	additive_expr_stub();
}


/*
 * relational-expr:
 *         additive_expr
 *         relational-expr < additive_expr
 *         relational-expr <= additive_expr
 *         relational-expr >= additive_expr
 *         relational-expr > additive_expr
 */

void relational_expr_stub()
{
		arithmetic_recursion(additive_expr, "cmp\nsetl_al\nmovzx_eax,al\n", "cmp\nsetb_al\nmovzx_eax,al\n", "<", relational_expr_stub);
		arithmetic_recursion(additive_expr, "cmp\nsetle_al\nmovzx_eax,al\n", "cmp\nsetbe_al\nmovzx_eax,al\n", "<=", relational_expr_stub);
		arithmetic_recursion(additive_expr, "cmp\nsetge_al\nmovzx_eax,al\n", "cmp\nsetae_al\nmovzx_eax,al\n", ">=", relational_expr_stub);
		arithmetic_recursion(additive_expr, "cmp\nsetg_al\nmovzx_eax,al\n", "cmp\nseta_al\nmovzx_eax,al\n", ">", relational_expr_stub);
		general_recursion(additive_expr, "cmp\nsete_al\nmovzx_eax,al\n", "==", relational_expr_stub);
		general_recursion(additive_expr, "cmp\nsetne_al\nmovzx_eax,al\n", "!=", relational_expr_stub);
}

void relational_expr()
{
	additive_expr();
	relational_expr_stub();
}

/*
 * bitwise-expr:
 *         relational-expr
 *         bitwise-expr & bitwise-expr
 *         bitwise-expr && bitwise-expr
 *         bitwise-expr | bitwise-expr
 *         bitwise-expr || bitwise-expr
 *         bitwise-expr ^ bitwise-expr
 */
void bitwise_expr_stub()
{
		general_recursion(relational_expr, "and_eax,ebx\n", "&", bitwise_expr_stub);
		general_recursion(relational_expr, "and_eax,ebx\n", "&&", bitwise_expr_stub);
		general_recursion(relational_expr, "or_eax,ebx\n", "|", bitwise_expr_stub);
		general_recursion(relational_expr, "or_eax,ebx\n", "||", bitwise_expr_stub);
		general_recursion(relational_expr, "xor_eax,ebx\n", "^", bitwise_expr_stub);
}


void bitwise_expr()
{
	relational_expr();
	bitwise_expr_stub();
}

/*
 * expression:
 *         bitwise-or-expr
 *         bitwise-or-expr = expression
 */

void primary_expr()
{
	require(NULL != global_token, "Received EOF where primary expression expected\n");
	Address_of = FALSE;

	if(match("sizeof", global_token->s)) unary_expr_sizeof();
	else if('-' == global_token->s[0])
	{
		emit_out("mov_eax, %0\n");

		common_recursion(primary_expr);

		emit_out("sub_ebx,eax\nmov_eax,ebx\n");
	}
	else if('!' == global_token->s[0])
	{
		emit_out("mov_eax, %1\n");

		common_recursion(postfix_expr);

		emit_out("cmp\nseta_al\nmovzx_eax,al\n");
	}
	else if('~' == global_token->s[0])
	{
		common_recursion(postfix_expr);

		emit_out("not_eax\n");
	}
	else if(global_token->s[0] == '(')
	{
		global_token = global_token->next;
		expression();
		require_match("Error in Primary expression\nDidn't get )\n", ")");
	}
	else if(global_token->s[0] == '\'') primary_expr_char();
	else if(global_token->s[0] == '"') primary_expr_string();
	else if(in_set(global_token->s[0], "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")) primary_expr_variable();
	else if(global_token->s[0] == '*') primary_expr_variable();
	else if(in_set(global_token->s[0], "0123456789")) primary_expr_number();
	else primary_expr_failure();
}

int expression()
{
	bitwise_expr();
	if(match("=", global_token->s))
	{
		char* store = "";
		if(match("]", global_token->prev->s))
		{
			store = store_value(current_target->type->size);
		}
		else
		{
			store = store_value(current_target->size);
		}

		common_recursion(expression);
		emit_out(store);
		current_target = integer;
	}
}

/* Similar to integer division a / b but rounds up */
unsigned ceil_div(unsigned a, unsigned b)
{
    return (a + b - 1) / b;
}

/* Process local variable */
void collect_local()
{
	struct type* type_size = type_name();
	require(NULL != global_token, "Received EOF while collecting locals\n");
	require(!in_set(global_token->s[0], "[{(<=>)}]|&!^%;:'\""), "forbidden character in local variable name\n");
	require(NULL != type_size, "Must have non-null type\n");
	struct token_list* a = sym_declare(global_token->s, type_size, function->locals);
	if(match("main", function->s) && (NULL == function->locals))
	{
		a->depth = -20;
	}
	else if((NULL == function->arguments) && (NULL == function->locals))
	{
		a->depth = -8;
	}
	else if(NULL == function->locals)
	{
		a->depth = function->arguments->depth - 8;
	}
	else
	{
		a->depth = function->locals->depth - register_size;
	}

	/* Adjust the depth of local structs. When stack grows downwards, we want them to 
	   start at the bottom of allocated space. */
	unsigned struct_depth_adjustment = (ceil_div(a->type->size, register_size) - 1) * register_size;
	a->depth = a->depth - struct_depth_adjustment;

	function->locals = a;

	emit_out("# Defining local ");
	emit_out(global_token->s);
	emit_out("\n");

	global_token = global_token->next;
	require(NULL != global_token, "incomplete local missing name\n");

	if(match("=", global_token->s))
	{
		global_token = global_token->next;
		require(NULL != global_token, "incomplete local assignment\n");
		expression();
	}

	require_match("ERROR in collect_local\nMissing ;\n", ";");

	unsigned i = (a->type->size + register_size - 1) / register_size;
	while(i != 0)
	{
		emit_out("push_eax\t#");
		emit_out(a->s);
		emit_out("\n");
		i = i - 1;
	}
}

void statement();

/* Evaluate if statements */
void process_if()
{
	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	emit_out("# IF_");
	uniqueID_out(function->s, number_string);

	global_token = global_token->next;
	require_match("ERROR in process_if\nMISSING (\n", "(");
	expression();

	emit_out("test_eax,eax\nje %ELSE_");

	uniqueID_out(function->s, number_string);

	require_match("ERROR in process_if\nMISSING )\n", ")");
	statement();
	require(NULL != global_token, "Reached EOF inside of function\n");

	emit_out("jmp %_END_IF_");

	uniqueID_out(function->s, number_string);

	emit_out(":ELSE_");
	uniqueID_out(function->s, number_string);

	if(match("else", global_token->s))
	{
		global_token = global_token->next;
		require(NULL != global_token, "Received EOF where an else statement expected\n");
		statement();
		require(NULL != global_token, "Reached EOF inside of function\n");
	}
	emit_out(":_END_IF_");
	uniqueID_out(function->s, number_string);
}

void process_for()
{
	struct token_list* nested_locals = break_frame;
	char* nested_break_head = break_target_head;
	char* nested_break_func = break_target_func;
	char* nested_break_num = break_target_num;

	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	break_target_head = "FOR_END_";
	break_target_num = number_string;
	break_frame = function->locals;
	break_target_func = function->s;

	emit_out("# FOR_initialization_");
	uniqueID_out(function->s, number_string);

	global_token = global_token->next;

	require_match("ERROR in process_for\nMISSING (\n", "(");
	if(!match(";",global_token->s))
	{
		expression();
	}

	emit_out(":FOR_");
	uniqueID_out(function->s, number_string);

	require_match("ERROR in process_for\nMISSING ;1\n", ";");
	expression();

	emit_out("test_eax,eax\nje %FOR_END_");

	uniqueID_out(function->s, number_string);

	emit_out("jmp %FOR_THEN_");

	uniqueID_out(function->s, number_string);

	emit_out(":FOR_ITER_");
	uniqueID_out(function->s, number_string);

	require_match("ERROR in process_for\nMISSING ;2\n", ";");
	expression();

	emit_out("jmp %FOR_");

	uniqueID_out(function->s, number_string);

	emit_out(":FOR_THEN_");
	uniqueID_out(function->s, number_string);

	require_match("ERROR in process_for\nMISSING )\n", ")");
	statement();
	require(NULL != global_token, "Reached EOF inside of function\n");

	emit_out("jmp %FOR_ITER_");

	uniqueID_out(function->s, number_string);

	emit_out(":FOR_END_");
	uniqueID_out(function->s, number_string);

	break_target_head = nested_break_head;
	break_target_func = nested_break_func;
	break_target_num = nested_break_num;
	break_frame = nested_locals;
}

/* Process Assembly statements */
void process_asm()
{
	global_token = global_token->next;
	require_match("ERROR in process_asm\nMISSING (\n", "(");
	while('"' == global_token->s[0])
	{
		emit_out((global_token->s + 1));
		emit_out("\n");
		global_token = global_token->next;
		require(NULL != global_token, "Received EOF inside asm statement\n");
	}
	require_match("ERROR in process_asm\nMISSING )\n", ")");
	require_match("ERROR in process_asm\nMISSING ;\n", ";");
}

/* Process do while loops */
void process_do()
{
	struct token_list* nested_locals = break_frame;
	char* nested_break_head = break_target_head;
	char* nested_break_func = break_target_func;
	char* nested_break_num = break_target_num;

	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	break_target_head = "DO_END_";
	break_target_num = number_string;
	break_frame = function->locals;
	break_target_func = function->s;

	emit_out(":DO_");
	uniqueID_out(function->s, number_string);

	global_token = global_token->next;
	require(NULL != global_token, "Received EOF where do statement is expected\n");
	statement();
	require(NULL != global_token, "Reached EOF inside of function\n");

	emit_out(":DO_TEST_");
	uniqueID_out(function->s, number_string);

	require_match("ERROR in process_do\nMISSING while\n", "while");
	require_match("ERROR in process_do\nMISSING (\n", "(");
	expression();
	require_match("ERROR in process_do\nMISSING )\n", ")");
	require_match("ERROR in process_do\nMISSING ;\n", ";");

	emit_out("test_eax,eax\njne %DO_");

	uniqueID_out(function->s, number_string);

	emit_out(":DO_END_");
	uniqueID_out(function->s, number_string);

	break_frame = nested_locals;
	break_target_head = nested_break_head;
	break_target_func = nested_break_func;
	break_target_num = nested_break_num;
}


/* Process while loops */
void process_while()
{
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
	require_match("ERROR in process_while\nMISSING (\n", "(");
	expression();

	emit_out("test_eax,eax\nje %END_WHILE_");

	uniqueID_out(function->s, number_string);

	emit_out("# THEN_while_");
	uniqueID_out(function->s, number_string);

	require_match("ERROR in process_while\nMISSING )\n", ")");
	statement();
	require(NULL != global_token, "Reached EOF inside of function\n");

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
void return_result()
{
	global_token = global_token->next;
	require(NULL != global_token, "Incomplete return statement received\n");
	if(global_token->s[0] != ';') expression();

	require_match("ERROR in return_result\nMISSING ;\n", ";");

	struct token_list* i;
	unsigned size_local_var;
	for(i = function->locals; NULL != i; i = i->next)
	{
		size_local_var = ceil_div(i->type->size, register_size);
		while(size_local_var != 0)
		{
			emit_out("pop_ebx\t# _return_result_locals\n");
			size_local_var = size_local_var - 1;
		}
	}

	emit_out("ret\n");
}

void process_break()
{
	if(NULL == break_target_head)
	{
		line_error();
		fputs("Not inside of a loop or case statement\n", stderr);
		exit(EXIT_FAILURE);
	}
	struct token_list* i = function->locals;
	while(i != break_frame)
	{
		if(NULL == i) break;
		emit_out("pop_ebx\t# break_cleanup_locals\n");
		i = i->next;
	}
	global_token = global_token->next;

	emit_out("jmp %");

	emit_out(break_target_head);
	emit_out(break_target_func);
	emit_out("_");
	emit_out(break_target_num);
	emit_out("\n");
	require_match("ERROR in break statement\nMissing ;\n", ";");
}

void recursive_statement()
{
	global_token = global_token->next;
	require(NULL != global_token, "Received EOF in recursive statement\n");
	struct token_list* frame = function->locals;

	while(!match("}", global_token->s))
	{
		statement();
		require(NULL != global_token, "Received EOF in recursive statement prior to }\n");
	}
	global_token = global_token->next;

	/* Clean up any locals added */

	if( !match("ret\n", output_list->s))
	{
		struct token_list* i;
		for(i = function->locals; frame != i; i = i->next)
		{
			emit_out( "pop_ebx\t# _recursive_statement_locals\n");
		}
	}
	function->locals = frame;
}

/*
 * statement:
 *     { statement-list-opt }
 *     type-name identifier ;
 *     type-name identifier = expression;
 *     if ( expression ) statement
 *     if ( expression ) statement else statement
 *     do statement while ( expression ) ;
 *     while ( expression ) statement
 *     for ( expression ; expression ; expression ) statement
 *     asm ( "assembly" ... "assembly" ) ;
 *     goto label ;
 *     label:
 *     return ;
 *     break ;
 *     expr ;
 */

void statement()
{
	require(NULL != global_token, "expected a C statement but received EOF\n");
	/* Always an integer until told otherwise */
	current_target = integer;

	if(global_token->s[0] == '{')
	{
		recursive_statement();
	}
	else if((NULL != lookup_type(global_token->s, prim_types)) ||
	          match("struct", global_token->s))
	{
		collect_local();
	}
	else if(match("if", global_token->s))
	{
		process_if();
	}
	else if(match("do", global_token->s))
	{
		process_do();
	}
	else if(match("while", global_token->s))
	{
		process_while();
	}
	else if(match("for", global_token->s))
	{
		process_for();
	}
	else if(match("asm", global_token->s))
	{
		process_asm();
	}
	else if(match("return", global_token->s))
	{
		return_result();
	}
	else if(match("break", global_token->s))
	{
		process_break();
	}
	else
	{
		expression();
		require_match("ERROR in statement\nMISSING ;\n", ";");
	}
}

/* Collect function arguments */
void collect_arguments()
{
	global_token = global_token->next;
	require(NULL != global_token, "Received EOF when attempting to collect arguments\n");
	struct type* type_size;
	struct token_list* a;

	while(!match(")", global_token->s))
	{
		type_size = type_name();
		require(NULL != global_token, "Received EOF when attempting to collect arguments\n");
		require(NULL != type_size, "Must have non-null type\n");
		if(global_token->s[0] == ')')
		{
			/* foo(int,char,void) doesn't need anything done */
			continue;
		}
		else if(global_token->s[0] != ',')
		{
			/* deal with foo(int a, char b) */
			require(!in_set(global_token->s[0], "[{(<=>)}]|&!^%;:'\""), "forbidden character in argument variable name\n");
			a = sym_declare(global_token->s, type_size, function->arguments);
			if(NULL == function->arguments)
			{
				a->depth = -4;
			}
			else
			{
				a->depth = function->arguments->depth - register_size;
			}

			global_token = global_token->next;
			require(NULL != global_token, "Incomplete argument list\n");
			function->arguments = a;
		}

		/* ignore trailing comma (needed for foo(bar(), 1); expressions*/
		if(global_token->s[0] == ',')
		{
			global_token = global_token->next;
			require(NULL != global_token, "naked comma in collect arguments\n");
		}

		require(NULL != global_token, "Argument list never completed\n");
	}
	global_token = global_token->next;
}

void declare_function()
{
	current_count = 0;
	function = sym_declare(global_token->prev->s, NULL, global_function_list);

	/* allow previously defined functions to be looked up */
	global_function_list = function;
	collect_arguments();

	require(NULL != global_token, "Function definitions either need to be prototypes or full\n");
	/* If just a prototype don't waste time */
	if(global_token->s[0] == ';') global_token = global_token->next;
	else
	{
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

void global_constant()
{
	global_token = global_token->next;
	require(NULL != global_token, "CONSTANT lacks a name\n");
	global_constant_list = sym_declare(global_token->s, NULL, global_constant_list);

	require(NULL != global_token->next, "CONSTANT lacks a value\n");
	global_constant_list->arguments = global_token->next;
	global_token = global_token->next->next;
}

/*
 * program:
 *     declaration
 *     declaration program
 *
 * declaration:
 *     CONSTANT identifer value
 *     typedef identifer type;
 *     type-name identifier ;
 *     type-name identifier = value ;
 *     type-name identifier [ value ];
 *     type-name identifier ( parameter-list ) ;
 *     type-name identifier ( parameter-list ) statement
 *
 * parameter-list:
 *     parameter-declaration
 *     parameter-list, parameter-declaration
 *
 * parameter-declaration:
 *     type-name identifier-opt
 */
void program()
{
	int i;
	struct type* type_size;

	function = NULL;
	Address_of = FALSE;

new_type:
	/* Deal with garbage input */
	if (NULL == global_token) return;
	require('#' != global_token->s[0], "unhandled macro directive\n");
	require(!match("\n", global_token->s), "unexpected newline token\n");

	/* Handle cc_* CONSTANT statements */
	if(match("CONSTANT", global_token->s))
	{
		global_constant();
		goto new_type;
	}

	type_size = type_name();
	/* Deal with case of struct definitions */
	if(NULL == type_size) goto new_type;

	require(NULL != global_token->next, "Unterminated global\n");

	/* Add to global symbol table */
	global_symbol_list = sym_declare(global_token->s, type_size, global_symbol_list);
	global_token = global_token->next;

	/* Deal with global variables */
	if(match(";", global_token->s))
	{
		/* Ensure enough bytes are allocated to store global variable.
		   In some cases it allocates too much but that is harmless. */
		globals_list = emit(":GLOBAL_", globals_list);
		globals_list = emit(global_token->prev->s, globals_list);

		/* round up division */
		i = ceil_div(type_size->size, register_size);
		globals_list = emit("\n", globals_list);
		while(i != 0)
		{
			globals_list = emit("NULL\n", globals_list);
			i = i - 1;
		}
		global_token = global_token->next;
		goto new_type;
	}

	/* Deal with global functions */
	if(match("(", global_token->s))
	{
		declare_function();
		goto new_type;
	}

	/* Everything else is just an error */
	line_error();
	fputs("Received ", stderr);
	fputs(global_token->s, stderr);
	fputs(" in program\n", stderr);
	exit(EXIT_FAILURE);
}

void recursive_output(struct token_list* head, int out)
{
	struct token_list* i = reverse_list(head);
	while(NULL != i)
	{
		fputs(i->s, out);
		i = i->next;
	}
}

void eat_current_token()
{
	int update_global_token = FALSE;
	if (macro_token == global_token)
		update_global_token = TRUE;

	macro_token = eat_token(macro_token);

	if(update_global_token)
		global_token = macro_token;
}

void eat_newline_tokens()
{
	macro_token = global_token;

	while(TRUE)
	{
		if(NULL == macro_token) return;

		if(match("\n", macro_token->s))
		{
			eat_current_token();
		}
		else
		{
			macro_token = macro_token->next;
		}
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
}

int main(int argc, char** argv)
{
	char* name;
	char* hold;
	char* val;
	int i;
	int in;
	int destination_file;

        initialize_globals();

	i = 1;
	hold_string = calloc(MAX_STRING + 4, sizeof(char));

	name = argv[i];

	in = fopen(name, "r");
	global_token = read_all_tokens(in, global_token, name);
	fclose(in);
	i = i + 1;

	destination_file = fopen(argv[i], "w");
	i = i + 1;

	global_token = reverse_list(global_token);

	global_token = remove_line_comment_tokens(global_token);
	global_token = remove_preprocessor_directives(global_token);

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

exit_success:
	fclose(destination_file);
	return EXIT_SUCCESS;
}
