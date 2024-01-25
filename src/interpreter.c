#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

literal_t *evaluate_expr(expr_t *expr) {
    if (expr->type == LITERAL) {
        literal_t *literal = malloc(sizeof(literal_t));
        if (strchr(expr->value, '.')) {
            literal->is_int = false;
            literal->value.float_value = atof(expr->value);
            printf("FLOAT %f\n", literal->value.float_value);
        } else {
            literal->is_int = true;
            literal->value.int_value = atoi(expr->value);
            printf("INT %d\n", literal->value.int_value);
        }
        return literal;
    } else if (expr->type == BINARY_OP) {
        literal_t *l = evaluate_expr(expr->left_operand);
        literal_t *r = evaluate_expr(expr->right_operand);
        if (expr->op == ADD) {
            if (l->is_int && r->is_int) {
                l->value.int_value += r->value.int_value;
            } else {
                if (l->is_int) l->value.float_value = (float) l->value.int_value;
                if (r->is_int) r->value.float_value = (float) r->value.int_value;
                l->value.float_value += r->value.float_value;
                l->is_int = false;
            }
        } else if (expr->op == SUBTRACT) {
            if (l->is_int && r->is_int) {
                l->value.int_value -= r->value.int_value;
            } else {
                if (l->is_int) l->value.float_value = (float) l->value.int_value;
                if (r->is_int) r->value.float_value = (float) r->value.int_value;
                l->value.float_value -= r->value.float_value;
                l->is_int = false;
            }
        } else if (expr->op == MULTIPLY) {
            if (l->is_int && r->is_int) {
                l->value.int_value *= r->value.int_value;
            } else {
                if (l->is_int) l->value.float_value = (float) l->value.int_value;
                if (r->is_int) r->value.float_value = (float) r->value.int_value;
                l->value.float_value *= r->value.float_value;
                l->is_int = false;
            }
        }
        free(r);
        return l;
    } 

    literal_t *zero_literal = malloc(sizeof(literal_t));
    zero_literal->is_int = true;
    zero_literal->value.int_value = 0;
    return zero_literal;
}

char *interpret_expr(expr_t *root) {
    literal_t *result = evaluate_expr(root);

    char *str;
    if (result->is_int) {
        printf("RESULT INT %d\n", result->value.int_value);
        int size = snprintf(NULL, 0, "%d", result->value.int_value);
        str = (char *)malloc(size + 1);
        snprintf(str, size + 1, "%d", result->value.int_value);
    } else {
        printf("RESULT float %f\n", result->value.float_value);
        int size = snprintf(NULL, 0, "%f", result->value.float_value);
        str = (char *)malloc(size + 1);
        snprintf(str, size + 1, "%f", result->value.float_value);
    }

    return str;
}
