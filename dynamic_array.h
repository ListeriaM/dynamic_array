/* dynamic_array - v3.1 - public domain dynamic array implementation

   DOCUMENTATION
     (usage: see provided examples)

     DynamicArray(T)
       type of a dynamic array of T

     DA_INIT
       zero value for the dynamic array

     da_from_parts(items, count, capacity)
       return a dynamic array with the given initial values.
       freeing the dynamic array will free (DA_FREE) with the provided buffer.
       appending to the dynamic array beyond the provided capacity will
       reallocate (DA_REALLOC) the provided `items' buffer.

     da_with_capacity(T, ctx, capacity) - uses DA_MALLOC
       return a dynamic array with the given initial capacity

     da_append(ctx, da, item) - uses DA_REALLOC
       append an item to the dynamic array and
       return the value of the passed in item

     da_append_many(ctx, da, items, count) - uses DA_REALLOC
       append `count' items from the provided buffer to the dynamic array
       unlike the other macros, this is a statement, not an expression

     da_pop(da)
       remove the last element in the dynamic array and return it as an rvalue

     da_pop_or(da, expr)
       remove the last element in the dynamic array and return it as an rvalue
       or evaluate the given expression and return it if the array is empty
       does not evaluate `expr' if the array is not empty
       (the provided expression must evaluate to a value of type T)

     da_memdup(ctx, da) - uses DA_MALLOC
       allocate a copy of the data contained in the dynamic array

     da_free(ctx, da) - uses DA_FREE
       free the memory allocated by the dynamic array

     StringBuilder (DynamicArray specialization for `char': DynamicArray(char))
       may be disabled by defining DA_NO_STRING_BUILDER.
       there is a corresponding sb_* macro for every da_* macro plus the
       following definitions:

     sb_with_capacity(ctx, cap)   -> da_with_capacity(char, ctx, cap)
     sb_append_null(ctx, sb)      -> da_append(ctx, sb, '\0')
     sb_append_cstr(ctx, sb, str) -> da_append_many(ctx, sb, str, strlen(str))

     sb_strdup(ctx, sb) - uses DA_MALLOC
       allocates a null-terminated copy of the built string and returns it

   LICENSE

     Placed in the public domain and also MIT licensed.
     See end of file for detailed license information.

   CREDITS

     Listeria monocytogenes
*/

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#define DA_VERSION_MAJOR 3
#define DA_VERSION_MINOR 1

/** Example (running total) */
#if 0
#include <stdio.h>
#include <stdlib.h>

#include "dynamic_array.h"

/* some convenience macros because we're using libc (malloc, realloc, free) */
#define da_make(T, cap)   da_with_capacity(T,, cap)
#define da_push(da, item) da_append(, da, item)
#define da_delete(da)     da_free(, da)

int main(int argc, char *argv[])
{
    DynamicArray(int) da = DA_INIT;
    int sum = 0;

    for (int i = 1; i < argc; i++)
        da_push(&da, atoi(argv[i]));

    /* running total */
    fputs("[", stdout);
    for (size_t i = 0; i < da.count; i++)
        printf("%s%d", i == 0 ? "" : ", ", sum += da.items[i]);
    fputs("]\n", stdout);

    /* safely pop at most 10 values, missing elements default to -1 */
    for (size_t i = 0; i < 10; i++)
        printf("%s%d", i == 0 ? "[" : ", ", da_pop_or(&da, -1));
    fputs("]\n", stdout);

    da_delete(&da);
    return 0;
}
#endif

/** Example (sort lines) */
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_array.h"

/* some convenience macros because we're using libc (malloc, realloc, free) */
#define da_make(T, cap)   da_with_capacity(T,, cap)
#define da_push(da, item) da_append(, da, item)
#define da_delete(da)     da_free(, da)

/* straight from qsort(3) */
static int
cmpstringp(const void *p1, const void *p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference. */

    return strcmp(*(const char **) p1, *(const char **) p2);
}

