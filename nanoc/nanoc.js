/* This is a heavily cut down version of M2-Planet
 * modifications (C) 2024 Liam Wilson (under same license)
 * original authors
 * Copyright (C) 2016 Jeremiah Orians
 * Copyright (C) 2018 Jan (janneke) Nieuwenhuizen <janneke@gnu.org>
 * Copyright (C) 2020 deesix <deesix@tuta.io>
 * FIXME (Liam Wilson) I've mangled andrius name as gawk doesn't like the "S"
 * Copyright (C) 2021 Andrius Stikonas <andrius@stikonas.eu>
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

var FALSE;
var TRUE;
var NULL;

var EXIT_SUCCESS;
var EXIT_FAILURE;

var EOF;

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

var token_list_next_offset;
var token_list_prev_offset;
var token_list_s_offset;
var token_list_linenumber_offset;

function token_list_layout_init(){
	token_list_next_offset = 0;
	token_list_prev_offset = 4;
	token_list_s_offset = 8;
	token_list_linenumber_offset = 12;
}

var sizeof_token_list;

/* What we are currently working on */
var global_token;

/* Output reorder collections*/
var output_list;
var globals_list;

/* Make our string collection more efficient */
var hold_string;
var string_index;

/* Our Target Architecture */
var register_size;

var MAX_STRING;

/* Globals */
var input;
var token;
var line;

/* Global lists */
var global_symbol_list;
var global_function_list;

/* Core lists for this file */
var func;

var current_count;

function ri32(o) {
        return or(or(and(ri8(o), 255),
                shl(and(ri8(add(o, 1)), 255), 8)),
                or(shl(and(ri8(add(o, 2)), 255), 16),
                shl(and(ri8(add(o, 3)), 255), 24)));
}

function wi32(o, v) {
  wi8(o, and(v, 255));
  v = shr(v, 8);
  wi8(add(o, 1), and(v, 255));
  v = shr(v, 8);
  wi8(add(o, 2), and(v, 255));
  v = shr(v, 8);
  wi8(add(o, 3), and(v, 255));
}

function set_s(t, v) {
	wi32(add(t, token_list_s_offset), v);
}

function get_s( t) {
	return ri32(add(t, token_list_s_offset));
}

function set_prev(t, v) {
	wi32(add(t, token_list_prev_offset), v);
}

function get_prev(t) {
	return ri32(add(t, token_list_prev_offset));
}

function set_next(t, v) {
	wi32(add(t, token_list_next_offset), v);
}

function get_next(t) {
	return ri32(add(t, token_list_next_offset));
}

function set_linenumber(t, v) {
	wi32(add(t, token_list_linenumber_offset), v);
}

function get_linenumber(t) {
	return ri32(add(t, token_list_linenumber_offset));
}

function skip(str) {
/* dummy impl should check and abort if doesn't match */
	global_token = get_next(global_token);
}

/* advance to next token */
function advance() {
	global_token = get_next(global_token);
}

function token_string(a) {
	return get_s(a);
}

function global_token_string() {
	return token_string(global_token);
}

function global_token_char0() {
	return ri8(token_string(global_token));
}

