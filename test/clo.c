#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/clo.h"

void core()
{
  clo o;
  char out[256], small[4];
  int e;

  /* basic JSON types */
  o = clo_array(clo_string("string"), clo_number(42), clo_true(), clo_false(), clo_null(), clo_object({"key", clo_string("value")}));
  e = clo_encode(&o, out, sizeof out);
  assert_int_equal(e, 0);
  assert_string_equal(out, "[\"string\",42,true,false,null,{\"key\":\"value\"}]");

  /* object */
  o = clo_object({"k1", clo_string("v1")}, {"hidden", clo_undefined()}, {"k2", clo_string("v2")});
  e = clo_encode(&o, out, sizeof out);
  assert_int_equal(e, 0);

  /* array */
  o = clo_array(clo_string("v1"), clo_undefined(), clo_string("v2"));
  e = clo_encode(&o, out, sizeof out);
  assert_int_equal(e, 0);

  /* control characters */
  e = clo_encode((clo[]){clo_string("\x0a\"\\")}, out, sizeof out);
  assert_int_equal(e, 0);
  assert_string_equal(out, "\"\\n\\\"\\\\\"");

  /* utf-8 snowman */
  e = clo_encode((clo[]){clo_string("\xe2\x98\x83")}, out, sizeof out);
  assert_int_equal(e, 0);
  assert_string_equal(out, "\"\xe2\x98\x83\"");

  /* buffer too small */
  e = clo_encode((clo[]){clo_string("test")}, small, sizeof small);
  assert_int_equal(e, -1);
}

void internals()
{
  clo_buffer b;
  char out[256];
  int e;

  /* invalid control character */
  e = 0;
  b = (clo_buffer) {.base = out, .size = sizeof out};
  clo_encode_control(64, &b, &e);
  assert_int_equal(e, 1);

  /* empty utf8 */
  e = 0;
  (void) clo_encode_utf8("", &b, &e);
  assert_int_equal(e, 0);

  /* invalid utf8 */
  e = 0;
  (void) clo_encode_utf8("\xff", &b, &e);
  assert_int_equal(e, 1);

  /* invalid clo type */
  e = 0;
  clo_encode_clo((clo[]){{.type = CLO_END_OF_ARRAY}}, &b, &e);
  assert_int_equal(e, 1);


}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core),
    cmocka_unit_test(internals)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
