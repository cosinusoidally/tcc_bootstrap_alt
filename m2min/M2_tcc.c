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

#define X86 3

void copy_string(char* target, char* source, int max);
int in_set(int c, char* s);
int match(char* a, char* b);
void require(int bool, char* error);
void reset_hold_string();

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
void initialize_types();
struct token_list* read_all_tokens(FILE* a, struct token_list* current, char* filename);
struct token_list* reverse_list(struct token_list* head);

struct token_list* remove_line_comment_tokens(struct token_list* head);
struct token_list* remove_preprocessor_directives(struct token_list* head);

void eat_newline_tokens();
void init_macro_env(char* sym, char* value, char* source, int num);
void preprocess();
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
int Architecture;
int register_size;

int MAX_STRING;
struct type* integer;

/* enable bootstrap-mode */
int BOOTSTRAP_MODE;



/* Globals */
FILE* input;
struct token_list* token;
int line;
char* file;

void require(int bool, char* error);
void line_error_token(struct token_list* list);
struct token_list* eat_token(struct token_list* head);

struct conditional_inclusion
{
	struct conditional_inclusion* prev;
	int include; /* 1 == include, 0 == skip */
	int previous_condition_matched; /* 1 == all subsequent conditions treated as FALSE */
};

struct macro_list
{
	struct macro_list* next;
	char* symbol;
	struct token_list* expansion;
};

struct macro_list* macro_env;
struct conditional_inclusion* conditional_inclusion_top;

/* point where we are currently modifying the global_token list */
struct token_list* macro_token;


void require(int bool, char* error)
{
	if(!bool)
	{
		fputs(error, stderr);
		exit(EXIT_FAILURE);
	}
}


int match(char* a, char* b)
{
	if((NULL == a) && (NULL == b)) return TRUE;
	if(NULL == a) return FALSE;
	if(NULL == b) return FALSE;

	int i = -1;
	do
	{
		i = i + 1;
		if(a[i] != b[i])
		{
			return FALSE;
		}
	} while((0 != a[i]) && (0 !=b[i]));
	return TRUE;
}


int in_set(int c, char* s)
{
	/* NULL set is always false */
	if(NULL == s) return FALSE;

	while(0 != s[0])
	{
		if(c == s[0]) return TRUE;
		s = s + 1;
	}
	return FALSE;
}

char* int2str(int x, int base, int signed_p)
{
	require(1 < base, "int2str doesn't support a base less than 2\n");
	require(37 > base, "int2str doesn't support a base more than 36\n");
	/* Be overly conservative and save space for 32binary digits and padding null */
	char* p = calloc(34, sizeof(char));
	/* if calloc fails return null to let calling code deal with it */
	if(NULL == p) return p;

	p = p + 32;
	unsigned i;
	int sign_p = FALSE;
	char* table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if(signed_p && (10 == base) && (0 != (x & 0x80000000)))
	{
		/* Truncate to 31bits */
		i = -x & 0x7FFFFFFF;
		if(0 == i) return "-2147483648";
		sign_p = TRUE;
	} /* Truncate to 32bits */
	else i = x & (0x7FFFFFFF | (1 << 31));

	do
	{
		p[0] = table[i % base];
		p = p - 1;
		i = i / base;
	} while(0 < i);

	if(sign_p)
	{
		p[0] = '-';
		p = p - 1;
	}

	return p + 1;
}

int grab_byte()
{
	int c = fgetc(input);
	if(10 == c) line = line + 1;
	return c;
}

int clearWhiteSpace(int c)
{
	if((32 == c) || (9 == c)) return clearWhiteSpace(grab_byte());
	return c;
}

int consume_byte(int c)
{
	hold_string[string_index] = c;
	string_index = string_index + 1;
	require(MAX_STRING > string_index, "Token exceeded MAX_STRING char limit\nuse --max-string number to increase\n");
	return grab_byte();
}

int preserve_string(int c)
{
	int frequent = c;
	int escape = FALSE;
	do
	{
		if(!escape && '\\' == c ) escape = TRUE;
		else escape = FALSE;
		c = consume_byte(c);
		require(EOF != c, "Unterminated string\n");
	} while(escape || (c != frequent));
	return grab_byte();
}


void copy_string(char* target, char* source, int max)
{
	int i = 0;
	while(0 != source[i])
	{
		target[i] = source[i];
		i = i + 1;
		if(i == max) break;
	}
}

