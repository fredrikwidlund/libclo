# libclo
Read-only compound literal objects

## Description
libclo is a C library with the single purpose of emitting serialized JSON structures in a high performance environment, while correctly handling escaping, control characters and UTF-8 encoding. This can be useful for example when using JSON for log formats, or when building RESTful services.

It does this by constructing the JSON structures as compound literals, which does not require any runtime heap memory allocation or system calls.

## Performance
Benchmarks coming soon...

## Build
```
$ git clone https://github.com/fredrikwidlund/libclo.git
$ cd libclo
$ ./autogen.sh
$ ./configure
$ make
```

## Test
Tests require 100% code coverage to succeed.

```
$ make check
[...]
$ cat test/coverage.sh.log 
[clo]
File 'src/clo.c'
Lines executed:100.00% of 95
Branches executed:100.00% of 50
Taken at least once:100.00% of 50
Calls executed:100.00% of 28
PASS test/coverage.sh (exit status: 0)
```

## Install
```
$ make install
```

## Use
```
$ cat main.c
#include <stdio.h>
#include <stdint.h>

#include <clo.h>

int main()
{
  char buffer[1024];
  clo o;

  o = clo_object({"hello", clo_string("world")});
  clo_encode(&o, buffer, sizeof buffer);
  puts(buffer);
}
$ gcc -Wall -O3 -flto -fuse-linker-plugin -o main main.c -lclo
$ ./main 
{"hello":"world"}
```