int main(int argc, char *argv[])
{
    typedef char *T;

    /* will fit 64 elements before needing to reallocate, we could also use
     * DA_INIT, this is meant to show how da_with_capacity() can be used */
    DynamicArray(T) da = da_make(T, 64);
    char *line = NULL;
    size_t n = 0;
    ssize_t nread;

    while ((nread = getline(&line, &n, stdin)) > 0)
        da_push(&da, strdup(line));

    qsort(da.items, da.count, sizeof(T), cmpstringp);

    for (size_t i = 0; i < da.count; i++) {
        fputs(da.items[i], stdout);
        free(da.items[i]);
    }

    da_delete(&da);
    free(line);

    return 0;
}
#endif

/** Example (custom allocators) */
#if 0
#define DA_MALLOC(ctx, sz)                    my_alloc(ctx, sz)
#define DA_REALLOC(ctx, oldptr, oldsz, newsz) my_realloc(ctx, oldptr, oldsz, newsz)
#define DA_FREE(ctx, ptr, sz)                 my_free(ctx, ptr, sz)
#include "dynamic_array.h"

...
#endif

#if defined(DA_MALLOC) || defined(DA_REALLOC) || defined(DA_FREE)
# if !defined(DA_MALLOC) || !defined(DA_REALLOC) || !defined(DA_FREE)
#  error "You must define DA_MALLOC, DA_REALLOC and DA_FREE, or none of them."
# endif
#else
# define DA_MALLOC(ctx, sz)                    malloc(sz)
# define DA_REALLOC(ctx, oldptr, oldsz, newsz) realloc(oldptr, newsz)
# define DA_FREE(ctx, ptr, sz)                 free(ptr)
#endif

/* More standard library stuff */
#ifndef DA_MEMSET
# define DA_MEMSET(s, c, n) memset((s), (c), (n))
#endif
#ifndef DA_MEMCPY
# define DA_MEMCPY(dst, src, n) memcpy((dst), (src), (n))
#endif
#ifndef DA_STRLEN
# define DA_STRLEN(s) strlen((s))
#endif

#ifndef DA_INIT_CAPACITY
# define DA_INIT_CAPACITY 16
#endif

/* type of count and capacity fields */
#ifdef DA_SIZE_T
typedef DA_SIZE_T da_size;
#else
typedef size_t da_size;
#endif

/* Field names */
#ifndef DA_ITEMS_FIELD
# define DA_ITEMS_FIELD items
#endif

#ifndef DA_COUNT_FIELD
# define DA_COUNT_FIELD count
#endif

#ifndef DA_CAPACITY_FIELD
# define DA_CAPACITY_FIELD capacity
#endif

/* Definitions */
#define DynamicArray(T) struct {                                              \
    T *DA_ITEMS_FIELD;                                                        \
    da_size DA_COUNT_FIELD;                                                   \
    da_size DA_CAPACITY_FIELD;                                                \
}

#define DA_INIT da_from_parts(0, 0, 0)

#define da_with_capacity(T, ctx, capacity)                                    \
    da_from_parts((T*)DA_MALLOC((ctx), (capacity) * sizeof(T)), 0, capacity)

#define da_from_parts(items, count, capacity) { (items), (count), (capacity) }

#if defined(__cplusplus) && defined(__cpp_decltype)
# define DA__CAST(T) (decltype(T))
#else
# define DA__CAST(T)
#endif

#define da_append(ctx, da, item)                                              \
    (((da)->DA_COUNT_FIELD == (da)->DA_CAPACITY_FIELD ?                       \
     (da)->DA_ITEMS_FIELD = DA__CAST((da)->DA_ITEMS_FIELD)DA_REALLOC(         \
         (ctx),                                                               \
         (da)->DA_ITEMS_FIELD,                                                \
         sizeof(*(da)->DA_ITEMS_FIELD) * (da)->DA_CAPACITY_FIELD,             \
         sizeof(*(da)->DA_ITEMS_FIELD) * ((da)->DA_CAPACITY_FIELD > 0 ?       \
                                          (da)->DA_CAPACITY_FIELD * 2 :       \
                                          DA_INIT_CAPACITY)),                 \
     (da)->DA_CAPACITY_FIELD = ((da)->DA_CAPACITY_FIELD > 0 ?                 \
                                (da)->DA_CAPACITY_FIELD * 2 :                 \
                                DA_INIT_CAPACITY) : 0),                       \
     (da)->DA_ITEMS_FIELD[(da)->DA_COUNT_FIELD++] = (item))

