# Dynamic Array

[Dynamic Array](https://en.wikipedia.org/wiki/Dynamic_array) implementation in
pure C macros as an
[stb-style single-file library](https://github.com/nothings/stb).

## Quick Start

The library itself does not require any special building. You can simple
copy-paste [./dynamic_array.h](./dynamic_array.h) to your project and
`#include` it.

```c
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
```

## Slow Start

Read [./dynamic_array.h](./dynamic_array.h) for documentation and examples.
