# Embedded MCU Tools
Tools for MCU on which a large flash space is unavailable.

Containing the following contents:

- `em_sprintf`
- global.awk

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

This function only occupies 2812 bytes on flash!!

```bash
$ arm-none-eabi-gcc --version
arm-none-eabi-gcc (GNU Tools for Arm Embedded Processors 7-2017-q4-major) 7.2.1 20170904 (release) [ARM/embedded-7-branch revision 255204]
Copyright (C) 2017 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

$ arm-none-eabi-size em_sprintf.o
   text    data     bss     dec     hex filename
   2792       0      20    2812     afc em_sprintf.o
```

# global.awk
This is a `awk` script for **making class instance declarations and definitions** for a header.



## 1. Preparation
For example, make a text file like the one below, named "globalList.txt":

```cpp
std::tuple<int, int> tpl;
double *list[100];
char **bufp; // pointer for uart buffer

// kinetic parameters
float speed;
float distance;
```

## 2. Generating the header file
And then execute this command to get the lists.

```bash
$ awk -f global.awk -v macrofile=macro.hpp globalList.txt > global.hpp
```

## 3. Making another header
`global.hpp`, which is generated in `2.` consists of `_global_instance()` and `_export_instance()` macros. `global.awk` is helped by these macros.

When you use this file, you need to make this file named "macro.hpp".

```cpp
#include <new>
#define _global_instance(name, type) \
    type *name; \
    char buf_##name[sizeof(type)]

#define _place_instance(name, obj) \
        do{name = new(buf_##name) obj;}while(0)

#define _export_instance(name, type) \
    extern type *name; \
    extern char buf_##name[sizeof(type)]
```

Then include this file in your `main.cpp`.
