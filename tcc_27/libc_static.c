/* -*-comment-start: "//";comment-end:""-*-
 * GNU Mes --- Maxwell Equations of Software
 * Copyright © 2017,2018,2019,2023 Jan (janneke) Nieuwenhuizen <janneke@gnu.org>
 * Copyright © 2021 Paul Dersey <pdersey@gmail.com>
 *
 * This file is part of GNU Mes.
 *
 * GNU Mes is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * GNU Mes is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Mes.  If not, see <http://www.gnu.org/licenses/>.
 */

int stdin=0;
int stdout=1;
int stderr=2;

int main (int argc, char *argv[], char *envp[]);

void* malloc(int size);

// *INDENT-OFF*
void
_start ()
{
  asm (
       "mov     %ebp,%eax\n\t"
       "add     $4,%eax\n\t"
       "mov     (%eax),%eax\n\t"
       "add     $3,%eax\n\t"
       "shl     $2,%eax\n\t"
       "add     %ebp,%eax\n\t"
       "push    %eax\n\t"

       "mov     %ebp,%eax\n\t"
       "add     $8,%eax\n\t"
       "push    %eax\n\t"

       "mov     %ebp,%eax\n\t"
       "add     $4,%eax\n\t"
       "mov     (%eax),%eax\n\t"
       "push    %eax\n\t"

       "call    main\n\t"

       "mov     %eax,%ebx\n\t"
       "mov     $1,%eax\n\t"
       "int     $0x80\n\t"
       "hlt     \n\t"
       );
}

#define SYS_write  "0x04"

// *INDENT-OFF*
int
_write (int filedes, void const *buffer, int size)
{
  long r;
  asm (
       "mov    $"SYS_write",%%eax\n\t"
       "mov    %1,%%ebx\n\t"
       "mov    %2,%%ecx\n\t"
       "mov    %3,%%edx\n\t"
       "int    $0x80\n\t"
       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "rm" (filedes), "rm" (buffer), "rm" (size)
       : "eax", "ebx", "ecx", "edx"
       );
  return r;
}

#define SYS_exit   "0x01"

// *INDENT-OFF*
void
exit (int code)
{
  asm (
       "mov    $"SYS_exit",%%eax\n\t"
       "mov    %0,%%ebx\n\t"
       "int    $0x80\n\t"
       : // no outputs "=" (r)
       : "rm" (code)
       : "eax", "ebx"
       );
  // not reached
  // _exit (0); // tcc has a problem with this
}


int
fputc (int c, int fd)
{
  char *p = &c;
  _write (fd, p, 1);
  return 0;
}

void fputs(char* s, int f)
{
        while(0 != s[0])
        {
                fputc(s[0], f);
                s = s + 1;
        }
}

void* calloc(int count, int size)
{
        void* ret = malloc(count * size);
        if(0 == ret) return 0;
        memset(ret, 0, (count * size));
        return ret;
}

int read(void){
  puts("read not impl");
  exit(1);
}

int fopen(void){
  puts("fopen not impl");
  exit(1);
}

int fclose(void){
  puts("fclose not impl");
  exit(1);
}

void free(void* l)
{
        return;
}

long
_sys_call1 (long sys_call, long one)
{
  long r;
  asm (
       "mov    %1,%%eax\n\t"
       "mov    %2,%%ebx\n\t"
       "int    $0x80\n\t"
       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "rm" (sys_call), "rm" (one)
       : "eax", "ebx"
       );
  return r;
}

long
_sys_call3 (long sys_call, long one, long two, long three)
{
  long r;
  asm (
       "mov    %2,%%ebx\n\t"
       "mov    %3,%%ecx\n\t"
       "mov    %4,%%edx\n\t"
       "mov    %1,%%eax\n\t"
       "int    $0x80\n\t"
       "mov    %%eax,%0\n\t"
       : "=r" (r)
       : "rm" (sys_call), "rm" (one), "rm" (two), "rm" (three)
       : "eax", "ebx", "ecx", "edx"
       );
  return r;
}

#define SYS_brk     0x2d

long
brk (void *addr)
{
  long long_addr = addr;
  return _sys_call1 (SYS_brk, long_addr);
}

long _malloc_ptr;
long _brk_ptr;

void* malloc(int size)
{
        /* align malloc to 4 bytes */
        size = 4+ (size & (~3));
        if(0 == _brk_ptr)
        {
                _brk_ptr = brk(0);
                _malloc_ptr = _brk_ptr;
        }

        if(_brk_ptr < _malloc_ptr + size)
        {
                _brk_ptr = brk(_malloc_ptr + size);
                if(-1 == _brk_ptr) return 0;
        }

        long old_malloc = _malloc_ptr;
        _malloc_ptr = _malloc_ptr + size;
        if( (old_malloc & 3) !=0) {
                puts("misalligned malloc");
                exit(1);
        }
        return old_malloc;
}

int realloc(int ptr, int size) {
  int r;
  r=malloc(size);
  if(ptr!=0) {
    memcpy(r, ptr, size);
    free(ptr);
  }
  return r;
}


int close(void){
  puts("close not impl");
  exit(1);
}

#define SYS_open    0x05

int
open (char *file_name, int flags, int mask)
{
  int r = _sys_call3 (SYS_open, file_name, flags, mask);
  return r;
}
