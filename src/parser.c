#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commons.h"

expr_t *turn_token_into_expr(token_t *token, int *paren_openings) {
    if (token->type == OPEN_PAREN || token->type == CLOSE_PAREN) {
        *paren_openings += (token->type == OPEN_PAREN) - (token->type == CLOSE_PAREN);
        return NULL;
    }

    expr_t *expr = malloc(sizeof(expr_t));
    if (token->type == INTEGER) {
        expr->type = CONSTANT;
        expr->subtype.const_t = INTEGER_CONST;
        expr->value.int_value = atoi(token->value);
    } else if (token->type == FLOAT) {
        expr->type = CONSTANT;
        expr->subtype.const_t = FLOAT_CONST;
        expr->value.float_value = atof(token->value);      
    } else {
        int paren_priority = *paren_openings;
        expr->type = BINARY_OP;
        if (token->type == ADD_OPERATOR) expr->subtype.op_t = ADD;
        else if (token->type == SUBTRACT_OPERATOR) expr->subtype.op_t = SUBTRACT;
        else if (token->type == MULTIPLY_OPERATOR) expr->subtype.op_t = MULTIPLY;
        else if (token->type == DIVIDE_OPERATOR) expr->subtype.op_t = DIVIDE;

        if (token->type == ADD_OPERATOR || token->type == SUBTRACT_OPERATOR) expr->op_priority = 2 - 2 * paren_priority;
        else expr->op_priority = 1 - 2 * paren_priority;
    }
    return expr;
}

expr_t *assemble_tree(expr_t **expr_list, int start_index, int end_index) {
    if (start_index == end_index - 1) return expr_list[start_index];

    int max_priority = -100; // the bigger, the lesser prioritized
    int least_prioritized_index = -1;

    expr_t *current_expr;
    for (int i = start_index; i < end_index; i++) {
        current_expr = expr_list[i];
        if (current_expr->type == BINARY_OP && current_expr->op_priority >= max_priority) {
            max_priority = current_expr->op_priority;
            least_prioritized_index = i;
        }
    }

    current_expr = expr_list[least_prioritized_index];
    current_expr->left_operand = assemble_tree(expr_list, start_index, least_prioritized_index);
    current_expr->right_operand = assemble_tree(expr_list, least_prioritized_index + 1, end_index);

    return current_expr;
}

expr_t *parse_expression(token_t **tokens, int token_count) {
    int parentheses_openings = 0;
    if (token_count == 1) return turn_token_into_expr(tokens[0], &parentheses_openings);

    expr_t **expr_list = malloc(token_count * sizeof(expr_t *));

    int expr_count = 0;
    for (int i = 0; i < token_count; i++) {
        expr_t *expr = turn_token_into_expr(tokens[i], &parentheses_openings);
        if (expr != NULL) expr_list[expr_count++] = expr;
    }

    expr_t *root = assemble_tree(expr_list, 0, expr_count);    
    free(expr_list);

    return root;
}

void free_expr(expr_t *expr) {
    if (expr->type == BINARY_OP) {
        free_expr(expr->left_operand);
        free_expr(expr->right_operand);
        free(expr);
    } else {
        free(expr);
    }
}
