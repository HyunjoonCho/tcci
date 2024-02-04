#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

literal_t *evaluate_expr(expr_t *expr) {
    if (expr->type == BINARY_OP) {
        literal_t *l = evaluate_expr(expr->left_operand);
        literal_t *r = evaluate_expr(expr->right_operand);
        if (l->type == INTEGER_LITERAL && r->type == INTEGER_LITERAL && expr->subtype.op_t != DIVIDE) {
            if (expr->subtype.op_t == ADD) l->value.int_value += r->value.int_value;
            else if (expr->subtype.op_t == SUBTRACT) l->value.int_value -= r->value.int_value;
            else if (expr->subtype.op_t == MULTIPLY) l->value.int_value *= r->value.int_value;
        } else {
            if (l->type == INTEGER_LITERAL) {
                l->value.float_value = (float) l->value.int_value;
                l->type = FLOAT_LITERAL;
            } 
            if (r->type == INTEGER_LITERAL) r->value.float_value = (float) r->value.int_value;

            if (expr->subtype.op_t == ADD) l->value.float_value += r->value.float_value;
            else if (expr->subtype.op_t == SUBTRACT) l->value.float_value -= r->value.float_value;
            else if (expr->subtype.op_t == MULTIPLY) l->value.float_value *= r->value.float_value;
            else if (expr->subtype.op_t == DIVIDE) l->value.float_value /= r->value.float_value;
        } 
        free(r);
        return l;
    } else {
        literal_t *literal = malloc(sizeof(literal_t));
        literal->value = expr->value;
        if (expr->subtype.const_t == INTEGER_CONST) literal->type = INTEGER_LITERAL;
        else literal->type = FLOAT_LITERAL;
        return literal;
    }

    literal_t *zero_literal = malloc(sizeof(literal_t));
    zero_literal->type = INTEGER_LITERAL;
    zero_literal->value.int_value = 0;
    return zero_literal;
}

literal_t *interpret_expr(expr_t *root) {
    return evaluate_expr(root);
}
