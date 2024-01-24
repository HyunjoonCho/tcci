#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"

int evaluate_expr(expr_t *expr) {
    if (expr->type == LITERAL) {
        return atoi(expr->value);
    }
    return 0;
}

char *interpret_expr(expr_t *root) {
    char *result = malloc(2);

    if (root->type == BINARY_OP) {
        if (root->op == ADD) {
            int value = evaluate_expr(root->left_operand) + evaluate_expr(root->right_operand);
            snprintf(result, sizeof(result), "%d", value);
        } else if (root->op == SUBTRACT) {

        } else if (root->op == MULTIPLY) {

        }
    }
    return result;
}
