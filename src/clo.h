#ifndef CLO_H_INCLUDED
#define CLO_H_INCLUDED

#define CLO_DECODE_UTF8_ACCEPT 0
#define CLO_DECODE_UTF8_REJECT 12

#define clo_string(v)   ((clo){.type = CLO_STRING, .string = (v)})
#define clo_number(v)   ((clo){.type = CLO_NUMBER, .number = (v)})
#define clo_object(...) ((clo){.type = CLO_OBJECT, .object = (clo_pair[]){__VA_ARGS__, {.string = NULL}}})
#define clo_array(...)  ((clo){.type = CLO_ARRAY, .array = (clo[]){__VA_ARGS__, {.type = CLO_END_OF_ARRAY}}})
#define clo_true()      ((clo){.type = CLO_TRUE})
#define clo_false()     ((clo){.type = CLO_FALSE})
#define clo_null()      ((clo){.type = CLO_NULL})
#define clo_undefined() ((clo){.type = CLO_UNDEFINED})

enum clo_type
{
  CLO_UNDEFINED = 0,
  CLO_STRING,
  CLO_NUMBER,
  CLO_OBJECT,
  CLO_ARRAY,
  CLO_TRUE,
  CLO_FALSE,
  CLO_NULL,
  CLO_END_OF_ARRAY
};

typedef struct clo_buffer clo_buffer;
struct clo_buffer
{
  char              *base;
  size_t             size;
};

typedef struct clo clo;
struct clo
{
  enum clo_type      type;
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
  char              *string;
  clo                value;
};

uint32_t clo_decode_utf8(uint32_t *, uint32_t *, uint32_t);
void     clo_buffer_append(clo_buffer *, char *, size_t, int *);
void     clo_encode_control(uint8_t, clo_buffer *, int *);
int      clo_encode_utf8(char *, clo_buffer *, int *);
void     clo_encode_number(double, clo_buffer *, int *);
void     clo_encode_string(char *, clo_buffer *, int *);
void     clo_encode_clo(clo *, clo_buffer *, int *);
int      clo_encode(clo *, char *, size_t);

#endif /* CLO_H_INCLUDED */
