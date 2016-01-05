#ifndef CLO_H_INCLUDED
#define CLO_H_INCLUDED

#define CLO_DECODE_UTF8_ACCEPT 0
#define CLO_DECODE_UTF8_REJECT 12

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

uint32_t clo_decode_utf8(uint32_t *, uint32_t *, uint32_t);
void     clo_encode_append(buffer *, char *, int *);
void     clo_encode_control(buffer *, uint8_t, int *);
int      clo_encode_utf8(buffer *, char *, int *);
void     clo_encode_string(buffer *, char *, int *);
void     clo_encode_number(buffer *, double, int *);
void     clo_encode(clo *, buffer *, int *);
  
#endif /* CLO_H_INCLUDED*/
