#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

literal_t *evaluate_ast(node_t *node) {
    if (node->type == BINARY_OP) {
        literal_t *l = evaluate_ast(node->left_child);
        literal_t *r = evaluate_ast(node->right_child);
        if (l->type == INTEGER_LITERAL && r->type == INTEGER_LITERAL && node->subtype != DIVIDE_OPERATOR) {
            if (node->subtype == ADD_OPERATOR) l->value.int_value += r->value.int_value;
            else if (node->subtype == SUBTRACT_OPERATOR) l->value.int_value -= r->value.int_value;
            else if (node->subtype == MULTIPLY_OPERATOR) l->value.int_value *= r->value.int_value;
        } else {
            if (l->type == INTEGER_LITERAL) {
                l->value.float_value = (float) l->value.int_value;
                l->type = FLOAT_LITERAL;
            } 
            if (r->type == INTEGER_LITERAL) r->value.float_value = (float) r->value.int_value;

            if (node->subtype == ADD_OPERATOR) l->value.float_value += r->value.float_value;
            else if (node->subtype == SUBTRACT_OPERATOR) l->value.float_value -= r->value.float_value;
            else if (node->subtype == MULTIPLY_OPERATOR) l->value.float_value *= r->value.float_value;
            else if (node->subtype == DIVIDE_OPERATOR) l->value.float_value /= r->value.float_value;
        } 
        free(r);
        return l;
    } else {
        literal_t *literal = malloc(sizeof(literal_t));
        literal->value = node->value;
        if (node->subtype == INTEGER_CONST) literal->type = INTEGER_LITERAL;
        else literal->type = FLOAT_LITERAL;
        return literal;
    }

    literal_t *zero_literal = malloc(sizeof(literal_t));
    zero_literal->type = INTEGER_LITERAL;
    zero_literal->value.int_value = 0;
    return zero_literal;
}

literal_t *interpret(node_t *root) {
    return evaluate_ast(root);
}
