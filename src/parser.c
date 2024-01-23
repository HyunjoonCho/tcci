#include <stdlib.h>
#include <string.h>
#include "commons.h"

expr_t parse_expression(token_t *tokens,int token_count) {
    expr_t root;
    root.type = BINARY_OP;
    root.op = ADD;  
    
    expr_t *left = malloc(sizeof(expr_t));
    left->type = LITERAL;
    char *left_value = malloc(sizeof(*(tokens[0].value)));
    strcpy(left_value, tokens[0].value);
    left->value = left_value;
    
    expr_t *right = malloc(sizeof(expr_t));
    right->type = LITERAL;
    char *right_value = malloc(sizeof(*(tokens[2].value)));
    strcpy(right_value, tokens[2].value);
    right->value = right_value;

    root.left_operand = left;
    root.right_operand = right;

    return root;
}