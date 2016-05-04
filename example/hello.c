#include <stdio.h>
#include <stdint.h>

#include "../src/clo.h"

int main(int argc, char **argv)
{
  char buffer[256];
  clo o;
  int e;

  o = clo_object({"hello", clo_string(argc >= 2 ? argv[1] : "world")}, {"arguments", clo_number(argc - 1)});
  e = clo_encode(&o, buffer, sizeof buffer);
  (void) puts(e == 0 ? buffer : "error");
}
