#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commons.h"

node_t *turn_token_into_node(token_t *token, int *paren_openings, type_t *id_type) {
    if (token->type == SEMICOLON) return NULL;
    if (token->type == OPEN_PAREN || token->type == CLOSE_PAREN) {
        *paren_openings += (token->type == OPEN_PAREN) - (token->type == CLOSE_PAREN);
        return NULL;
    }

    if (token->type == INTEGER_TYPE || token->type == FLOAT_TYPE) {
        *id_type = token->type;
        return NULL;
    }

    node_t *node = malloc(sizeof(node_t));
    if (token->type == INTEGER_CONST) {
        node->type = CONSTANT;
        node->subtype = INTEGER_CONST;
        node->value.int_value = atoi(token->value);
    } else if (token->type == FLOAT_CONST) {
        node->type = CONSTANT;
        node->subtype = FLOAT_CONST;
        node->value.float_value = atof(token->value);      
    } else if (token->type == IDENTIFIER) {
        node->type = ID;
        node->subtype = *id_type;
        *id_type = NULLTOKEN;
        node->value.id_name = strdup(token->value);
    } else if (token->type == EQ_ASSIGN) {
        node->type = ASSIGN_OP;
        node->subtype = EQ_ASSIGN;
        node->op_priority = 3;
    } else {
        int paren_priority = *paren_openings;
        node->type = BINARY_OP;
        if (token->type == ADD_OPERATOR) node->subtype = ADD_OPERATOR;
        else if (token->type == SUBTRACT_OPERATOR) node->subtype = SUBTRACT_OPERATOR;
        else if (token->type == MULTIPLY_OPERATOR) node->subtype = MULTIPLY_OPERATOR;
        else if (token->type == DIVIDE_OPERATOR) node->subtype = DIVIDE_OPERATOR;

        if (token->type == ADD_OPERATOR || token->type == SUBTRACT_OPERATOR) node->op_priority = 2 - 2 * paren_priority;
        else node->op_priority = 1 - 2 * paren_priority;
    }
    return node;
}

node_t *assemble_tree(node_t **nodes, int start_index, int end_index) {
    if (start_index == end_index - 1) return nodes[start_index];

    int max_priority = -100; // the bigger, the lesser prioritized
    int least_prioritized_index = -1;

    node_t *current_node;
    for (int i = start_index; i < end_index; i++) {
        current_node = nodes[i];
        if ((current_node->type == BINARY_OP || current_node->type == ASSIGN_OP) && 
             current_node->op_priority >= max_priority) {
            max_priority = current_node->op_priority;
            least_prioritized_index = i;
        }
    }

    current_node = nodes[least_prioritized_index];
    current_node->left_child = assemble_tree(nodes, start_index, least_prioritized_index);
    current_node->right_child = assemble_tree(nodes, least_prioritized_index + 1, end_index);

    return current_node;
}

node_t *parse(token_t **tokens, int token_count) {
    int parentheses_openings = 0;
    type_t id_type = NULLTOKEN;
    if (token_count == 1) return turn_token_into_node(tokens[0], &parentheses_openings, &id_type);

    node_t **nodes = malloc(token_count * sizeof(node_t *));

    int node_count = 0;
    for (int i = 0; i < token_count; i++) {
        node_t *node = turn_token_into_node(tokens[i], &parentheses_openings, &id_type);
        if (node != NULL) nodes[node_count++] = node;
    }

    node_t *root = assemble_tree(nodes, 0, node_count);    
    free(nodes);

    return root;
}

void free_node(node_t *node) {
    if (node->type == BINARY_OP) {
        free_node(node->left_child);
        free_node(node->right_child);
        free(node);
    } else {
        free(node);
    }
}