#define da_append_many(ctx, da, items, count)                                 \
    do {                                                                      \
        da_size da__count = (count);                                          \
        if ((da)->DA_COUNT_FIELD + da__count > (da)->DA_CAPACITY_FIELD) {     \
            da_size da_size__v = ((da)->DA_CAPACITY_FIELD > 0 ?               \
                                  (da)->DA_CAPACITY_FIELD * 2 :               \
                                  DA_INIT_CAPACITY);                          \
            while (da_size__v < (da)->DA_COUNT_FIELD + da__count)             \
                da_size__v *= 2;                                              \
            (da)->DA_ITEMS_FIELD = DA__CAST((da)->DA_ITEMS_FIELD)DA_REALLOC(  \
                (ctx),                                                        \
                (da)->DA_ITEMS_FIELD,                                         \
                sizeof(*(da)->DA_ITEMS_FIELD) * (da)->DA_CAPACITY_FIELD,      \
                sizeof(*(da)->DA_ITEMS_FIELD) * da_size__v);                  \
            (da)->DA_CAPACITY_FIELD = da_size__v;                             \
        }                                                                     \
        for (da_size da__i = 0; da__i < da__count; da__i++)                   \
            (da)->DA_ITEMS_FIELD[(da)->DA_COUNT_FIELD++] = (items)[da__i];    \
    } while (0)

/* convert an lvalue to an rvalue (for private use) */
#if defined(__cplusplus) && defined(__cpp_auto_cast)
# define DA__RVALUE(V) auto(V)
#else
# define DA__RVALUE(V) (1 ? (V) : /* not evaluated */ DA__CAST(V)(V))
#endif

#define da_pop(da) DA__RVALUE((da)->DA_ITEMS_FIELD[--(da)->DA_COUNT_FIELD])

#define da_pop_or(da, expr) ((da)->DA_COUNT_FIELD > 0 ? da_pop(da) : (expr))

#define da_memdup(ctx, da)                                                    \
    DA__CAST((da)->DA_ITEMS_FIELD)DA_MEMCPY(                                  \
        DA_MALLOC((ctx), sizeof(*(da)->DA_ITEMS_FIELD)*(da)->DA_COUNT_FIELD), \
        (da)->DA_ITEMS_FIELD,                                                 \
        sizeof(*(da)->DA_ITEMS_FIELD)*(da)->DA_COUNT_FIELD)

#define da_free(ctx, da)                                                      \
    DA_FREE((ctx),                                                            \
            (da)->DA_ITEMS_FIELD,                                             \
            (da)->DA_CAPACITY_FIELD * sizeof(*(da)->DA_ITEMS_FIELD))

#ifndef DA_NO_STRING_BUILDER

#ifdef DA_STRING_BUILDER_T
typedef DynamicArray(char) DA_STRING_BUILDER_T;
#else
typedef DynamicArray(char) StringBuilder;
#endif

#define SB_INIT        DA_INIT
#define sb_from_parts  da_from_parts
#define sb_append      da_append
#define sb_append_many da_append_many
#define sb_pop         da_pop
#define sb_pop_or      da_pop_or
#define sb_memdup      da_memdup
#define sb_free        da_free

#define sb_with_capacity(ctx, cap)   da_with_capacity(char, ctx, cap)
#define sb_append_null(ctx, sb)      da_append(ctx, sb, '\0')
#define sb_append_cstr(ctx, sb, str)                                          \
    da_append_many(ctx, sb, str, DA_STRLEN(str))

#define sb_strdup(ctx, sb)                                                    \
    ((char*)DA_MEMSET(                                                        \
        (char*)DA_MEMCPY(DA_MALLOC((ctx), (sb)->DA_COUNT_FIELD + 1),          \
                         (sb)->DA_ITEMS_FIELD,                                \
                         (sb)->DA_COUNT_FIELD)                                \
        + (sb)->DA_COUNT_FIELD, '\0', 1)                                      \
     - (sb)->DA_COUNT_FIELD)

#endif // DA_NO_STRING_BUILDER
#endif // DYNAMIC_ARRAY_H

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Listeria monocytogenes
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
