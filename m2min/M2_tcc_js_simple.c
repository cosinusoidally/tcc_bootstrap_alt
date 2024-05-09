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

int quote_string;

int EOF;

/* init support code */
int init_support();

/* structs are not used, kept as comment for reference */
/*
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
*/

int type_next_offset;
int type_size_offset;
int type_offset_offset;
int type_is_signed_offset;
int type_indirect_offset;
int type_members_offset;
int type_type_offset;
int type_name_offset;
int sizeof_type;

int type_list_layout_init(){
	type_next_offset = 0;
	type_size_offset = 4;
	type_offset_offset = 8;
	type_is_signed_offset = 12;
	type_indirect_offset = 16;
	type_members_offset = 20;
	type_type_offset = 24;
	type_name_offset = 28;

	sizeof_type = 32;
}

/* structs are not used. kept as a comment as reference */
/*
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
*/

int token_list_next_offset;
int token_list_locals_offset;
int token_list_prev_offset;
int token_list_s_offset;
int token_list_type_offset;
int token_list_filename_offset;
int token_list_arguments_offset;
int token_list_depth_offset;
int token_list_linenumber_offset;
int sizeof_token_list;

int token_list_layout_init(){
	token_list_next_offset = 0;
	token_list_locals_offset = 4;
	token_list_prev_offset = 4;
	token_list_s_offset = 8;
	token_list_type_offset = 12;
	token_list_filename_offset = 12;
	token_list_arguments_offset = 16;
	token_list_depth_offset = 16;
	token_list_linenumber_offset = 16;

	sizeof_token_list = 20;
}

/* type getters and setters */

int sty_next(int t,int v) {
        wi32(add(t, type_next_offset), v);
}

int gty_next(int t) {
        return ri32(add(t, type_next_offset));
}

int sty_size(int t,int v) {
        wi32(add(t, type_size_offset), v);
}

int gty_size(int t) {
        return ri32(add(t, type_size_offset));
}

int sty_is_signed(int t,int v) {
        wi32(add(t, type_is_signed_offset), v);
}

int gty_is_signed(int t) {
        return ri32(add(t, type_is_signed_offset));
}

int sty_indirect(int t,int v) {
        wi32(add(t, type_indirect_offset), v);
}

int gty_indirect(int t) {
        return ri32(add(t, type_indirect_offset));
}

int sty_type(int t,int v) {
        wi32(add(t, type_type_offset), v);
}

int gty_type(int t) {
        return ri32(add(t, type_type_offset));
}

int sty_name(int t,int v) {
        wi32(add(t, type_name_offset), v);
}

int gty_name(int t) {
        return ri32(add(t, type_name_offset));
}

/* token_list getters and setters */

int stl_next(int t,int v) {
        wi32(add(t, token_list_next_offset), v);
}

int gtl_next(int t) {
        return ri32(add(t, token_list_next_offset));
}

int stl_locals(int t,int v) {
        wi32(add(t, token_list_locals_offset), v);
}

int gtl_locals(int t) {
        return ri32(add(t, token_list_locals_offset));
}

int stl_prev(int t,int v) {
        wi32(add(t, token_list_prev_offset), v);
}

int gtl_prev(int t) {
        return ri32(add(t, token_list_prev_offset));
}

int stl_s(int t,int v) {
        wi32(add(t, token_list_s_offset), v);
}

int gtl_s(int t) {
        return ri32(add(t, token_list_s_offset));
}

int stl_type(int t,int v) {
        wi32(add(t, token_list_type_offset), v);
}

int gtl_type(int t) {
        return ri32(add(t, token_list_type_offset));
}

int stl_filename(int t,int v) {
        wi32(add(t, token_list_filename_offset), v);
}

int gtl_filename(int t) {
        return ri32(add(t, token_list_filename_offset));
}

int stl_arguments(int t,int v) {
        wi32(add(t, token_list_arguments_offset), v);
}

int gtl_arguments(int t) {
        return ri32(add(t, token_list_arguments_offset));
}

int stl_depth(int t,int v) {
        wi32(add(t, token_list_depth_offset), v);
}

int gtl_depth(int t) {
        return ri32(add(t, token_list_depth_offset));
}

int stl_linenumber(int t,int v) {
        wi32(add(t, token_list_linenumber_offset), v);
}

int gtl_linenumber(int t) {
        return ri32(add(t, token_list_linenumber_offset));
}

/* The core functions */
int initialize_types();
int reverse_list(int head);

int remove_line_comment_tokens(int head);
int remove_preprocessor_directives(int head);

int eat_newline_tokens();
int program();
int recursive_output(int i, int out);

/* What types we have */
int global_types;
int prim_types;

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
int integer;

/* Globals */
int input;
int token;
int line;
int file;

int eat_token(int head);

/* point where we are currently modifying the global_token list */
int macro_token;


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
int eat_token(int token) {
	if(neq(NULL, gtl_prev(token))) {
		stl_next(gtl_prev(token), gtl_next(token));
	}

	/* update backlinks */
	if(neq(NULL, gtl_next(token))) {
		stl_prev(gtl_next(token), gtl_prev(token));
	}

	return gtl_next(token);
}

int eat_until_newline(int head) {
	while (neq(NULL, head)) {
		if(eq('\n', ri8(gtl_s(head)))) {
			return head;
		} else {
			head = eat_token(head);
		}
	}

	return NULL;
}

int remove_line_comment_tokens(int head) {
	int first;

	first = NULL;

	while (neq(NULL, head)) {
		if(match("//", gtl_s(head))) {
			head = eat_token(head);
		} else {
			if(eq(NULL, first)) {
				first = head;
			}
			head = gtl_next(head);
		}
	}

	return first;
}

int remove_preprocessor_directives(int head) {
	int first;

	first = NULL;

	while (neq(NULL, head)) {
		if(eq('#', ri8(gtl_s(head)))) {
			head = eat_until_newline(head);
		} else {
			if(eq(NULL, first)) {
				first = head;
			}
			head = gtl_next(head);
		}
	}

	return first;
}