int preserve_keyword(int c, char* S)
{
	while(in_set(c, S))
	{
		c = consume_byte(c);
	}
	return c;
}

void reset_hold_string()
{
	int i = MAX_STRING;
	while(0 <= i)
	{
		hold_string[i] = 0;
		i = i - 1;
	}
	string_index = 0;
}

/* note if this is the first token in the list, head needs fixing up */
struct token_list* eat_token(struct token_list* token)
{
	if(NULL != token->prev)
	{
		token->prev->next = token->next;
	}

	/* update backlinks */
	if(NULL != token->next)
	{
		token->next->prev = token->prev;
	}

	return token->next;
}

struct token_list* eat_until_newline(struct token_list* head)
{
	while (NULL != head)
	{
		if('\n' == head->s[0])
		{
			return head;
		}
		else
		{
			head = eat_token(head);
		}
	}

	return NULL;
}

struct token_list* remove_line_comment_tokens(struct token_list* head)
{
	struct token_list* first = NULL;

	while (NULL != head)
	{
		if(match("//", head->s))
		{
			head = eat_token(head);
		}
		else
		{
			if(NULL == first)
			{
				first = head;
			}
			head = head->next;
		}
	}

	return first;
}

struct token_list* remove_preprocessor_directives(struct token_list* head)
{
	struct token_list* first = NULL;

	while (NULL != head)
	{
		if('#' == head->s[0])
		{
			head = eat_until_newline(head);
		}
		else
		{
			if(NULL == first)
			{
				first = head;
			}
			head = head->next;
		}
	}

	return first;
}

void new_token(char* s, int size)
{
	struct token_list* current = calloc(1, sizeof(struct token_list));
	require(NULL != current, "Exhausted memory while getting token\n");

	/* More efficiently allocate memory for string */
	current->s = calloc(size, sizeof(char));
	require(NULL != current->s, "Exhausted memory while trying to copy a token\n");
	copy_string(current->s, s, MAX_STRING);

	current->prev = token;
	current->next = token;
	current->linenumber = line;
	current->filename = file;
	token = current;
}

