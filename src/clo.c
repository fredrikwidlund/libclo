#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dynamic.h>

#include "clo.h"

/* clo_decode_utf8 from http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ */

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

void clo_buffer_append(clo_buffer *b, char *base, size_t size, int *error)
{
  if (size < b->size)
    {
      memcpy(b->base, base, size);
      b->base += size;
      b->size -= size;
    }
  else
    (*error) ++;
}

void clo_encode_control(uint8_t c, clo_buffer *b, int *error)
{
  static const char *(control[32]) = {
    "\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
    "\\b"    , "\\t"    , "\\n"    , "\\u000b", "\\f"    , "\\r"    , "\\u000e", "\\u000f",
    "\\u0010", "\\u0011", "\\u0012", "\\u0013", "\\u0014", "\\u0015", "\\u0016", "\\u0017",
    "\\u0018", "\\u0019", "\\u001a", "\\u001b", "\\u001c", "\\u001d", "\\u001e", "\\u001f",
  };
  if (c < 32)
    clo_buffer_append(b, (char *) control[c], strlen(control[c]), error);
  else
    (*error) ++;
}

int clo_encode_utf8(char *string, clo_buffer *b, int *error)
{
  uint32_t codepoint, state = CLO_DECODE_UTF8_ACCEPT;
  int i = 0;

  while (string[i])
    {
      (void) clo_decode_utf8(&state, &codepoint, (uint8_t) string[i]);
      i ++;
      if (state == CLO_DECODE_UTF8_ACCEPT || state == CLO_DECODE_UTF8_REJECT)
        break;
    }

  if (state == CLO_DECODE_UTF8_ACCEPT)
    clo_buffer_append(b, string, i, error);
  else
    (*error) ++;
  return i;
}

void clo_encode_number(double number, clo_buffer *b, int *error)
{
  char string[32];

  (void) snprintf(string, sizeof string, "%.16g", number);
  clo_buffer_append(b, string, strlen(string), error);
}

void clo_encode_string(char *string, clo_buffer *b, int *error)
{
  clo_buffer_append(b, "\"", 1, error);
  while (1)
    {
      while (*string >= 0x20 && *string != '"' && *string != '\\')
        {
          clo_buffer_append(b, string, 1, error);
          string ++;
        }
      if (*string < 0)
        string += clo_encode_utf8(string, b, error);
      else if (!*string)
        break;
      else if (*string == '"')
        {
          clo_buffer_append(b, "\\\"", 2, error);
          string ++;
        }
      else if (*string == '\\')
        {
          clo_buffer_append(b, "\\\\", 2, error);
          string ++;
        }
      else
        {
          clo_encode_control(*string, b, error);
          string ++;
        }
    }
  clo_buffer_append(b, "\"", 1, error);
}

void clo_encode_clo(clo *o, clo_buffer *b, int *error)
{
  int first;

  switch (o->type)
    {
    case CLO_OBJECT:
      clo_buffer_append(b, "{", 1, error);
      first = 1;
      for (clo_pair *p = o->object; p->string; p ++)
        {
          if (p->value.type != CLO_UNDEFINED)
            {
              if (first)
                first = 0;
              else
                clo_buffer_append(b, ",", 1, error);
              clo_encode_string(p->string, b, error);
              clo_buffer_append(b, ":", 1, error);
              clo_encode_clo(&p->value, b, error);
            }
        }
      clo_buffer_append(b, "}", 1, error);
      break;
    case CLO_ARRAY:
      clo_buffer_append(b, "[", 1, error);
      first = 1;
      for (clo *e = o->array; e->type != CLO_END_OF_ARRAY; e ++)
        {
          if (e->type != CLO_UNDEFINED)
            {
              if (first)
                first = 0;
              else
                clo_buffer_append(b, ",", 1, error);
              clo_encode_clo(e, b, error);
            }
        }
      clo_buffer_append(b, "]", 1, error);
      break;
    case CLO_STRING:
      clo_encode_string(o->string, b, error);
      break;
    case CLO_NUMBER:
      clo_encode_number(o->number, b, error);
      break;
    case CLO_TRUE:
      clo_buffer_append(b, "true", 4, error);
      break;
    case CLO_FALSE:
      clo_buffer_append(b, "false", 5, error);
      break;
    case CLO_NULL:
      clo_buffer_append(b, "null", 4, error);
      break;
    default:
      (*error) ++;
    }
}

int clo_encode(clo *o, char *base, size_t size)
{
  clo_buffer b = {.base = base, .size = size};
  int error = 0;

  clo_encode_clo(o, &b, &error);
  clo_buffer_append(&b, "", 1, &error);

  return error ? - 1 : 0;
}
