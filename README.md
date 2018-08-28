# `em_sprintf` - **em**bedded **sprintf**

is a **compact version** of `sprintf`. This function is suitable especially for MCUs with a small capacity.

## Supporting Formats
- `%d` `%u` `%o` `%x` `%X`: Integer
- `%ld` `%lu` `%lo` `%lx` `%lX`: Long Integer
- `%p`: Pointer
- `%c` `%s`: Character(s)
- `%f` `%e` `%E`: Floating-point Number

## Supporting Flags
- `0`
- `+`
- `-`
- ` `

Field width and Precision are also supported.

```
%6.2f
```

## Requirements
- C99
- floating point support(including software implementation)

\* ) `stdio.h` and `math.h` is not needed at all!!

## How to Use
Just to run the main program on `em_sprintf.c`, execute:

```bash
$ gcc -D_EM_SPRINTF_MAIN em_sprintf.c
$ ./a.out
```

Or you can include `em_sprintf` into your project:

```c
__attribute__((format(printf, 2, 3)))
int em_sprintf(char *buf, const char *fmt, ...); // Add in your header

// in some functions
char buf[50];
em_sprintf(buf, "%f", ad_data);
```