int get_token(int c)
{
	struct token_list* current = calloc(1, sizeof(struct token_list));
	require(NULL != current, "Exhausted memory while getting token\n");

reset:
	reset_hold_string();
	string_index = 0;

	c = clearWhiteSpace(c);
	if(c == EOF)
	{
		free(current);
		return c;
	}
	else if('#' == c)
	{
		c = consume_byte(c);
		c = preserve_keyword(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
	}
	else if(in_set(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"))
	{
		c = preserve_keyword(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
	}
	else if(in_set(c, "<=>|&!^%"))
	{
		c = preserve_keyword(c, "<=>|&!^%");
	}
	else if(in_set(c, "'\""))
	{
		c = preserve_string(c);
	}
	else if(c == '/')
	{
		c = consume_byte(c);
		if(c == '*')
		{
			c = grab_byte();
			while(c != '/')
			{
				while(c != '*')
				{
					c = grab_byte();
					require(EOF != c, "Hit EOF inside of block comment\n");
				}
				c = grab_byte();
				require(EOF != c, "Hit EOF inside of block comment\n");
			}
			c = grab_byte();
			goto reset;
		}
		else if(c == '/')
		{
			c = consume_byte(c);
		}
		else if(c == '=')
		{
			c = consume_byte(c);
		}
	}
	else if (c == '\n')
	{
		c = consume_byte(c);
	}
	else if(c == '*')
	{
		c = consume_byte(c);
		if(c == '=')
		{
			c = consume_byte(c);
		}
	}
	else if(c == '+')
	{
		c = consume_byte(c);
		if(c == '=')
		{
			c = consume_byte(c);
		}
		if(c == '+')
		{
			c = consume_byte(c);
		}
	}
	else if(c == '-')
	{
		c = consume_byte(c);
		if(c == '=')
		{
			c = consume_byte(c);
		}
		if(c == '>')
		{
			c = consume_byte(c);
		}
		if(c == '-')
		{
			c = consume_byte(c);
		}
	}
	else
	{
		c = consume_byte(c);
	}

	new_token(hold_string, string_index + 2);
	return c;
}

struct token_list* reverse_list(struct token_list* head)
{
	struct token_list* root = NULL;
	struct token_list* next;
	while(NULL != head)
	{
		next = head->next;
		head->next = root;
		root = head;
		head = next;
	}
	return root;
}

struct token_list* read_all_tokens(FILE* a, struct token_list* current, char* filename)
{
	input  = a;
	line = 1;
	file = filename;
	token = current;
	int ch = grab_byte();
	while(EOF != ch)
	{
		ch = get_token(ch);
		require(NULL != token, "Empty files don't need to be compiled\n");
	}

	return token;
}

struct token_list* emit(char *s, struct token_list* head);
void require(int bool, char* error);

int escape_lookup(char* c);

/* Lookup escape values */
int escape_lookup(char* c)
{
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
char* collect_regular_string(char* string)
{
	string_index = 0;

collect_regular_string_reset:
	require((MAX_STRING - 3) > string_index, "Attempt at parsing regular string exceeds max length\n");
	if(string[0] == '\\')
	{
		hold_string[string_index] = escape_lookup(string);
		if (string[1] == 'x') string = string + 2;
		string = string + 2;
	}
	else
	{
		hold_string[string_index] = string[0];
		string = string + 1;
	}

	string_index = string_index + 1;
	if(string[0] != 0) goto collect_regular_string_reset;

	hold_string[string_index] = '"';
	hold_string[string_index + 1] = '\n';
	char* message = calloc(string_index + 3, sizeof(char));
	require(NULL != message, "Exhausted memory while storing regular string\n");
	copy_string(message, hold_string, string_index + 2);
	reset_hold_string();
	return message;
}

/* Parse string to deal with hex characters*/
char* parse_string(char* string)
{
	/* the string */
	return collect_regular_string(string);
}

/* Imported functions */
void line_error();
void require(int bool, char* error);

/* enable easy primitive extension */
struct type* add_primitive(struct type* a)
{
	if(NULL == prim_types) return a;
	struct type* i = prim_types;
	while(NULL != i->next)
	{
		i = i->next;
	}
	i->next = a;

	return prim_types;
}

/* enable easy primitive creation */
struct type* new_primitive(char* name0, char* name1, char* name2, int size, int sign)
{
	/* Create type** */
	struct type* a = calloc(1, sizeof(struct type));
	require(NULL != a, "Exhausted memory while declaring new primitive**\n");
	a->name = name2;
	a->size = register_size;
	a->indirect = a;
	a->is_signed = sign;

	/* Create type* */
	struct type* b = calloc(1, sizeof(struct type));
	require(NULL != b, "Exhausted memory while declaring new primitive*\n");
	b->name = name1;
	b->size = register_size;
	b->is_signed = sign;
	b->indirect = a;
	a->type = b;

	struct type* r = calloc(1, sizeof(struct type));
	require(NULL != r, "Exhausted memory while declaring new primitive\n");
	r->name = name0;
	r->size = size;
	r->is_signed = sign;
	r->indirect = b;
	r->type = r;
	b->type = r;

	return r;
}

/* Initialize default types */
void initialize_types()
{
	register_size = 4;

	/* Define void */
	struct type* hold = new_primitive("void", "void*", "void**", register_size, FALSE);
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

	if(BOOTSTRAP_MODE)
	{
		/* Define FILE */
		hold = new_primitive("FILE", "FILE*", "FILE**", register_size, TRUE);
		prim_types = add_primitive(hold);

		/* Primitives mes.c wanted */
		hold = new_primitive("size_t", "size_t*", "size_t**", register_size, FALSE);
		prim_types = add_primitive(hold);

		hold = new_primitive("ssize_t", "ssize_t*", "ssize_t**", register_size, FALSE);
		prim_types = add_primitive(hold);
	}

	global_types = prim_types;
}

struct type* lookup_type(char* s, struct type* start)
{
	struct type* i;
	for(i = start; NULL != i; i = i->next)
	{
		if(match(i->name, s))
		{
			return i;
		}
	}
	return NULL;
}

struct type* lookup_member(struct type* parent, char* name)
{
puts("lookup_member not impl");exit(1);
}

struct type* type_name();
void require_match(char* message, char* required);

int member_size;
struct type* build_member(struct type* last, int offset)
{
puts("build_member not impl");exit(1);
}

struct type* build_union(struct type* last, int offset)
{
puts("build_union not impl");exit(1);
}

void create_struct()
{
puts("create_struct not impl");exit(1);
}


struct type* type_name()
{
	struct type* ret;

	require(NULL != global_token, "Received EOF instead of type name\n");

	if(match("extern", global_token->s))
	{
		global_token = global_token->next;
		require(NULL != global_token, "unfinished type definition in extern\n");
	}

	if(match("struct", global_token->s))
	{
		global_token = global_token->next;
		require(NULL != global_token, "structs can not have a EOF type name\n");
		ret = lookup_type(global_token->s, global_types);
		if(NULL == ret)
		{
			create_struct();
			return NULL;
		}
	}
	else
	{
		ret = lookup_type(global_token->s, global_types);
		if(NULL == ret)
		{
			fputs("Unknown type ", stderr);
			fputs(global_token->s, stderr);
			fputs("\n", stderr);
			line_error();
			fputs("\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	global_token = global_token->next;
	require(NULL != global_token, "unfinished type definition\n");

	if(match("const", global_token->s))
	{
		global_token = global_token->next;
		require(NULL != global_token, "unfinished type definition in const\n");
	}

	while(global_token->s[0] == '*')
	{
		ret = ret->indirect;
		global_token = global_token->next;
		require(NULL != global_token, "unfinished type definition in indirection\n");
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
char* continue_target_head;
struct token_list* break_frame;
int current_count;
int Address_of;

/* Imported functions */
char* int2str(int x, int base, int signed_p);
char* parse_string(char* string);
int escape_lookup(char* c);
void require(int bool, char* error);
struct token_list* reverse_list(struct token_list* head);
struct type* mirror_type(struct type* source, char* name);
struct type* add_primitive(struct type* a);

struct token_list* emit(char *s, struct token_list* head)
{
	struct token_list* t = calloc(1, sizeof(struct token_list));
	require(NULL != t, "Exhausted memory while generating token to emit\n");
	t->next = head;
	t->s = s;
	return t;
}

void emit_out(char* s)
{
	output_list = emit(s, output_list);
}

struct token_list* uniqueID(char* s, struct token_list* l, char* num)
{
	l = emit("\n", emit(num, emit("_", emit(s, l))));
	return l;
}

void uniqueID_out(char* s, char* num)
{
	output_list = uniqueID(s, output_list, num);
}

struct token_list* sym_declare(char *s, struct type* t, struct token_list* list)
{
	struct token_list* a = calloc(1, sizeof(struct token_list));
	require(NULL != a, "Exhausted memory while attempting to declare a symbol\n");
	a->next = list;
	a->s = s;
	a->type = t;
	return a;
}

struct token_list* sym_lookup(char *s, struct token_list* symbol_list)
{
	struct token_list* i;
	for(i = symbol_list; NULL != i; i = i->next)
	{
		if(match(i->s, s)) return i;
	}
	return NULL;
}

void line_error_token(struct token_list *token)
{
	if(NULL == token)
	{
		fputs("EOF reached inside of line_error\n", stderr);
		fputs("problem at end of file\n", stderr);
		return;
	}
	fputs(token->filename, stderr);
	fputs(":", stderr);
	fputs(int2str(token->linenumber, 10, TRUE), stderr);
	fputs(":", stderr);
}

void line_error()
{
	line_error_token(global_token);
}

void require_match(char* message, char* required)
{
	if(NULL == global_token)
	{
		line_error();
		fputs("EOF reached inside of require match\n", stderr);
		fputs("problem at end of file\n", stderr);
		fputs(message, stderr);
		exit(EXIT_FAILURE);
	}
	if(!match(global_token->s, required))
	{
		line_error();
		fputs(message, stderr);
		exit(EXIT_FAILURE);
	}
	global_token = global_token->next;
}

void maybe_bootstrap_error(char* feature)
{
	line_error();
	fputs(feature, stderr);
	fputs(" is not supported in --bootstrap-mode\n", stderr);
	exit(EXIT_FAILURE);
}

void expression();
void function_call(char* s, int bool)
{
	require_match("ERROR in process_expression_list\nNo ( was found\n", "(");
	require(NULL != global_token, "Improper function call\n");
	int passed = 0;

		emit_out("push_edi\t# Prevent overwriting in recursion\n");
		emit_out("push_ebp\t# Protect the old base pointer\n");
		emit_out("mov_edi,esp\t# Copy new base pointer\n");

	if(global_token->s[0] != ')')
	{
		expression();
		require(NULL != global_token, "incomplete function call, received EOF instead of )\n");
		emit_out("push_eax\t#_process_expression1\n");
		passed = 1;

		while(global_token->s[0] == ',')
		{
			global_token = global_token->next;
			require(NULL != global_token, "incomplete function call, received EOF instead of argument\n");
			expression();
			emit_out("push_eax\t#_process_expression2\n");
			passed = passed + 1;
		}
	}

	require_match("ERROR in process_expression_list\nNo ) was found\n", ")");

	if(TRUE == bool)
	{
			emit_out("lea_eax,[ebp+DWORD] %");
			emit_out(s);
			emit_out("\nmov_eax,[eax]\n");
			emit_out("mov_ebp,edi\n");
			emit_out("call_eax\n");
	}
	else
	{
			emit_out("mov_ebp,edi\n");
			emit_out("call %FUNCTION_");
			emit_out(s);
			emit_out("\n");
	}

	for(; passed > 0; passed = passed - 1)
	{
		emit_out("pop_ebx\t# _process_expression_locals\n");
	}

		emit_out("pop_ebp\t# Restore old base pointer\n");
		emit_out("pop_edi\t# Prevent overwrite\n");
}

void constant_load(struct token_list* a)
{
	emit_out("mov_eax, %");
	emit_out(a->arguments->s);
	emit_out("\n");
}

char* load_value_signed(unsigned size)
{
	if(size == 1)
	{
		return "movsx_eax,BYTE_PTR_[eax]\n";
	}
	else if(size == 2)
	{
		return "movsx_eax,WORD_PTR_[eax]\n";
	}
	else if(size == 4)
	{
		return "mov_eax,[eax]\n";
	}
	line_error();
	fputs(" Got unsupported size ", stderr);
	fputs(int2str(size, 10, TRUE), stderr);
	fputs(" when trying to load value.\n", stderr);
	exit(EXIT_FAILURE);
}

char* load_value_unsigned(unsigned size)
{
	if(size == 1)
	{
		return "movzx_eax,BYTE_PTR_[eax]\n";
	}
	else if(size == 2)
	{
		return "movzx_eax,WORD_PTR_[eax]\n";
	}
	else if(size == 4)
	{
		return "mov_eax,[eax]\n";
	}
	fputs(" Got unsupported size ", stderr);
	fputs(int2str(size, 10, TRUE), stderr);
	fputs(" when trying to load value.\n", stderr);
	exit(EXIT_FAILURE);
}

char* load_value(unsigned size, int is_signed)
{
	if(is_signed) return load_value_signed(size);
	return load_value_unsigned(size);
}

char* store_value(unsigned size)
{
	if(size == 1)
	{
		return "mov_[ebx],al\n";
	}
	else if(size == 2)
	{
		return "mov_[ebx],ax\n";
	}
	else if(size == 4)
	{
		return "mov_[ebx],eax\n";
	}
	/* Should not happen but print error message. */
	fputs("Got unsupported size ", stderr);
	fputs(int2str(size, 10, TRUE), stderr);
	fputs(" when storing number in register.\n", stderr);
	line_error();
	exit(EXIT_FAILURE);
}

int is_compound_assignment(char* token)
{
	return FALSE;
}

void postfix_expr_stub();
void variable_load(struct token_list* a, int num_dereference)
{
	require(NULL != global_token, "incomplete variable load received\n");
	if((match("FUNCTION", a->type->name) || match("FUNCTION*", a->type->name)) && match("(", global_token->s))
	{
		function_call(int2str(a->depth, 10, TRUE), TRUE);
		return;
	}
	current_target = a->type;

	emit_out("lea_eax,[ebp+DWORD] %");

	emit_out(int2str(a->depth, 10, TRUE));
	emit_out("\n");

	if(TRUE == Address_of) return;
	if(match(".", global_token->s))
	{
		postfix_expr_stub();
		return;
	}
	if(!match("=", global_token->s) && !is_compound_assignment(global_token->s))
	{
		emit_out(load_value(current_target->size, current_target->is_signed));
	}

	while (num_dereference > 0)
	{
		current_target = current_target->type;
		emit_out(load_value(current_target->size, current_target->is_signed));
		num_dereference = num_dereference - 1;
	}
}

void function_load(struct token_list* a)
{
	require(NULL != global_token, "incomplete function load\n");
	if(match("(", global_token->s))
	{
		function_call(a->s, FALSE);
		return;
	}

	emit_out("mov_eax, &FUNCTION_");
	emit_out(a->s);
	emit_out("\n");
}

void global_load(struct token_list* a)
{
	current_target = a->type;
	emit_out("mov_eax, &GLOBAL_");
	emit_out(a->s);
	emit_out("\n");

	require(NULL != global_token, "unterminated global load\n");
	if(TRUE == Address_of) return;
	if(match("=", global_token->s) || is_compound_assignment(global_token->s)) return;

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

void primary_expr_failure()
{
	require(NULL != global_token, "hit EOF when expecting primary expression\n");
	line_error();
	fputs("Received ", stderr);
	fputs(global_token->s, stderr);
	fputs(" in primary_expr\n", stderr);
	exit(EXIT_FAILURE);
}

void primary_expr_string()
{
	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;
	emit_out("mov_eax, &STRING_");
	uniqueID_out(function->s, number_string);

	/* The target */
	strings_list = emit(":STRING_", strings_list);
	strings_list = uniqueID(function->s, strings_list, number_string);

	/* catch case of just "foo" from segfaulting */
	require(NULL != global_token->next, "a string by itself is not valid C\n");

	/* Parse the string */
	if('"' != global_token->next->s[0])
	{
		strings_list = emit(parse_string(global_token->s), strings_list);
		global_token = global_token->next;
	}
	else
	{
		char* s = calloc(MAX_STRING, sizeof(char));

		/* prefix leading string */
		s[0] = '"';
		int i = 1;

		int j;
		while('"' == global_token->s[0])
		{
			/* Step past the leading '"' */
			j = 1;

			/* Copy the rest of the string as is */
			while(0 != global_token->s[j])
			{
				require(i < MAX_STRING, "concat string exceeded max string length\n");
				s[i] = global_token->s[j];
				i = i + 1;
				j = j + 1;
			}

			/* Move on to the next token */
			global_token = global_token->next;
			require(NULL != global_token, "multi-string null is not valid C\n");
		}

		/* Now use it */
		strings_list = emit(parse_string(s), strings_list);
	}
}

void primary_expr_char()
{
	emit_out("mov_eax, %");
	emit_out(int2str(escape_lookup(global_token->s + 1), 10, TRUE));
	emit_out("\n");
	global_token = global_token->next;
}

int hex2char(int c)
{
	if((c >= 0) && (c <= 9)) return (c + 48);
	else if((c >= 10) && (c <= 15)) return (c + 55);
	else return -1;
}

char* number_to_hex(int a, int bytes)
{
	require(bytes > 0, "number to hex must have a positive number of bytes greater than zero\n");
	char* result = calloc(1 + (bytes << 1), sizeof(char));
	if(NULL == result)
	{
		fputs("calloc failed in number_to_hex\n", stderr);
		exit(EXIT_FAILURE);
	}
	int i = 0;

	int divisor = (bytes << 3);
	require(divisor > 0, "unexpected wrap around in number_to_hex\n");

	/* Simply collect numbers until divisor is gone */
	while(0 != divisor)
	{
		divisor = divisor - 4;
		result[i] = hex2char((a >> divisor) & 0xF);
		i = i + 1;
	}

	return result;
}

void primary_expr_number()
{
		emit_out("mov_eax, %");
		emit_out(global_token->s);
	emit_out("\n");
	global_token = global_token->next;
}

void primary_expr_variable()
{
	int num_dereference = 0;
	while(global_token->s[0] == '*') {
		global_token = global_token->next;
		require(NULL != global_token, "Walked off the end of a variable dereference\n");
		num_dereference = num_dereference + 1;
	}
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
struct type* lookup_member(struct type* parent, char* name);

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

	if(match("=", global_token->s) || is_compound_assignment(global_token->s) || match(".", global_token->s))
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

void postfix_expr_stub()
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

void expression()
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


int iskeywordp(char* s)
{
	if(match("auto", s)) return TRUE;
	if(match("break", s)) return TRUE;
	if(match("case", s)) return TRUE;
	if(match("char", s)) return TRUE;
	if(match("const", s)) return TRUE;
	if(match("continue", s)) return TRUE;
	if(match("default", s)) return TRUE;
	if(match("do", s)) return TRUE;
	if(match("double", s)) return TRUE;
	if(match("else", s)) return TRUE;
	if(match("enum", s)) return TRUE;
	if(match("extern", s)) return TRUE;
	if(match("float", s)) return TRUE;
	if(match("for", s)) return TRUE;
	if(match("goto", s)) return TRUE;
	if(match("if", s)) return TRUE;
	if(match("int", s)) return TRUE;
	if(match("long", s)) return TRUE;
	if(match("register", s)) return TRUE;
	if(match("return", s)) return TRUE;
	if(match("short", s)) return TRUE;
	if(match("signed", s)) return TRUE;
	if(match("sizeof", s)) return TRUE;
	if(match("static", s)) return TRUE;
	if(match("struct", s)) return TRUE;
	if(match("switch", s)) return TRUE;
	if(match("typedef", s)) return TRUE;
	if(match("union", s)) return TRUE;
	if(match("unsigned", s)) return TRUE;
	if(match("void", s)) return TRUE;
	if(match("volatile", s)) return TRUE;
	if(match("while", s)) return TRUE;
	return FALSE;
}

/* Similar to integer division a / b but rounds up */
unsigned ceil_div(unsigned a, unsigned b)
{
    return (a + b - 1) / b;
}

/* Process local variable */
void collect_local()
{
	if(NULL != break_target_func)
	{
		fputs("Local variable initialized inside of loop in file: ", stderr);
		line_error();
		fputs("\nMove the variable outside of the loop to resolve\n", stderr);
		fputs("Otherwise the binary will segfault while running\n", stderr);
		exit(EXIT_FAILURE);
	}
	struct type* type_size = type_name();
	require(NULL != global_token, "Received EOF while collecting locals\n");
	require(!in_set(global_token->s[0], "[{(<=>)}]|&!^%;:'\""), "forbidden character in local variable name\n");
	require(!iskeywordp(global_token->s), "You are not allowed to use a keyword as a local variable name\n");
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
	char* nested_continue_head = continue_target_head;

	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	break_target_head = "FOR_END_";
	continue_target_head = "FOR_ITER_";
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
	continue_target_head = nested_continue_head;
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
	char* nested_continue_head = continue_target_head;

	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	break_target_head = "DO_END_";
	continue_target_head = "DO_TEST_";
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
	continue_target_head = nested_continue_head;
}


/* Process while loops */
void process_while()
{
	struct token_list* nested_locals = break_frame;
	char* nested_break_head = break_target_head;
	char* nested_break_func = break_target_func;
	char* nested_break_num = break_target_num;
	char* nested_continue_head = continue_target_head;

	char* number_string = int2str(current_count, 10, TRUE);
	current_count = current_count + 1;

	break_target_head = "END_WHILE_";
	continue_target_head = "WHILE_";
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
	continue_target_head = nested_continue_head;
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

struct type* lookup_type(char* s, struct type* start);
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
			require(!iskeywordp(global_token->s), "You are not allowed to use a keyword as a argument variable name\n");
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
	unsigned i;
	function = NULL;
	Address_of = FALSE;
	struct type* type_size;

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

void recursive_output(struct token_list* head, FILE* out)
{
	struct token_list* i = reverse_list(head);
	while(NULL != i)
	{
		fputs(i->s, out);
		i = i->next;
	}
}

void init_macro_env(char* sym, char* value, char* source, int num)
{
	struct macro_list* hold = macro_env;
	macro_env = calloc(1, sizeof(struct macro_list));
	macro_env->symbol = sym;
	macro_env->next = hold;
	macro_env->expansion = calloc(1, sizeof(struct token_list));
	macro_env->expansion->s = value;
	macro_env->expansion->filename = source;
	macro_env->expansion->linenumber = num;
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

int main(int argc, char** argv)
{
	MAX_STRING = 4096;
	BOOTSTRAP_MODE = TRUE;
	FILE* in;
	FILE* destination_file;
	Architecture = X86;
	init_macro_env("__M2__", "42", "__INTERNAL_M2__", 0); /* Setup __M2__ */
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

	init_macro_env("__i386__", "1", "--architecture", env);
	env = env + 1;

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
