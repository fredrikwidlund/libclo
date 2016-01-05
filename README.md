# libclo

Read-only compound literal objects

## Description

Fast complex object C abstraction using compound literal objects, including JSON serialization

## Requirements

- libdynamic

## Build

```
git clone https://github.com/fredrikwidlund/libclo.git
cd libclo
./autogen.sh
./configure
make install
```

## Example

```
$ cat test.c
#include <stdio.h>
#include <stdint.h>
#include <dynamic.h>
#include <clo.h>

int main()
{
  buffer b;
  int e;

  buffer_init(&b);
  e = 0;
  clo_encode((clo[]){clo_object({"some", clo_string("object")})}, &b, &e);
  printf("%.*s\n", (int) buffer_size(&b), buffer_data(&b));
}
$ gcc -Wall -Wpedantic -O3 -flto -fuse-linker-plugin -o test test.c -ldynamic -lclo
$ ./test
{"some":"object"}
```
