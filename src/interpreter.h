#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include "commons.h"


typedef enum {
    INTEGER_LITERAL,
    FLOAT_LITERAL,
} literal_type;

struct literal_t {
    literal_type type;
    literal_value value;
};

typedef struct literal_t literal_t;

literal_t *interpret_expr(expr_t *root);

#endif