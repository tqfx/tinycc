/*  tccdefs.h
 *
 *  By using native platf�rm macros this file may be included at runtime
 *  just as is.
 *
 *  If converted to C-strings and included in tccpp.c, these are trahslated
 *  to tcc target macros accordingly.
 */

#if __SIZEOF_POINTER__ == 4
    /* 32bit systems. */
    #define __SIZE_TYPE__ unsigned int
    #define __PTRDIFF_TYPE__ int
    #define __ILP32__ 1
#elif __SIZEOF_LONG__ == 4
    /* 64bit Windows. */
    #define __SIZE_TYPE__ unsigned long long
    #define __PTRDIFF_TYPE__ long long
    #define __LLP64__ 1
#else
    /* Other 64bit systems. */
    #define __SIZE_TYPE__ unsigned long
    #define __PTRDIFF_TYPE__ long
    #define __LP64__ 1
#endif
    #define __SIZEOF_INT__ 4
    #define __INT_MAX__ 0x7fffffff
#if __SIZEOF_LONG__ == 4
    #define __LONG_MAX__ 0x7fffffffL
#else
    #define __LONG_MAX__ 0x7fffffffffffffffL
#endif
    #define __SIZEOF_LONG_LONG__ 8
    #define __LONG_LONG_MAX__ 0x7fffffffffffffffLL
    #define __CHAR_BIT__ 8
    #define __ORDER_LITTLE_ENDIAN__ 1234
    #define __ORDER_BIG_ENDIAN__ 4321
    #define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#if defined _WIN32
    #define __WCHAR_TYPE__ unsigned short
    #define __WINT_TYPE__ unsigned short
#elif defined __linux__
    #define __WCHAR_TYPE__ int
    #define __WINT_TYPE__ unsigned int
#else
    #define __WCHAR_TYPE__ int
    #define __WINT_TYPE__ int
#endif

    #if __STDC_VERSION__ == 201112L
    # define __STDC_NO_ATOMICS__ 1
    # define __STDC_NO_COMPLEX__ 1
    # define __STDC_NO_THREADS__ 1
#if !defined _WIN32
    # define __STDC_UTF_16__ 1
    # define __STDC_UTF_32__ 1
#endif
    #endif

#if defined _WIN32
    //#define _WIN32 1
# if __SIZEOF_POINTER__ == 8
    #define _WIN64 1
# endif
    #define __declspec(x) __attribute__((x))
    #define __cdecl

#elif defined __FreeBSD__
    //#define __FreeBSD__ 12
    #define __GNUC__ 9
    #define __GNUC_MINOR__ 3
    #define __GNUC_PATCHLEVEL__ 0
    #define __GNUC_STDC_INLINE__ 1
    #define __NO_TLS 1
# if __SIZEOF_POINTER__ == 8
    /* FIXME, __int128_t is used by setjump */
    #define __int128_t struct { unsigned char _dummy[16] __attribute((aligned(16))); }
# endif

#elif defined __FreeBSD_kernel__
    //#define __FreeBSD_kernel__ 1

#elif defined __NetBSD__
    //#define __NetBSD__ 1
    #define __GNUC__ 4
    #define __GNUC_MINOR__ 0
    #define __GNUC_PATCHLEVEL__ 0
    #define _Pragma(x)
    #define __ELF__ 1

#elif defined __OpenBSD__
    //#define __OpenBSD__ 1
    #define __GNUC__ 4
    #define _ANSI_LIBRARY 1

#elif defined __APPLE__
    //#define __APPLE__ 1
    /* emulate APPLE-GCC to make libc's headerfiles compile: */
    #define __GNUC__ 4   /* darwin emits warning on GCC<4 */
    #define __APPLE_CC__ 1 /* for <TargetConditionals.h> */
    #define _DONT_USE_CTYPE_INLINE_ 1
    /* avoids usage of GCC/clang specific builtins in libc-headerfiles: */
    #define __FINITE_MATH_ONLY__ 1
    #define _FORTIFY_SOURCE 0

#else
    //#define __linux__ 1
    //#define __linux 1
