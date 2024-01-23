#include <stdlib.h>
#include <string.h>
#include "commons.h"

expr_t *turn_token_into_expr(token_t token) {
    expr_t *expr = malloc(sizeof(expr_t));
    if (token.type == INTEGER) {
        expr->type = LITERAL;
        char *value = malloc(sizeof(*token.value));
        strcpy(value, token.value);
        expr->value = value;
    } else {
        expr->type = BINARY_OP;
        if (token.type == ADD_OPERATOR) expr->op = ADD;
        else if (token.type == SUBTRACT_OPERATOR) expr->op = SUBTRACT;
        else if (token.type == MULTIPLY_OPERATOR) expr->op = MULTIPLY;
    }
    return expr;
}

expr_t *parse_expression(token_t *tokens,int token_count) {
    // stack
    expr_t *root = turn_token_into_expr(tokens[1]);
    expr_t *left = turn_token_into_expr(tokens[0]);
    expr_t *right = turn_token_into_expr(tokens[2]);
    
    root->left_operand = left;
    root->right_operand = right;

    return root;
}