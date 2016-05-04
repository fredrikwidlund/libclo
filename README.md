# libclo

Compound literal JSON objects

## Description

libclo is a C library with the single purpose of emitting serialized JSON structures in a high performance environment, while correctly handling escaping, control characters and UTF-8 encoding. This can be useful for example when using JSON for log formats, or when building RESTful services.

It does this by constructing the JSON structures as compound literals, which does not require any runtime memory allocation or system calls.

## Performance

[Simple JSON serialization benchmark](https://github.com/fredrikwidlund/libclo_benchmark)

| Library | Performance | Factor |
| --- | --- | --- |
| libclo | 378.94 ns | 1.0x |
| jansson | 2463.8 ns | 6.5x |
| cJSON | 1693.1 ns | 4.5x |

libclo is a small subset though of the functionality in cJSON, and even more so of the functionality in jansson.

## Installation
```
$ git clone https://github.com/fredrikwidlund/libclo.git
$ cd libclo
$ ./autogen.sh
$ ./configure
$ make install
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

## Usage

### Hello world
```c
$ cat hello.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <clo.h>

int main()
{
  char string[256];
  clo_encode((clo[]){clo_object({"hello", clo_string("world")})}, string, sizeof string);
  puts(string);
}
$ gcc -Wall -o hello hello.c -lclo
$ ./hello
{"hello":"world"}
```

### JSON types

```c
$ cat types.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <clo.h>

int main()
{
  char buffer[256];

  clo string = clo_string("unicode snowman - \xE2\x98\x83");
  clo real = clo_real(3.14);
  clo integer = clo_integer(42);
  clo true = clo_true();
  clo false = clo_false();
  clo null = clo_null();

  clo array = clo_array(string, real, integer, true, false, null);
  clo_encode(&array, buffer, sizeof buffer);
  puts(buffer);

  clo object = clo_object
    ({"string", string},
     {"real", real},
     {"integer", integer},
     {"true", true},
     {"false", false},
     {"null", null},
     {"array", array},
     {"nested object", clo_object({"object", clo_object({"inner", clo_true()})})});
  clo_encode(&object, buffer, sizeof buffer);
  puts(buffer);
}
$ gcc -Wall -o types types.c -lclo
$ ./types 
["unicode snowman - ☃",3.14,42,true,false,null]
{"string":"unicode snowman - ☃","real":3.14,"integer":42,"true":true,"false":false,"null":null,"array":["unicode snowman - ☃",3.14,42,true,false,null],"nested object":{"object":{"inner":true}}}
```
