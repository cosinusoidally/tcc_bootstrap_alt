/* heavily modified version of some M2lib files modified by Liam Wilson */

/* Copyright (C) 2016 Jeremiah Orians
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

var stdin;
var stdout;
var stderr;

function fputs(si, f) {
	while(neq(0, ri8(si))) {
		fputc(ri8(si), f);
		si = add(si, 1);
	}
}

function fopen(filename, mode) {
	var f;
	if(eq(mkc('w'), ri8(mode))) {
		/* 577 is O_WRONLY|O_CREAT|O_TRUNC, 384 is 600 in octal */
		f = open(filename, 577 , 384);
	} else {
		/* Everything else is a read */
		f = open(filename, 0, 0);
	}

	/* Negative numbers are error codes */
	if(gt(0, f)) {
		return 0;
	}
	return f;
}

function fclose(stream) {
	var error;
	error = close(stream);
	return error;
}

var _malloc_ptr;
var _brk_ptr;

function malloc(size) {
	var old_malloc;
	if(eq(NULL, _brk_ptr)) {
		_brk_ptr = brk(0);
		_malloc_ptr = _brk_ptr;
	}

	if(lt(_brk_ptr, add(_malloc_ptr, size))) {
		_brk_ptr = brk(add(_malloc_ptr, size));
		if(eq(sub(0,1), _brk_ptr)) return 0;
	}

	old_malloc = _malloc_ptr;
	_malloc_ptr = add(_malloc_ptr, size);
	return old_malloc;
}

function memset(ptr, value, num) {
	var s;
	s = ptr;
	while(lt(0, num))
	{
		wi8(s, value);
		s = add(s, 1);
		num = sub(num, 1);
	}
}

function calloc(count, size) {
	var ret;
	ret = malloc(mul(count, size));
	if(eq(NULL, ret)) {
		return NULL;
	}
	memset(ret, 0, mul(count, size));
	return ret;
}

function free(l) {
	return;
}

function init_support(){
	stdin = 0;
	stdout = 1;
	stderr = 2;
}
