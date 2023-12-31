/* attribute identifiers */
DEF(TOK_SECTION, "section")
DEF(TOK___SECTION__, "__section__")
DEF(TOK_ALIGNED, "aligned")
DEF(TOK___ALIGNED__, "__aligned__")
DEF(TOK_UNUSED, "unused")
DEF(TOK___UNUSED__, "__unused__")
DEF(TOK_CDECL, "cdecl")
DEF(TOK___CDECL, "__cdecl")
DEF(TOK___CDECL__, "__cdecl__")
DEF(TOK_STDCALL, "stdcall")
DEF(TOK___STDCALL, "__stdcall")
DEF(TOK___STDCALL__, "__stdcall__")
DEF(TOK_NORETURN, "noreturn")
DEF(TOK___NORETURN__, "__noreturn__")

/* builtin functions or variables */
DEF(TOK_memcpy, "memcpy")
DEF(TOK_memset, "memset")
DEF(TOK___divdi3, "__divdi3")
DEF(TOK___moddi3, "__moddi3")
DEF(TOK___udivdi3, "__udivdi3")
DEF(TOK___umoddi3, "__umoddi3")
DEF(TOK___sardi3, "__sardi3")
DEF(TOK___shrdi3, "__shrdi3")
DEF(TOK___shldi3, "__shldi3")
DEF(TOK___tcc_int_fpu_control, "__tcc_int_fpu_control")
DEF(TOK___tcc_fpu_control, "__tcc_fpu_control")
DEF(TOK___ulltof, "__ulltof")
DEF(TOK___ulltod, "__ulltod")
DEF(TOK___ulltold, "__ulltold")
DEF(TOK___fixunssfdi, "__fixunssfdi")
DEF(TOK___fixunsdfdi, "__fixunsdfdi")
DEF(TOK___fixunsxfdi, "__fixunsxfdi")

/* bound checking symbols */
#ifdef CONFIG_TCC_BCHECK
DEF(TOK___bound_ptr_add, "__bound_ptr_add")
DEF(TOK___bound_ptr_indir1, "__bound_ptr_indir1")
DEF(TOK___bound_ptr_indir2, "__bound_ptr_indir2")
DEF(TOK___bound_ptr_indir4, "__bound_ptr_indir4")
DEF(TOK___bound_ptr_indir8, "__bound_ptr_indir8")
DEF(TOK___bound_ptr_indir12, "__bound_ptr_indir12")
DEF(TOK___bound_ptr_indir16, "__bound_ptr_indir16")
DEF(TOK___bound_local_new, "__bound_local_new")
DEF(TOK___bound_local_delete, "__bound_local_delete")
DEF(TOK_malloc, "malloc")
DEF(TOK_free, "free")
DEF(TOK_realloc, "realloc")
DEF(TOK_memalign, "memalign")
DEF(TOK_calloc, "calloc")
DEF(TOK_memmove, "memmove")
DEF(TOK_strlen, "strlen")
DEF(TOK_strcpy, "strlen")
#endif
