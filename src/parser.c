#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commons.h"

expr_t *turn_token_into_expr(token_t *token) {
    expr_t *expr = malloc(sizeof(expr_t));
    if (token->type == INTEGER) {
        expr->type = LITERAL;
        char *value = malloc(sizeof(*(token->value)));
        strcpy(value, token->value);
        expr->value = value;
    } else if (token->type == FLOAT) {
        expr->type = LITERAL;
        char *value = malloc(sizeof(*(token->value)));
        strcpy(value, token->value);
        expr->value = value;      
    } else {
        expr->type = BINARY_OP;
        if (token->type == ADD_OPERATOR) expr->op = ADD;
        else if (token->type == SUBTRACT_OPERATOR) expr->op = SUBTRACT;
        else if (token->type == MULTIPLY_OPERATOR) expr->op = MULTIPLY;
    }
    return expr;
}

expr_t *parse_expression(token_t **tokens, int token_count) {
    if (token_count == 1) return turn_token_into_expr(tokens[0]);

    expr_t **expr_list = malloc(token_count * sizeof(expr_t *));

    for (int i = 0; i < token_count; i++) {
        expr_t *expr = turn_token_into_expr(tokens[i]);
        expr_list[i] = expr;
    }
    
    int current = 0;
    for (int i = 0; i < token_count; i++) {
        if (expr_list[i] == NULL) break;
        if (expr_list[current]->type == BINARY_OP && expr_list[current]->op == MULTIPLY) { // higher priority operator
            expr_list[current]->left_operand = expr_list[current - 1];
            expr_list[current]->right_operand = expr_list[current + 1];
            expr_list[current - 1] = expr_list[current];

            for (int j = current + 2; j < token_count; j++) {
                expr_list[j - 2] = expr_list[j];
            }
            expr_list[token_count - 2] = NULL;
            expr_list[token_count - 1] = NULL;
        } else {
            current++;
        }
    }

    current = 0;
    for (int i = 0; i < token_count; i++) {
        if (expr_list[i] == NULL) break;
        if (expr_list[current]->type == BINARY_OP && 
            (expr_list[current]->op == ADD || expr_list[current]->op == SUBTRACT)) {
            expr_list[current]->left_operand = expr_list[current - 1];
            expr_list[current]->right_operand = expr_list[current + 1];
            expr_list[current - 1] = expr_list[current];

            for (int j = current + 2; j < token_count; j++) {
                expr_list[j - 2] = expr_list[j];
            }
            expr_list[token_count - 2] = NULL;
            expr_list[token_count - 1] = NULL;
        } else {
            current++;
        }
    }

    expr_t *root = expr_list[0];
    free(expr_list);

    return root;
}

void free_expr(expr_t *expr) {
    if (expr->type == BINARY_OP) {
        free_expr(expr->left_operand);
        free_expr(expr->right_operand);
        free(expr);
    } else {
        free(expr->value);
        free(expr);
    }
}
