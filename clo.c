#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dynamic.h>

#include "clo.h"

void clo_encode_append(buffer *b, char *string, int *error)
{
  int e;

  e = buffer_insert(b, buffer_size(b), string, strlen(string));
  *error += (e != 0);
}

void clo_encode_string(buffer *b, char *string, int *error)
{
  char *data;
  
  clo_encode_append(b, "\"", error);
  buffer_reserve(b, buffer_size(b) + strlen(string));
  data = buffer_data(b) + buffer_size(b);
  while (*string >= 0x20 && *string <= 0x7f && *string != '\n' && *string != '"' && *string != '\\')
    {
      *data = *string;
      data ++;
      string ++;
    }
  b->size = data - buffer_data(b);
  clo_encode_append(b, "\"", error);
}

void clo_encode_number(buffer *b, double number, int *error)
{
  char string[32];
  int e;
  
  e = snprintf(string, sizeof string, "%.16g", number);
  if (e > 0 && e < sizeof string)
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

int main()
{
  buffer buffer;
  int error = 0;
  clo o;

  o = clo_object
    ({"hello", clo_string("world")},
     {"test", clo_array
	 (clo_number(42.23),
	  clo_number(1),
	  clo_string("test"),
	  clo_true(),
	  clo_object
	  ({"some", clo_true()},
	   {"object", clo_false()}),
	  clo_false(),
	  clo_null())});

  buffer_init(&buffer);
  clo_encode(&o, &buffer, &error);
  fprintf(stderr, "errors %d\n", error);
  fprintf(stdout, "%.*s\n", (int) buffer_size(&buffer), buffer_data(&buffer));
}