int new_token(int s, int size) {
	int current;

	current = calloc(1, sizeof_token_list);
	require(neq(NULL, current), "Exhausted memory while getting token\n");

	/* More efficiently allocate memory for string */
	stl_s(current, calloc(size, 1));
	require(neq(NULL, gtl_s(current)), "Exhausted memory while trying to copy a token\n");
	copy_string(gtl_s(current), s, MAX_STRING);

	stl_prev(current, token);
	stl_next(current, token);
	stl_linenumber(current, line);
	stl_filename(current, file);
	token = current;
}

int get_token(int c) {
	int reset;
	int current;

	current = calloc(1, sizeof_token_list);
	require(neq(NULL, current), "Exhausted memory while getting token\n");

	reset = 1;
	while(eq(reset, 1)) {
		reset = 0;
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
		} else if(in_set(c, quote_string)) {
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
				reset = 1;
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
	}

	new_token(hold_string, add(string_index, 2));
	return c;
}

int reverse_list(int head) {
	int root;
	int next;

	root = NULL;
	while(neq(NULL, head)) {
		next = gtl_next(head);
		stl_next(head, root);
		root = head;
		head = next;
	}
	return root;
}

int read_all_tokens(int a, int current, int filename) {
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

	while(1) {
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
		if(eq(ri8(string), 0)) {
			break;
		}
	}

	wi8(add(hold_string, string_index), '"');
	wi8(add(hold_string, add(string_index, 1)), '\n');
	message = calloc(add(string_index, 3), 1);
	require(neq(NULL, message), "Exhausted memory while storing regular string\n");
	copy_string(message, hold_string, add(string_index, 2));
	reset_hold_string();
	return message;
}

/*
int collect_weird_string(int string) {
	fputs("collect_weird_string not impl\n", stdout);
	fputs("line: ", stdout);
	fputs(int2str(gtl_linenumber(global_token), 10, 0), stdout);
	fputs("\n", stdout);
	exit(1);
}
*/

/* Deal with non-human strings */
int collect_weird_string(int string) {
        int temp;
        int table;
        int hold;

        string_index = 1;
        table = "0123456789ABCDEF";

        wi8(hold_string, '\'');

	while(1) {
		require(gt((sub(MAX_STRING, 6)), string_index), "Attempt at parsing weird string exceeds max length\n");
		string = add(string, 1);
		wi8(add(hold_string, string_index), ' ');
		temp = and(escape_lookup(string), 0xFF);
		wi8(add(hold_string, add(string_index, 1)), ri8(add(table,shr(temp, 4))));
		wi8(add(hold_string, add(string_index, 2)), ri8(add(table, and(temp, 15))));

		if(eq(ri8(string), '\\')) {
			if(eq(ri8(add(string, 1)), 'x')) {
				string = add(string, 2);
			}
			string = add(string, 1);
		}

		string_index = add(string_index, 3);
		if(eq(ri8(add(string, 1)), 0)) {
			break;
		}
	}

        wi8(add(hold_string, string_index), ' ');
        wi8(add(hold_string, add(string_index, 1)), '0');
        wi8(add(hold_string, add(string_index, 2)), '0');
        wi8(add(hold_string, add(string_index, 3)), '\'');
        wi8(add(hold_string, add(string_index, 4)), '\n');

        hold = calloc(add(string_index, 6), 1);
        require(neq(NULL, hold), "Exhausted available memory while attempting to collect a weird string\n");
        copy_string(hold, hold_string, add(string_index, 5));
        reset_hold_string();
        return hold;
}


int weird(int string) {
	int c;
	string = add(string, 1);

	while(1) {
		c = ri8(string);
		if(eq(0, c)) {
			return FALSE;
		}
		if(eq('\\', c)) {
			c = escape_lookup(string);
			if(eq('x', ri8(add(string, 1)))) {
				string = add(string, 2);
			}
			string = add(string, 1);
		}
		if(eq(0, in_set(c, "\t\n !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"))) {
			return TRUE;
		}
		if(and(in_set(c, " \t\n\r"), (eq(':', ri8(add(string, 1)))))) {
			return TRUE;
		}
		string = add(string, 1);
	}
}

/* Parse string to deal with hex characters*/
int parse_string(int string) {
	/* the string */
        if(weird(string)) {
		return collect_weird_string(string);
	} else {
		return collect_regular_string(string);
	}
}

/* enable easy primitive extension */
int add_primitive(int a) {
	int i;
	if(eq(NULL, prim_types)) {
		return a;
	}
	i = prim_types;
	while(neq(NULL, gty_next(i))) {
		i = gty_next(i);
	}
	sty_next(i, a);

	return prim_types;
}

/* enable easy primitive creation */
int new_primitive(int name0, int name1, int name2, int size, int sign) {
	/* Create type** */
	int a;
	int b;
	int r;

	a = calloc(1, sizeof_type);
	require(neq(NULL, a), "Exhausted memory while declaring new primitive**\n");
	sty_name(a, name2);
	sty_size(a, register_size);
	sty_indirect(a, a);
	sty_is_signed(a, sign);

	/* Create type* */
	b = calloc(1, sizeof_type);
	require(neq(NULL, b), "Exhausted memory while declaring new primitive*\n");
	sty_name(b, name1);
	sty_size(b, register_size);
	sty_is_signed(b, sign);
	sty_indirect(b, a);
	sty_type(a, b);

	r = calloc(1, sizeof_type);
	require(neq(NULL, r), "Exhausted memory while declaring new primitive\n");
	sty_name(r, name0);
	sty_size(r, size);
	sty_is_signed(r, sign);
	sty_indirect(r, b);
	sty_type(r, r);
	sty_type(b, r);

	return r;
}

