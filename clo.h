#ifndef CLO_H_INCLUDED
#define CLO_H_INCLUDED

enum
{
  CLO_UNDEFINED = 0,
  CLO_STRING,
  CLO_NUMBER,
  CLO_OBJECT,
  CLO_ARRAY,
  CLO_TRUE,
  CLO_FALSE,
  CLO_NULL
};

typedef struct clo clo;
struct clo
{
  unsigned char type;
  union
  {
    char            *string;
    double           number;
    struct clo_pair *object;
    clo             *array;
  };
};

typedef struct clo_pair clo_pair;
struct clo_pair
{
  char         *string;
  clo           value;
};

#define clo_string(v)   ((clo){.type = CLO_STRING, .string = (v)})
#define clo_number(v)   ((clo){.type = CLO_NUMBER, .number = (v)})
#define clo_object(...) ((clo){.type = CLO_OBJECT, .object = (clo_pair[]){__VA_ARGS__, {.string = NULL}}})
#define clo_array(...)  ((clo){.type = CLO_ARRAY, .array = (clo[]){__VA_ARGS__, {.type = CLO_UNDEFINED}}})
#define clo_true()      ((clo){.type = CLO_TRUE})
#define clo_false()     ((clo){.type = CLO_FALSE})
#define clo_null()      ((clo){.type = CLO_NULL})

#endif /* CLO_H_INCLUDED*/
