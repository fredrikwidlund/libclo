#include <stdio.h>

#include "clo.h"

void clo_print_one(char c)
{
  printf("%c", c);
}

void clo_print_literal(char *l)
{
  printf("%s", l);
}

void clo_print_number(double n)
{
  printf("%.16g", n);
}

void clo_print_string(char *string)
{
  clo_print_one('"');
  clo_print_literal(string);
  clo_print_one('"');
}

void clo_print(clo *o)
{
  clo_pair *p;
  clo *e;
  
  switch(o->type)
    {
    case CLO_STRING:
      clo_print_string(o->string);
      break;
    case CLO_NUMBER:
      clo_print_number(o->number);
      break;
    case CLO_OBJECT:
      clo_print_one('{');
      for (p = o->object; p->string; p ++)
	{
	  clo_print_string(p->string);
	  clo_print_one(':');
	  clo_print(&p->value);
	  if (p[1].string)
	    clo_print_one(',');
	}
      clo_print_one('}');
      break;
    case CLO_ARRAY:
      clo_print_one('[');
      for (e = o->array; e->type != CLO_UNDEFINED; e ++)
	{
	  clo_print(e);
	  if (e[1].type != CLO_UNDEFINED)
	    clo_print_one(',');
	}
      clo_print_one(']');
      break;
    case CLO_TRUE:
      clo_print_literal("true");
      break;
    case CLO_FALSE:
      clo_print_literal("false");
      break;
    case CLO_NULL:
      clo_print_literal("null");
      break;
    default:
      break;
      
    }
}

int main()
{
  clo o = clo_object
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

  clo_print(&o);
}
