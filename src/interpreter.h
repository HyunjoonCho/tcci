#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include "commons.h"

struct literal_t {
    type_t type;
    value_t value;
};

typedef struct literal_t literal_t;

struct identifier_t {
    char *id_name;
    literal_t literal;
};

typedef struct identifier_t identifier_t;

typedef struct interpreter interpreter;
typedef interpreter *interpreter_handle;

interpreter_handle interpreter_init(node_t *root);
literal_t *get_value_of(interpreter_handle interpreter, const char *id_name);
literal_t *interpret(interpreter_handle interpreter);

#endif