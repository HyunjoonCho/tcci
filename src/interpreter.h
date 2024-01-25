#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include "commons.h"

union literal_value {
    int int_value;
    float float_value;
};

typedef union literal_value literal_value;

struct literal_t {
    bool is_int;
    literal_value value;
};

typedef struct literal_t literal_t;

literal_t *interpret_expr(expr_t *root);

#endif