#endif

#if !defined _WIN32
    /* glibc defines */
    #define __REDIRECT(name, proto, alias) name proto __asm__ (#alias)
    #define __REDIRECT_NTH(name, proto, alias) name proto __asm__ (#alias) __THROW
#endif

    /* skip __builtin... with -E */
    #ifndef __TCC_PP__

    #define __builtin_offsetof(type, field) ((__SIZE_TYPE__)&((type*)0)->field)
    #define __builtin_extract_return_addr(x) x
#if !defined __linux__ && !defined _WIN32
    /* used by math.h */
    #define __builtin_huge_val() 1e500
    #define __builtin_huge_valf() 1e50f
    #define __builtin_huge_vall() 1e5000L
# if defined __APPLE__
    #define __builtin_nanf(ignored_string) __nan()
    /* used by floats.h to implement FLT_ROUNDS C99 macro. 1 == to nearest */
    #define __builtin_flt_rounds() 1
    /* used by _fd_def.h */
    #define __builtin_bzero(p, ignored_size) bzero(p, sizeof(*(p)))
# else
    #define __builtin_nanf(ignored_string) (0.0F/0.0F)
# endif
#endif

    /* __builtin_va_list */
#if defined __x86_64__
#if !defined _WIN32
    /* GCC compatible definition of va_list. */
    /* This should be in sync with the declaration in our lib/libtcc1.c */
    typedef struct {
        unsigned gp_offset, fp_offset;
        union {
            unsigned overflow_offset;
            char *overflow_arg_area;
        };
        char *reg_save_area;
    } __builtin_va_list[1];

    void *__va_arg(__builtin_va_list ap, int arg_type, int size, int align);
    #define __builtin_va_start(ap, last) \
       (*(ap) = *(__builtin_va_list)((char*)__builtin_frame_address(0) - 24))
    #define __builtin_va_arg(ap, t)   \
       (*(t *)(__va_arg(ap, __builtin_va_arg_types(t), sizeof(t), __alignof__(t))))
    #define __builtin_va_copy(dest, src) (*(dest) = *(src))

#else /* _WIN64 */
    typedef char *__builtin_va_list;
    #define __builtin_va_arg(ap, t) ((sizeof(t) > 8 || (sizeof(t) & (sizeof(t) - 1))) \
        ? **(t **)((ap += 8) - 8) : *(t  *)((ap += 8) - 8))
#endif

#elif defined __arm__
    typedef char *__builtin_va_list;
    #define _tcc_alignof(type) ((int)&((struct {char c;type x;} *)0)->x)
    #define _tcc_align(addr,type) (((unsigned)addr + _tcc_alignof(type) - 1) \
                                  & ~(_tcc_alignof(type) - 1))
    #define __builtin_va_start(ap,last) (ap = ((char *)&(last)) + ((sizeof(last)+3)&~3))
    #define __builtin_va_arg(ap,type) (ap = (void *) ((_tcc_align(ap,type)+sizeof(type)+3) \
                           &~3), *(type *)(ap - ((sizeof(type)+3)&~3)))

#elif defined __aarch64__
    typedef struct {
        void *__stack, *__gr_top, *__vr_top;
        int   __gr_offs, __vr_offs;
    } __builtin_va_list;

#elif defined __riscv
    typedef char *__builtin_va_list;
    #define __va_reg_size (__riscv_xlen >> 3)
    #define _tcc_align(addr,type) (((unsigned long)addr + __alignof__(type) - 1) \
                                  & -(__alignof__(type)))
    #define __builtin_va_arg(ap,type) (*(sizeof(type) > (2*__va_reg_size) ? *(type **)((ap += __va_reg_size) - __va_reg_size) : (ap = (va_list)(_tcc_align(ap,type) + (sizeof(type)+__va_reg_size - 1)& -__va_reg_size), (type *)(ap - ((sizeof(type)+ __va_reg_size - 1)& -__va_reg_size)))))

