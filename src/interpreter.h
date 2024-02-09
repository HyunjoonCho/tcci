#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include "commons.h"

struct literal_t {
    type_t type;
    value_t value;
};

typedef struct literal_t literal_t;

literal_t *interpret(node_t *root);

#endif