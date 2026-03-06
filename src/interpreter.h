#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "commons.h"

typedef struct literal_t {
    type_t type;
    value_t value;
} literal_t;

typedef struct identifier_t {
    char *id_name;
    literal_t literal;
} identifier_t;

typedef struct interpreter interpreter;
typedef interpreter *interpreter_handle;

interpreter_handle interpreter_init(node_t *root);
literal_t *get_value_of(interpreter_handle interpreter, const char *id_name);
literal_t *interpret(interpreter_handle interpreter);
void interpreter_free(interpreter_handle interpreter);
tcci_status_t interpreter_status(interpreter_handle interpreter);
const char *interpreter_error_message(interpreter_handle interpreter);

#endif
