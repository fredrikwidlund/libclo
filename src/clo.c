#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dynamic.h>

#include "clo.h"

// clo_decode_utf8 from http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
uint32_t clo_decode_utf8(uint32_t *state, uint32_t *codep, uint32_t byte)
{
  static const uint8_t utf8d[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,
    0,12,24,36,60,96,84,12,12,12,48,72,12,12,12,12,12,12,12,12,12,12,12,12,
    12,0,12,12,12,12,12,0,12,0,12,12,12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12,12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12,12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12
  };
  uint32_t type = utf8d[byte];

  *codep = (*state != CLO_DECODE_UTF8_ACCEPT) ?
    (byte & 0x3fu) | (*codep << 6) :
    (0xff >> type) & (byte);

  *state = utf8d[256 + *state + type];
  return *state;
}

void clo_encode_append(buffer *b, char *string, int *error)
{
  int e;

  e = buffer_insert(b, buffer_size(b), string, strlen(string));
  *error += (e != 0);
}

void clo_encode_control(buffer *b, uint8_t c, int *error)
{
  static const char *(control[32]) = {
    "\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
    "\\b"    , "\\t"    , "\\n"    , "\\u000b", "\\f"    , "\\r"    , "\\u000e", "\\u000f",
    "\\u0010", "\\u0011", "\\u0012", "\\u0013", "\\u0014", "\\u0015", "\\u0016", "\\u0017",
    "\\u0018", "\\u0019", "\\u001a", "\\u001b", "\\u001c", "\\u001d", "\\u001e", "\\u001f",
  };
  if (c < 32)
    clo_encode_append(b, (char *) control[c], error);
  else
    (*error) ++;
}

int clo_encode_utf8(buffer *b, char *string, int *error)
{
  uint32_t codepoint, state = 0;
  int i = 0;

  while (string[i])
    {
      (void) clo_decode_utf8(&state, &codepoint, (uint8_t) string[i]);
      i ++;
      if (state == CLO_DECODE_UTF8_ACCEPT || state == CLO_DECODE_UTF8_REJECT)
        break;
    }

  if (state == CLO_DECODE_UTF8_ACCEPT)
    buffer_insert(b, buffer_size(b), string, i);
  else
    (*error) ++;
  return i;
}

void clo_encode_string(buffer *b, char *string, int *error)
{
  char *data;

  clo_encode_append(b, "\"", error);
  while (1)
    {
      buffer_reserve(b, buffer_size(b) + strlen(string));
      data = buffer_data(b) + buffer_size(b);
      while (*string >= 0x20 && *string != '"' && *string != '\\')
        {
          *data = *string;
          data ++;
          string ++;
        }
      b->size = data - buffer_data(b);
      if (*string < 0)
        string += clo_encode_utf8(b, string, error);
      else if (!*string)
        break;
      else if (*string < 0x20)
        {
          clo_encode_control(b, *string, error);
          string ++;
        }
      else if (*string == '"')
        {
          clo_encode_append(b, "\\\"", error);
          string ++;
        }
      else if (*string == '\\')
        {
          clo_encode_append(b, "\\\\", error);
          string ++;
        }
      else
        {
          (*error) ++;
          return;
        }
    }
  clo_encode_append(b, "\"", error);
}

void clo_encode_number(buffer *b, double number, int *error)
{
  char string[32];
  int e;

  e = snprintf(string, sizeof string, "%.16g", number);
  if (e > 0 && e < (int) sizeof string)
    clo_encode_append(b, string, error);
  else
    (*error) ++;
}

void clo_encode(clo *o, buffer *b, int *error)
{
  switch (o->type)
    {
    case CLO_STRING:
      clo_encode_string(b, o->string, error);
      break;
    case CLO_NUMBER:
      clo_encode_number(b, o->number, error);
      break;
    case CLO_OBJECT:
      clo_encode_append(b, "{", error);
      for (clo_pair *p = o->object; p->string; p ++)
        {
          clo_encode_string(b, p->string, error);
          clo_encode_append(b, ":", error);
          clo_encode(&p->value, b, error);
          if (p[1].string)
            clo_encode_append(b, ",", error);
        }
      clo_encode_append(b, "}", error);
      break;
    case CLO_ARRAY:
      clo_encode_append(b, "[", error);
      for (clo *e = o->array; e->type != CLO_UNDEFINED; e ++)
        {
          clo_encode(e, b, error);
          if (e[1].type != CLO_UNDEFINED)
            clo_encode_append(b, ",", error);
        }
      clo_encode_append(b, "]", error);
      break;
    case CLO_TRUE:
      clo_encode_append(b, "true", error);
      break;
    case CLO_FALSE:
      clo_encode_append(b, "false", error);
      break;
    case CLO_NULL:
      clo_encode_append(b, "null", error);
      break;
    }
}