#else /* __i386__ */
    typedef char *__builtin_va_list;
    #define __builtin_va_start(ap,last) (ap = ((char *)&(last)) + ((sizeof(last)+3)&~3))
    #define __builtin_va_arg(ap,t) (*(t*)((ap+=(sizeof(t)+3)&~3)-((sizeof(t)+3)&~3)))

#endif
    #define __builtin_va_end(ap) (void)(ap)
    #ifndef __builtin_va_copy
    # define __builtin_va_copy(dest, src) (dest) = (src)
    #endif

    /* TCC BBUILTIN AND BOUNDS ALIASES */
    #ifdef __leading_underscore
    # define __RENAME(X) __asm__("_"X)
    #else
    # define __RENAME(X) __asm__(X)
    #endif
    #ifdef __BOUNDS_CHECKING_ON
    # define __BUILTINBC(ret,name,params) ret __builtin_##name params __RENAME("__bound_"#name);
    # define __BOUND(ret,name,params) ret name params __RENAME("__bound_"#name);
    #else
    # define __BUILTINBC(ret,name,params) ret __builtin_##name params __RENAME(#name);
    # define __BOUND(ret,name,params)
    #endif
    #define __BOTH(ret,name,params) __BUILTINBC(ret,name,params)__BOUND(ret,name,params)
    #define __BUILTIN(ret,name,params) ret __builtin_##name params __RENAME(#name);

    __BOTH(void*, memcpy, (void *, const void*, __SIZE_TYPE__))
    __BOTH(void*, memmove, (void *, const void*, __SIZE_TYPE__))
    __BOTH(void*, memset, (void *, int, __SIZE_TYPE__))
    __BOTH(int, memcmp, (const void *, const void*, __SIZE_TYPE__))
    __BOTH(__SIZE_TYPE__, strlen, (const char *))
    __BOTH(char*, strcpy, (char *, const char *))
    __BOTH(char*, strncpy, (char *, const char*, __SIZE_TYPE__))
    __BOTH(int, strcmp, (const char*, const char*))
    __BOTH(int, strncmp, (const char*, const char*, __SIZE_TYPE__))
    __BOTH(char*, strcat, (char*, const char*))
    __BOTH(char*, strchr, (const char*, int))
    __BOTH(char*, strdup, (const char*))
#if defined __ARM_EABI__
    __BOUND(void*,__aeabi_memcpy,(void*,const void*,__SIZE_TYPE__))
    __BOUND(void*,__aeabi_memmove,(void*,const void*,__SIZE_TYPE__))
    __BOUND(void*,__aeabi_memmove4,(void*,const void*,__SIZE_TYPE__))
    __BOUND(void*,__aeabi_memmove8,(void*,const void*,__SIZE_TYPE__))
    __BOUND(void*,__aeabi_memset,(void*,int,__SIZE_TYPE__))
#endif

#if defined __linux__ || defined __APPLE__ // HAVE MALLOC_REDIR
    #define __MAYBE_REDIR __BUILTIN
#else
    #define __MAYBE_REDIR __BOTH
#endif
    __MAYBE_REDIR(void*, malloc, (__SIZE_TYPE__))
    __MAYBE_REDIR(void*, realloc, (void *, __SIZE_TYPE__))
    __MAYBE_REDIR(void*, calloc, (__SIZE_TYPE__, __SIZE_TYPE__))
    __MAYBE_REDIR(void*, memalign, (__SIZE_TYPE__, __SIZE_TYPE__))
    __MAYBE_REDIR(void, free, (void*))
#if defined __i386__ || defined __x86_64__
    __BOTH(void*, alloca, (__SIZE_TYPE__))
#else
    __BUILTIN(void*, alloca, (__SIZE_TYPE__))
#endif
    __BUILTIN(void, abort, (void))
    __BOUND(void, longjmp, ())
#if !defined _WIN32
    __BOUND(void*, mmap, ())
    __BOUND(int, munmap, ())
#endif
    #undef __BUILTINBC
    #undef __BUILTIN
    #undef __BOUND
    #undef __BOTH
    #undef __MAYBE_REDIR
    #undef __RENAME

    #endif /* ndef __TCC_PP__ */