/* Initialize default types */
int initialize_types() {
	int hold;

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

int lookup_type(int s, int start) {
	int i;

	i = start;
	while(1) {
		if(eq(NULL, i)) {
			break;
		}
		if(match(gty_name(i), s)) {
			return i;
		}
		i = gty_next(i);
	}
	return NULL;
}

int member_size;

int type_name() {
	int ret;

	require(neq(NULL, global_token), "Received EOF instead of type name\n");

	ret = lookup_type(gtl_s(global_token), global_types);

	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "unfinished type definition\n");

	while(eq(ri8(gtl_s(global_token)), '*')) {
		ret = gty_indirect(ret);
		global_token = gtl_next(global_token);
		require(neq(NULL, global_token), "unfinished type definition in indirection\n");
	}

	return ret;
}


/* Global lists */
int global_symbol_list;
int global_function_list;
int global_constant_list;

/* Core lists for this file */
int function;

/* What we are currently working on */
int current_target;
int break_target_head;
int break_target_func;
int break_target_num;
int break_frame;
int current_count;

int add_primitive(int a);

int emit(int s, int head) {
	int t;
	t = calloc(1, sizeof_token_list);
	require(neq(NULL, t), "Exhausted memory while generating token to emit\n");
	stl_next(t, head);
	stl_s(t, s);
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

int sym_declare(int s, int t, int list) {
	int a;
	a = calloc(1, sizeof_token_list);
	require(neq(NULL, a), "Exhausted memory while attempting to declare a symbol\n");
	stl_next(a, list);
	stl_s(a, s);
	stl_type(a, t);
	return a;
}

int sym_lookup(int s, int symbol_list) {
	int i;

	i = symbol_list;
	while(1) {
		if(eq(NULL, i)) {
			break;
		}
		if(match(gtl_s(i), s)) {
			return i;
		}
		i = gtl_next(i);
	}
	return NULL;
}

int line_error_token(int token) {
	if(eq(NULL, token)) {
		fputs("EOF reached inside of line_error\n", stderr);
		fputs("problem at end of file\n", stderr);
		return;
	}
	fputs(gtl_filename(token), stderr);
	fputs(":", stderr);
	fputs(int2str(gtl_linenumber(token), 10, TRUE), stderr);
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
	if(eq(0, match(gtl_s(global_token), required))) {
		line_error();
		fputs(message, stderr);
		exit(EXIT_FAILURE);
	}
	global_token = gtl_next(global_token);
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

	if(neq(ri8(gtl_s(global_token)), ')')) {
		expression();
		require(neq(NULL, global_token), "incomplete function call, received EOF instead of )\n");
		emit_out("push_eax\t#_process_expression1\n");
		passed = 1;

		while(eq(ri8(gtl_s(global_token)), ',')) {
			global_token = gtl_next(global_token);
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

	while(1) {
		if(lte(passed, 0)) {
			break;
		}
		emit_out("pop_ebx\t# _process_expression_locals\n");
		passed = sub(passed, 1);
	}

	emit_out("pop_ebp\t# Restore old base pointer\n");
	emit_out("pop_edi\t# Prevent overwrite\n");
}

int constant_load(int a) {
	emit_out("mov_eax, %");
	emit_out(gtl_s(gtl_arguments(a)));
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
int variable_load(int a, int num_dereference)
{
	require(neq(NULL, global_token), "incomplete variable load received\n");
	if(and(or(match("FUNCTION", gty_name(gtl_type(a))), match("FUNCTION*", gty_name(gtl_type(a)))), match("(", gtl_s(global_token)))) {
		function_call(int2str(gtl_depth(a), 10, TRUE), TRUE);
		return;
	}
	current_target = gtl_type(a);

	emit_out("lea_eax,[ebp+DWORD] %");

	emit_out(int2str(gtl_depth(a), 10, TRUE));
	emit_out("\n");

	if(eq(0, match("=", gtl_s(global_token)))) {
		emit_out(load_value(gty_size(current_target), gty_is_signed(current_target)));
	}

	while (gt(num_dereference, 0)) {
		current_target = gty_type(current_target);
		emit_out(load_value(gty_size(current_target), gty_is_signed(current_target)));
		num_dereference = sub(num_dereference, 1);
	}
}

int function_load(int a) {
	require(neq(NULL, global_token), "incomplete function load\n");
	if(match("(", gtl_s(global_token))) {
		function_call(gtl_s(a), FALSE);
		return;
	}

	emit_out("mov_eax, &FUNCTION_");
	emit_out(gtl_s(a));
	emit_out("\n");
}

int global_load(int a) {
	current_target = gtl_type(a);
	emit_out("mov_eax, &GLOBAL_");
	emit_out(gtl_s(a));
	emit_out("\n");

	require(neq(NULL, global_token), "unterminated global load\n");

	if(match("=", gtl_s(global_token))) {
		return;
	}

	emit_out(load_value(register_size, gty_is_signed(current_target)));
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
	fputs(gtl_s(global_token), stderr);
	fputs(" in primary_expr\n", stderr);
	exit(EXIT_FAILURE);
}

int primary_expr_string() {
	int number_string;
	number_string = int2str(current_count, 10, TRUE);
	current_count = add(current_count, 1);
	emit_out("mov_eax, &STRING_");
	uniqueID_out(gtl_s(function), number_string);

	/* The target */
	strings_list = emit(":STRING_", strings_list);
	strings_list = uniqueID(gtl_s(function), strings_list, number_string);

	/* catch case of just "foo" from segfaulting */
	require(neq(NULL, gtl_next(global_token)), "a string by itself is not valid C\n");

	/* Parse the string */
	if(neq('"', ri8(gtl_s(gtl_next(global_token))))) {
		strings_list = emit(parse_string(gtl_s(global_token)), strings_list);
		global_token = gtl_next(global_token);
	}
}

int primary_expr_char() {
	emit_out("mov_eax, %");
	emit_out(int2str(escape_lookup(add(gtl_s(global_token), 1)), 10, TRUE));
	emit_out("\n");
	global_token = gtl_next(global_token);
}

int primary_expr_number() {
	emit_out("mov_eax, %");
	emit_out(gtl_s(global_token));
	emit_out("\n");
	global_token = gtl_next(global_token);
}

int primary_expr_variable() {
	int num_dereference;
	int s;
	int a;

	num_dereference = 0;
	s = gtl_s(global_token);
	global_token = gtl_next(global_token);
	a = sym_lookup(s, global_constant_list);
	if(neq(NULL, a)) {
		constant_load(a);
		return;
	}

	a = sym_lookup(s, gtl_locals(function));
	if(neq(NULL, a)) {
		variable_load(a, num_dereference);
		return;
	}

	a = sym_lookup(s, gtl_arguments(function));
	if(neq(NULL, a)) {
		variable_load(a, num_dereference);
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

	line_error();
	fputs(s ,stderr);
	fputs(" is not a defined symbol\n", stderr);
	exit(EXIT_FAILURE);
}

int primary_expr();
int promote_type(int a, int b) {
	int i;

	require(neq(NULL, b), "impossible case 1 in promote_type\n");
	require(neq(NULL, a), "impossible case 2 in promote_type\n");

	if(eq(a, b)) {
		return a;
	}

	i = global_types;
	while(1) {
		if(eq(NULL, i)) {
			break;
		}
		if(eq(gty_name(a), gty_name(i))) { break; }
		if(eq(gty_name(b), gty_name(i))) { break; }
		if(eq(gty_name(a), gty_name(gty_indirect(i)))) { break; }
		if(eq(gty_name(b), gty_name(gty_indirect(i)))) { break; }
		i = gty_next(i);
	}
	require(neq(NULL, i), "impossible case 3 in promote_type\n");
	return i;
}

int postfix_expr();
int relational_expr_stub();
int bitwise_expr_stub();
int additive_expr_stub();
int additive_expr();
int relational_expr();

int fn_expression;
int fn_primary_expr;
int fn_postfix_expr;
int fn_additive_expr;
int fn_relational_expr;
int fn_relational_expr_stub;
int fn_bitwise_expr_stub;
int fn_additive_expr_stub;

int dispatch(int fn) {
	if(eq(fn, fn_expression)) {
		expression();
	} else if(eq(fn, fn_primary_expr)) {
		primary_expr();
	} else if(eq(fn, fn_postfix_expr)) {
		postfix_expr();
	} else if(eq(fn, fn_additive_expr)) {
		additive_expr();
	} else if(eq(fn, fn_relational_expr)) {
		relational_expr();
	} else if(eq(fn, fn_relational_expr_stub)) {
		relational_expr_stub();
	} else if(eq(fn, fn_bitwise_expr_stub)) {
		bitwise_expr_stub();
	} else if(eq(fn, fn_additive_expr_stub)) {
		additive_expr_stub();
	} else {
		fputs("unsupported dispatch\n", stdout);
		exit(1);
	}
}

int common_recursion(int f) {
	int last_type;
	emit_out("push_eax\t#_common_recursion\n");

	last_type = current_target;
	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "Received EOF in common_recursion\n");
	dispatch(f);
	current_target = promote_type(current_target, last_type);

	emit_out("pop_ebx\t# _common_recursion\n");
}

int general_recursion(int f, int s, int name, int iterate) {
	require(neq(NULL, global_token), "Received EOF in general_recursion\n");
	if(match(name, gtl_s(global_token)))
	{
		common_recursion(f);
		emit_out(s);
		dispatch(iterate);
	}
}

int arithmetic_recursion(int f, int s1, int s2, int name, int iterate) {
	require(neq(NULL, global_token), "Received EOF in arithmetic_recursion\n");
	if(match(name, gtl_s(global_token))) {
		common_recursion(f);
		if(eq(NULL, current_target)) {
			emit_out(s1);
		} else if(gty_is_signed(current_target)) {
			emit_out(s1);
		} else {
			emit_out(s2);
		}
		dispatch(iterate);
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

int postfix_expr_array() {
	int array;
	int assign;

	array = current_target;
	common_recursion(fn_expression);
	current_target = array;
	require(neq(NULL, current_target), "Arrays only apply to variables\n");

	assign = load_value(register_size, gty_is_signed(current_target));

	/* Add support for Ints */
	if(match("char*", gty_name(current_target))) {
		assign = load_value(1, TRUE);
	} else {
		emit_out("push_ebx\nmov_ebx, %");
		emit_out(int2str(gty_size(gty_type(current_target)), 10, TRUE));
		emit_out("\nmul_ebx\npop_ebx\n");
	}

	emit_out("add_eax,ebx\n");

	require_match("ERROR in postfix_expr\nMissing ]\n", "]");
	require(neq(NULL, global_token), "truncated array expression\n");

	if(or(match("=", gtl_s(global_token)), match(".", gtl_s(global_token)))) {
		assign = "";
	}

	if(match("[", gtl_s(global_token))) {
		current_target = gty_type(current_target);
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

int type_name();

int unary_expr_sizeof() {
	int a;

	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "Received EOF when starting sizeof\n");
	require_match("ERROR in unary_expr\nMissing (\n", "(");
	a = type_name();
	require_match("ERROR in unary_expr\nMissing )\n", ")");

	emit_out("mov_eax, %");
	emit_out(int2str(gty_size(a), 10, TRUE));
	emit_out("\n");
}

int postfix_expr_stub() {
	require(neq(NULL, global_token), "Unexpected EOF, improperly terminated primary expression\n");
	if(match("[", gtl_s(global_token))) {
		postfix_expr_array();
		postfix_expr_stub();
	}
}

int postfix_expr() {
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
int additive_expr_stub() {
		arithmetic_recursion(fn_postfix_expr, "add_eax,ebx\n", "add_eax,ebx\n", "+", fn_additive_expr_stub);
		arithmetic_recursion(fn_postfix_expr, "sub_ebx,eax\nmov_eax,ebx\n", "sub_ebx,eax\nmov_eax,ebx\n", "-", fn_additive_expr_stub);
		arithmetic_recursion(fn_postfix_expr, "imul_ebx\n", "mul_ebx\n", "*", fn_additive_expr_stub);
		arithmetic_recursion(fn_postfix_expr, "xchg_ebx,eax\ncdq\nidiv_ebx\n", "xchg_ebx,eax\nmov_edx, %0\ndiv_ebx\n", "/", fn_additive_expr_stub);
		arithmetic_recursion(fn_postfix_expr, "xchg_ebx,eax\ncdq\nidiv_ebx\nmov_eax,edx\n", "xchg_ebx,eax\nmov_edx, %0\ndiv_ebx\nmov_eax,edx\n", "%", fn_additive_expr_stub);
		arithmetic_recursion(fn_postfix_expr, "mov_ecx,eax\nmov_eax,ebx\nsal_eax,cl\n", "mov_ecx,eax\nmov_eax,ebx\nshl_eax,cl\n", "<<", fn_additive_expr_stub);
		arithmetic_recursion(fn_postfix_expr, "mov_ecx,eax\nmov_eax,ebx\nsar_eax,cl\n", "mov_ecx,eax\nmov_eax,ebx\nshr_eax,cl\n", ">>", fn_additive_expr_stub);
}


int additive_expr() {
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

int relational_expr_stub() {
		arithmetic_recursion(fn_additive_expr, "cmp\nsetl_al\nmovzx_eax,al\n", "cmp\nsetb_al\nmovzx_eax,al\n", "<", fn_relational_expr_stub);
		arithmetic_recursion(fn_additive_expr, "cmp\nsetle_al\nmovzx_eax,al\n", "cmp\nsetbe_al\nmovzx_eax,al\n", "<=", fn_relational_expr_stub);
		arithmetic_recursion(fn_additive_expr, "cmp\nsetge_al\nmovzx_eax,al\n", "cmp\nsetae_al\nmovzx_eax,al\n", ">=", fn_relational_expr_stub);
		arithmetic_recursion(fn_additive_expr, "cmp\nsetg_al\nmovzx_eax,al\n", "cmp\nseta_al\nmovzx_eax,al\n", ">", fn_relational_expr_stub);
		general_recursion(fn_additive_expr, "cmp\nsete_al\nmovzx_eax,al\n", "==", fn_relational_expr_stub);
		general_recursion(fn_additive_expr, "cmp\nsetne_al\nmovzx_eax,al\n", "!=", fn_relational_expr_stub);
}

int relational_expr() {
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
int bitwise_expr_stub() {
		general_recursion(fn_relational_expr, "and_eax,ebx\n", "&", fn_bitwise_expr_stub);
		general_recursion(fn_relational_expr, "and_eax,ebx\n", "&&", fn_bitwise_expr_stub);
		general_recursion(fn_relational_expr, "or_eax,ebx\n", "|", fn_bitwise_expr_stub);
		general_recursion(fn_relational_expr, "or_eax,ebx\n", "||", fn_bitwise_expr_stub);
		general_recursion(fn_relational_expr, "xor_eax,ebx\n", "^", fn_bitwise_expr_stub);
}


int bitwise_expr() {
	relational_expr();
	bitwise_expr_stub();
}

/*
 * expression:
 *         bitwise-or-expr
 *         bitwise-or-expr = expression
 */

int primary_expr() {
	require(neq(NULL, global_token), "Received EOF where primary expression expected\n");

	if(match("sizeof", gtl_s(global_token))) {
		unary_expr_sizeof();
	} else if(eq('-', ri8(gtl_s(global_token)))) {
		emit_out("mov_eax, %0\n");

		common_recursion(fn_primary_expr);

		emit_out("sub_ebx,eax\nmov_eax,ebx\n");
	} else if(eq('!', ri8(gtl_s(global_token)))) {
		emit_out("mov_eax, %1\n");

		common_recursion(fn_postfix_expr);

		emit_out("cmp\nseta_al\nmovzx_eax,al\n");
	} else if(eq('~', ri8(gtl_s(global_token)))) {
		common_recursion(fn_postfix_expr);

		emit_out("not_eax\n");
	} else if(eq(ri8(gtl_s(global_token)), '(')) {
		global_token = gtl_next(global_token);
		expression();
		require_match("Error in Primary expression\nDidn't get )\n", ")");
	} else if(eq(ri8(gtl_s(global_token)), '\'')) {
		primary_expr_char();
	} else if(eq(ri8(gtl_s(global_token)), '"')) {
		primary_expr_string();
	} else if(in_set(ri8(gtl_s(global_token)), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")) {
		primary_expr_variable();
	} else if(eq(ri8(gtl_s(global_token)), '*')) {
		primary_expr_variable();
	} else if(in_set(ri8(gtl_s(global_token)), "0123456789")) {
		primary_expr_number();
	} else {
		primary_expr_failure();
	}
}

int expression() {
	int store;
	bitwise_expr();
	if(match("=", gtl_s(global_token))) {
		store = "";
		if(match("]", gtl_s(gtl_prev(global_token)))) {
			store = store_value(gty_size(gty_type(current_target)));
		} else {
			store = store_value(gty_size(current_target));
		}

		common_recursion(fn_expression);
		emit_out(store);
		current_target = integer;
	}
}

/* Similar to integer division a / b but rounds up */
int ceil_div(int a, int b) {
    return div((sub(add(a, b), 1)), b);
}

/* Process local variable */
int collect_local() {
	int type_size;
	int struct_depth_adjustment;
	int i;
	int a;

	type_size = type_name();
	require(neq(NULL, global_token), "Received EOF while collecting locals\n");
/* temp disable as we don't support M2 weird strings */
/*
	require(eq(0, in_set(ri8(gtl_s(global_token)), "[{(<=>)}]|&!^%;:'\"")), "forbidden character in local variable name\n");
*/
	require(neq(NULL, type_size), "Must have non-null type\n");

	a = sym_declare(gtl_s(global_token), type_size, gtl_locals(function));
	if(and(match("main", gtl_s(function)), (eq(NULL, gtl_locals(function))))) {
		stl_depth(a, sub(0, 20));
	} else if(and(eq(NULL, gtl_arguments(function)), eq(NULL, gtl_locals(function)))) {
		stl_depth(a, sub(0, 8));
	} else if(eq(NULL, gtl_locals(function))) {
		stl_depth(a, sub(gtl_arguments(gtl_depth(function)), 8));
	} else {
		stl_depth(a, sub(gtl_depth(gtl_locals(function)), register_size));
	}

	/* Adjust the depth of local structs. When stack grows downwards, we want them to 
	   start at the bottom of allocated space. */
	struct_depth_adjustment = mul(sub(ceil_div(gty_size(gtl_type(a)), register_size), 1), register_size);
	stl_depth(a, sub(gtl_depth(a), struct_depth_adjustment));

	stl_locals(function, a);

	emit_out("# Defining local ");
	emit_out(gtl_s(global_token));
	emit_out("\n");

	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "incomplete local missing name\n");

	if(match("=", gtl_s(global_token))) {
		global_token = gtl_next(global_token);
		require(neq(NULL, global_token), "incomplete local assignment\n");
		expression();
	}

	require_match("ERROR in collect_local\nMissing ;\n", ";");

	i = div(sub(add(gty_size(gtl_type(a)), register_size), 1), register_size);
	while(neq(i, 0)) {
		emit_out("push_eax\t#");
		emit_out(gtl_s(a));
		emit_out("\n");
		i = sub(i, 1);
	}
}

int statement();

/* Evaluate if statements */
int process_if() {
	int number_string;

	number_string = int2str(current_count, 10, TRUE);
	current_count = add(current_count, 1);

	emit_out("# IF_");
	uniqueID_out(gtl_s(function), number_string);

	global_token = gtl_next(global_token);
	require_match("ERROR in process_if\nMISSING (\n", "(");
	expression();

	emit_out("test_eax,eax\nje %ELSE_");

	uniqueID_out(gtl_s(function), number_string);

	require_match("ERROR in process_if\nMISSING )\n", ")");
	statement();
	require(neq(NULL, global_token), "Reached EOF inside of function\n");

	emit_out("jmp %_END_IF_");

	uniqueID_out(gtl_s(function), number_string);

	emit_out(":ELSE_");
	uniqueID_out(gtl_s(function), number_string);

	if(match("else", gtl_s(global_token))) {
		global_token = gtl_next(global_token);
		require(neq(NULL, global_token), "Received EOF where an else statement expected\n");
		statement();
		require(neq(NULL, global_token), "Reached EOF inside of function\n");
	}
	emit_out(":_END_IF_");
	uniqueID_out(gtl_s(function), number_string);
}

int process_for() {
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

	break_target_head = "FOR_END_";
	break_target_num = number_string;
	break_frame = gtl_locals(function);
	break_target_func = gtl_s(function);

	emit_out("# FOR_initialization_");
	uniqueID_out(gtl_s(function), number_string);

	global_token = gtl_next(global_token);

	require_match("ERROR in process_for\nMISSING (\n", "(");
	if(eq(0, match(";", gtl_s(global_token)))) {
		expression();
	}

	emit_out(":FOR_");
	uniqueID_out(gtl_s(function), number_string);

	require_match("ERROR in process_for\nMISSING ;1\n", ";");
	expression();

	emit_out("test_eax,eax\nje %FOR_END_");

	uniqueID_out(gtl_s(function), number_string);

	emit_out("jmp %FOR_THEN_");

	uniqueID_out(gtl_s(function), number_string);

	emit_out(":FOR_ITER_");
	uniqueID_out(gtl_s(function), number_string);

	require_match("ERROR in process_for\nMISSING ;2\n", ";");
	expression();

	emit_out("jmp %FOR_");

	uniqueID_out(gtl_s(function), number_string);

	emit_out(":FOR_THEN_");
	uniqueID_out(gtl_s(function), number_string);

	require_match("ERROR in process_for\nMISSING )\n", ")");
	statement();
	require(neq(NULL, global_token), "Reached EOF inside of function\n");

	emit_out("jmp %FOR_ITER_");

	uniqueID_out(gtl_s(function), number_string);

	emit_out(":FOR_END_");
	uniqueID_out(gtl_s(function), number_string);

	break_target_head = nested_break_head;
	break_target_func = nested_break_func;
	break_target_num = nested_break_num;
	break_frame = nested_locals;
}

/* Process Assembly statements */
int process_asm() {
	global_token = gtl_next(global_token);
	require_match("ERROR in process_asm\nMISSING (\n", "(");
	while(eq('"', ri8(gtl_s(global_token)))) {
		emit_out(add(gtl_s(global_token), 1));
		emit_out("\n");
		global_token = gtl_next(global_token);
		require(neq(NULL, global_token), "Received EOF inside asm statement\n");
	}
	require_match("ERROR in process_asm\nMISSING )\n", ")");
	require_match("ERROR in process_asm\nMISSING ;\n", ";");
}

/* Process do while loops */
int process_do() {
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

	break_target_head = "DO_END_";
	break_target_num = number_string;
	break_frame = gtl_locals(function);
	break_target_func = gtl_s(function);

	emit_out(":DO_");
	uniqueID_out(gtl_s(function), number_string);

	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "Received EOF where do statement is expected\n");
	statement();
	require(neq(NULL, global_token), "Reached EOF inside of function\n");

	emit_out(":DO_TEST_");
	uniqueID_out(gtl_s(function), number_string);

	require_match("ERROR in process_do\nMISSING while\n", "while");
	require_match("ERROR in process_do\nMISSING (\n", "(");
	expression();
	require_match("ERROR in process_do\nMISSING )\n", ")");
	require_match("ERROR in process_do\nMISSING ;\n", ";");

	emit_out("test_eax,eax\njne %DO_");

	uniqueID_out(gtl_s(function), number_string);

	emit_out(":DO_END_");
	uniqueID_out(gtl_s(function), number_string);

	break_frame = nested_locals;
	break_target_head = nested_break_head;
	break_target_func = nested_break_func;
	break_target_num = nested_break_num;
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
	break_frame = gtl_locals(function);
	break_target_func = gtl_s(function);

	emit_out(":WHILE_");
	uniqueID_out(gtl_s(function), number_string);

	global_token = gtl_next(global_token);
	require_match("ERROR in process_while\nMISSING (\n", "(");
	expression();

	emit_out("test_eax,eax\nje %END_WHILE_");

	uniqueID_out(gtl_s(function), number_string);

	emit_out("# THEN_while_");
	uniqueID_out(gtl_s(function), number_string);

	require_match("ERROR in process_while\nMISSING )\n", ")");
	statement();
	require(neq(NULL, global_token), "Reached EOF inside of function\n");

	emit_out("jmp %WHILE_");

	uniqueID_out(gtl_s(function), number_string);

	emit_out(":END_WHILE_");
	uniqueID_out(gtl_s(function), number_string);

	break_target_head = nested_break_head;
	break_target_func = nested_break_func;
	break_target_num = nested_break_num;
	break_frame = nested_locals;
}

/* Ensure that functions return */
int return_result() {
	int i;
	int size_local_var;

	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "Incomplete return statement received\n");
	if(neq(ri8(gtl_s(global_token)), ';')) {
		expression();
	}

	require_match("ERROR in return_result\nMISSING ;\n", ";");

	i = gtl_locals(function);
	while(1) {
		if(eq(NULL, i)) {
			break;
		}
		size_local_var = ceil_div(gty_size(gtl_type(i)), register_size);
		while(neq(size_local_var, 0)) {
			emit_out("pop_ebx\t# _return_result_locals\n");
			size_local_var = sub(size_local_var, 1);
		}
		i = gtl_next(i);
	}

	emit_out("ret\n");
}

int process_break() {
	int i;

	if(eq(NULL, break_target_head)) {
		line_error();
		fputs("Not inside of a loop or case statement\n", stderr);
		exit(EXIT_FAILURE);
	}

	i = gtl_locals(function);
	while(neq(i, break_frame)) {
		if(eq(NULL, i)) { break; }
		emit_out("pop_ebx\t# break_cleanup_locals\n");
		i = gtl_next(i);
	}
	global_token = gtl_next(global_token);

	emit_out("jmp %");

	emit_out(break_target_head);
	emit_out(break_target_func);
	emit_out("_");
	emit_out(break_target_num);
	emit_out("\n");
	require_match("ERROR in break statement\nMissing ;\n", ";");
}

int recursive_statement() {
	int i;
	int frame;

	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "Received EOF in recursive statement\n");
	frame = gtl_locals(function);

	while(eq(0, match("}", gtl_s(global_token)))) {
		statement();
		require(neq(NULL, global_token), "Received EOF in recursive statement prior to }\n");
	}
	global_token = gtl_next(global_token);

	/* Clean up any locals added */

	if(eq(0, match("ret\n", gtl_s(output_list)))) {
		i = gtl_locals(function);
		while(1) {
			if(eq(frame, i)) {
				break;
			}
			emit_out( "pop_ebx\t# _recursive_statement_locals\n");
			i = gtl_next(i);
		}
	}
	stl_locals(function, frame);
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

int statement() {
	require(neq(NULL, global_token), "expected a C statement but received EOF\n");
	/* Always an integer until told otherwise */
	current_target = integer;

	if(eq(ri8(gtl_s(global_token)), '{')) {
		recursive_statement();
	} else if(or(neq(NULL, lookup_type(gtl_s(global_token), prim_types)),
	          match("struct", gtl_s(global_token)))) {
		collect_local();
	} else if(match("if", gtl_s(global_token))) {
		process_if();
	} else if(match("do", gtl_s(global_token))) {
		process_do();
	} else if(match("while", gtl_s(global_token))) {
		process_while();
	} else if(match("for", gtl_s(global_token))) {
		process_for();
	} else if(match("asm", gtl_s(global_token))) {
		process_asm();
	} else if(match("return", gtl_s(global_token))) {
		return_result();
	} else if(match("break", gtl_s(global_token))) {
		process_break();
	} else {
		expression();
		require_match("ERROR in statement\nMISSING ;\n", ";");
	}
}

/* Collect function arguments */
int collect_arguments() {
	int cont;
	int type_size;
	int a;

	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "Received EOF when attempting to collect arguments\n");

	cont = 1; /* work around since no continue */
	while(eq(cont, 1)) {
		cont = 0;
		while(eq(0, match(")", gtl_s(global_token)))) {
			type_size = type_name();
			require(neq(NULL, global_token), "Received EOF when attempting to collect arguments\n");
			require(neq(NULL, type_size), "Must have non-null type\n");
			if(eq(ri8(gtl_s(global_token)), ')')) {
				/* foo(int,char,void) doesn't need anything done */
				cont = 1;
				break;
			} else if(neq(ri8(gtl_s(global_token)), ',')) {
				/* deal with foo(int a, char b) */
/* temp disable as we don't support M2 weird strings */
/*
				require(eq(0, in_set(ri8(gtl_s(global_token)), "[{(<=>)}]|&!^%;:'\"")), "forbidden character in argument variable name\n");
*/
				a = sym_declare(gtl_s(global_token), type_size, gtl_arguments(function));
				if(eq(NULL, gtl_arguments(function))) {
					stl_depth(a, sub(0, 4));
				} else {
					stl_depth(a, sub(gtl_depth(gtl_arguments(function)), register_size));
				}

				global_token = gtl_next(global_token);
				require(neq(NULL, global_token), "Incomplete argument list\n");
				stl_arguments(function, a);
			}

			/* ignore trailing comma (needed for foo(bar(), 1); expressions*/
			if(eq(ri8(gtl_s(global_token)), ',')) {
				global_token = gtl_next(global_token);
				require(neq(NULL, global_token), "naked comma in collect arguments\n");
			}

			require(neq(NULL, global_token), "Argument list never completed\n");
		}
	}
	global_token = gtl_next(global_token);
}

int declare_function() {
	current_count = 0;
	function = sym_declare(gtl_s(gtl_prev(global_token)), NULL, global_function_list);

	/* allow previously defined functions to be looked up */
	global_function_list = function;
	collect_arguments();

	require(neq(NULL, global_token), "Function definitions either need to be prototypes or full\n");
	/* If just a prototype don't waste time */
	if(eq(ri8(gtl_s(global_token)), ';')) {
		global_token = gtl_next(global_token);
	} else {
		emit_out("# Defining function ");
		emit_out(gtl_s(function));
		emit_out("\n");
		emit_out(":FUNCTION_");
		emit_out(gtl_s(function));
		emit_out("\n");
		statement();

		/* Prevent duplicate RETURNS */
		if(eq(0, match("ret\n", gtl_s(output_list)))) {
			emit_out("ret\n");
		}
	}
}

int global_constant() {
	global_token = gtl_next(global_token);
	require(neq(NULL, global_token), "CONSTANT lacks a name\n");
	global_constant_list = sym_declare(gtl_s(global_token), NULL, global_constant_list);

	require(neq(NULL, gtl_next(global_token)), "CONSTANT lacks a value\n");
	stl_arguments(global_constant_list, gtl_next(global_token));
	global_token = gtl_next(gtl_next(global_token));
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
int program() {
	int i;
	int type_size;

	function = NULL;

	while(1) {
		while(1) {
			while(1) {
				while(1) {
					/* Deal with garbage input */
					if (eq(NULL, global_token)) {
						return;
					}
					require(neq('#', ri8(gtl_s(global_token))), "unhandled macro directive\n");
					require(eq(0, match("\n", gtl_s(global_token))), "unexpected newline token\n");

					/* Handle cc_* CONSTANT statements */
					if(match("CONSTANT", gtl_s(global_token))) {
						global_constant();
					} else {
						break;
					}
				}

				type_size = type_name();
				/* Deal with case of struct definitions */
				if(neq(NULL, type_size)) {
					break;
				}
			}

			require(neq(NULL, gtl_next(global_token)), "Unterminated global\n");

			/* Add to global symbol table */
			global_symbol_list = sym_declare(gtl_s(global_token), type_size, global_symbol_list);
			global_token = gtl_next(global_token);

			/* Deal with global variables */
			if(match(";", gtl_s(global_token))) {
				/* Ensure enough bytes are allocated to store global variable.
				   In some cases it allocates too much but that is harmless. */
				globals_list = emit(":GLOBAL_", globals_list);
				globals_list = emit(gtl_s(gtl_prev(global_token)), globals_list);

				/* round up division */
				i = ceil_div(gty_size(type_size), register_size);
				globals_list = emit("\n", globals_list);
				while(neq(i, 0)) {
					globals_list = emit("NULL\n", globals_list);
					i = sub(i, 1);
				}
				global_token = gtl_next(global_token);
			} else {
				break;
			}
		}

		/* Deal with global functions */
		if(match("(", gtl_s(global_token))) {
			declare_function();
		} else {
			break;
		}
	}

	/* Everything else is just an error */
	line_error();
	fputs("Received ", stderr);
	fputs(gtl_s(global_token), stderr);
	fputs(" in program\n", stderr);
	exit(EXIT_FAILURE);
}

int recursive_output(int head, int out) {
	int i;

	i = reverse_list(head);
	while(neq(NULL, i)) {
		fputs(gtl_s(i), out);
		i = gtl_next(i);
	}
}

int eat_current_token() {
	int update_global_token;

	update_global_token = FALSE;
	if (eq(macro_token, global_token)) {
		update_global_token = TRUE;
	}

	macro_token = eat_token(macro_token);

	if(update_global_token) {
		global_token = macro_token;
	}
}

int eat_newline_tokens() {
	macro_token = global_token;

	while(TRUE) {
		if(eq(NULL, macro_token)) {
			return;
		}

		if(match("\n", gtl_s(macro_token))) {
			eat_current_token();
		} else {
			macro_token = gtl_next(macro_token);
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

	fn_expression = 1;
	fn_primary_expr = 2;
	fn_postfix_expr = 3;
	fn_additive_expr = 4;
	fn_relational_expr = 5;
	fn_relational_expr_stub = 6;
	fn_bitwise_expr_stub = 7;
	fn_additive_expr_stub = 8;

	quote_string=calloc(1, 16); /* round up */
	wi8(quote_string, '\'');
	wi8(add(quote_string, 1), '"');
	wi8(add(quote_string, 2), 0);

	token_list_layout_init();
	type_list_layout_init();
}

int main(int argc, int argv) {
	int name;
	int hold;
	int val;
	int i;
	int in;
	int destination_file;

        init_support();
        initialize_globals();

	i = 1;
	hold_string = calloc(add(MAX_STRING, 4), 1);

	name = ri32(add(argv,mul(4, i)));

	in = fopen(name, "r");
	global_token = read_all_tokens(in, global_token, name);
	fclose(in);
	i = add(i, 1);

	destination_file = fopen(ri32(add(argv, mul(4, i))), "w");
	i = add(i, 1);

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

	fclose(destination_file);
	return EXIT_SUCCESS;
}