function match(a, b) {
	var i;
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

function in_set(c, s) {
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

function grab_byte() {
	var c;
	c = fgetc(input);
	if(eq(10, c)) {
		line = add(line, 1);
	}
	return c;
}

function clearWhiteSpace(c) {
	if(or(eq(32, c), eq(9, c))) {
		return clearWhiteSpace(grab_byte());
	}
	return c;
}

function consume_byte(c) {
	wi8(add(hold_string, string_index), c);
	string_index = add(string_index, 1);
	return grab_byte();
}

function copy_string(target, source, max) {
	var i;
	i = 0;
	while(neq(0, ri8(add(source, i)))) {
		wi8(add(target, i), ri8(add(source, i)));
		i = add(i, 1);
		if(eq(i, max)) {
			break;
		}
	}
}

function preserve_keyword(c, S) {
	while(in_set(c, S)) {
		c = consume_byte(c);
	}
	return c;
}

function reset_hold_string() {
	var i;
	var o;
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

function new_token(s, size) {
	var current;
	current = calloc(1, sizeof_token_list);

	/* More efficiently allocate memory for string */
	set_s(current, calloc(size, 1));
	copy_string(get_s(current), s, MAX_STRING);

	set_prev(current, token);
	set_next(current, token);
	set_linenumber(current, line);
	token = current;
}

function get_token(c) {
	var current;
	var reset;
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
		} else if(in_set(c, mks("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"))) {
			c = preserve_keyword(c, mks("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
		} else if(in_set(c, mks("="))) {
			c = preserve_keyword(c, mks("="));
		} else if(eq(c, mkc('/'))) {
			c = consume_byte(c);
			if(eq(c, mkc('*'))) {
				c = grab_byte();
				while(neq(c, mkc('/'))) {
					while(neq(c, mkc('*'))) {
						c = grab_byte();
					}
					c = grab_byte();
				}
				c = grab_byte();
				reset = 1;
			}
		} else if (eq(c, mkc('\n'))) {
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

function reverse_list(head) {
	var root;
	var next;

	root = NULL;

	while(neq(NULL, head)) {
		next = get_next(head);
		set_next(head, root);
		root = head;
		head = next;
	}
	return root;
}

function read_all_tokens(a, current) {
	var ch;

	input  = a;
	line = 1;
	token = current;
	ch = grab_byte();
	while(neq(EOF, ch)) {
		ch = get_token(ch);
	}
	return token;
}

function emit(s, head) {
	var t;
	t = calloc(1, sizeof_token_list);
	set_next(t, head);
	set_s(t, s);
	return t;
}

function emit_out(s) {
	output_list = emit(s, output_list);
}

function sym_declare(s, list) {
	var a;
	a = calloc(1, sizeof_token_list);
	set_next(a, list);
	set_s(a, s);
	return a;
}

function sym_lookup(s, symbol_list) {
	var i;

	i = symbol_list;
	while(neq(NULL, i)) {
		if(match(get_s(i), s)) {
			return i;
		}
		i = get_next(i);
	}
	return NULL;
}

function function_call(s) {
	skip(mks("("));
	skip(mks(")"));

	emit_out(mks("call %FUNCTION_"));
	emit_out(s);

	emit_out(mks("\n"));
}

function load_value() {
	return mks("load ");
}

function function_load(a) {
	if(match(mks("("), global_token_string())) {
		function_call(a);
		return;
	}
}

function global_load(a) {
	emit_out(mks("global &GLOBAL_"));
	emit_out(get_s(a));
	emit_out(mks(" "));

	if(match(mks("="), global_token_string())) {
		return;
	}
	emit_out(load_value());
}

function primary_expr_number() {
	emit_out(mks("constant %"));
	emit_out(global_token_string());
	emit_out(mks(" "));
	advance();
}

function primary_expr_variable() {
	var a;
	var s;

	s = global_token_string();
	advance();

	a = sym_lookup(s, global_symbol_list);
	if(neq(NULL, a)) {
		global_load(a);
		return;
	}

	/* anything else is implicity a function */
	function_load(s);
	return;
}

function expression() {
	if(in_set(global_token_char0(), mks("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"))) {
		primary_expr_variable();
	} else if(in_set(global_token_char0(), mks("0123456789"))) {
		primary_expr_number();
	} else {
		exit(1);
	}

	if(match(mks("="), global_token_string())) {
		emit_out(mks("push_address "));
		advance();
		expression();
		emit_out(mks("store\n"));
	}
}

function recursive_statement() {
	advance();
	while(eq(0, match(mks("}"), global_token_string()))) {
		statement();
	}
	advance();
}

function statement() {
	if(eq(global_token_char0(), mkc('{'))) {
		recursive_statement();
	} else {
		expression();
		skip(mks(";"));
	}
}

function declare_function() {
	current_count = 0;
	func = sym_declare(get_s(get_prev(global_token)),
				global_function_list);

	/* allow previously defined functions to be looked up */
	global_function_list = func;
	advance(); /* skip ( */
	advance(); /* skip ) */

	emit_out(mks(":FUNCTION_"));
	emit_out(get_s(func));
	emit_out(mks("\n"));

	statement();

	emit_out(mks("ret\n"));
}

function program() {
	var new_type;
	var tmp;
	func = NULL;

	new_type = 1;
	while(eq(new_type, 1)) {
		new_type = 0;
		/* Deal with garbage input */
		if (eq(NULL, global_token)) {
			return;
		}

		advance();

		/* Add to global symbol table (tmp since we only want non-fns)*/
		tmp = sym_declare(global_token_string(), global_symbol_list);
		advance();

		/* Deal with global variables */
		if(match(mks(";"), global_token_string())) {
			global_symbol_list = tmp;
			globals_list = emit(mks(":GLOBAL_"), globals_list);
			globals_list = emit(get_s(get_prev(global_token)),
						globals_list);
			globals_list = emit(mks("\n"), globals_list);
			globals_list = emit(mks("NULL\n"), globals_list);
			advance();
			new_type = 1;
		}

		/* Deal with global functions */
		if(match(mks("("), global_token_string())) {
			declare_function();
			new_type = 1;
		}
	}

	/* Everything else is just an error */
	exit(add(EXIT_FAILURE, 3));
}

function recursive_output(head, out) {
	var i;
	i = reverse_list(head);
	while(neq(NULL, i)) {
		fputs(get_s(i), out);
		i = get_next(i);
	}
}

function initialize_globals() {
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

}

function main(argc, argv) {
	var inp;
	var destination_file;
	var name;

	init_support();
	initialize_globals();

	hold_string = calloc(add(MAX_STRING, 4), 1);

	name = ri32(add(argv, 4));

	inp = fopen(name, mks("r"));
	global_token = read_all_tokens(inp, global_token);
	fclose(inp);

	destination_file = fopen(ri32(add(argv,mul(4, 2))), mks("w"));

	global_token = reverse_list(global_token);

	reset_hold_string();
	output_list = NULL;
	program();

	/* Output the program we have compiled */
	fputs(mks("\n# Core program\n"), destination_file);
	recursive_output(output_list, destination_file);
	fputs(mks("\n# Program global variables\n"), destination_file);
	recursive_output(globals_list, destination_file);
	fputs(mks("\n:ELF_end\n"), destination_file);

	fclose(destination_file);
	return EXIT_SUCCESS;
}
