#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commons.h"

struct parser {
    token_t **tokens;
    int token_count;
    int current;
    int opened_paren_count;
    node_t **nodes;
};

parser_handle parser_init(token_t **tokens, int token_count) {
    parser_handle new_parser = malloc(sizeof(parser));
    new_parser->tokens = tokens; // may copy the tokens?
    new_parser->token_count = token_count;
    new_parser->current = 0;
    new_parser->opened_paren_count = 0;
    return new_parser;
}

int has_next(parser_handle parser) {
    return parser->current < parser->token_count;
}

node_t *turn_token_into_node(parser_handle parser) {
    token_t *token = parser->tokens[parser->current++];
    if (token->type == SEMICOLON) return NULL;
    if (token->type == OPEN_PAREN || token->type == CLOSE_PAREN) {
        parser->opened_paren_count += (token->type == OPEN_PAREN) - (token->type == CLOSE_PAREN);
        return NULL;
    }

    node_t *node = malloc(sizeof(node_t));
    if (token->type == INTEGER_CONST || token->type == FLOAT_CONST){
        node->actual_node = malloc(sizeof(constant_node));
        node->type = CONSTANT_NODE;
        ((constant_node *)node->actual_node)->subtype = token->type;
        if (token->type == INTEGER_CONST) ((constant_node *)node->actual_node)->value.int_value = atoi(token->value);
        else ((constant_node *)node->actual_node)->value.float_value = atof(token->value);
    } else if (token->type == ADD_OPERATOR || token->type == SUBTRACT_OPERATOR || token->type == MULTIPLY_OPERATOR || token->type == DIVIDE_OPERATOR) {
        node->actual_node = malloc(sizeof(binary_op_node));
        node->type = BINARY_OP_NODE;
        ((binary_op_node *)node->actual_node)->subtype = token->type;
        int paren_priority = parser->opened_paren_count;
        if (token->type == ADD_OPERATOR || token->type == SUBTRACT_OPERATOR) ((binary_op_node *)node->actual_node)->op_priority= 2 - 2 * paren_priority;
        else ((binary_op_node *)node->actual_node)->op_priority= 1 - 2 * paren_priority;
    } else if (token->type == INTEGER_TYPE || token->type == FLOAT_TYPE) {
        node->actual_node = malloc(sizeof(type_specifier_node));
        node->type = TYPE_SPECIFIER_NODE;
        ((type_specifier_node *)node->actual_node)->subtype = token->type;
    } else if (token->type == IDENTIFIER) {
        node->actual_node = malloc(sizeof(identifier_node));
        node->type = IDENTIFIER_NODE;
        ((identifier_node *)node->actual_node)->id_name = strdup(token->value);
    } else if (token->type == EQ_ASSIGN) {
        node->actual_node = malloc(sizeof(assign_op_node));
        node->type = ASSIGN_OP_NODE;
        ((assign_op_node *)node->actual_node)->subtype = token->type;
        ((assign_op_node *)node->actual_node)->op_priority= 3;
    }
    
    return node;
}

node_t *generate_decl_token() {
    node_t *node = malloc(sizeof(node_t));
    node->actual_node = malloc(sizeof(declaration_node));
    node->type = DECLARATION_NODE;
    ((declaration_node *)node->actual_node)->subtype = DECLARATION;
    ((declaration_node *)node->actual_node)->op_priority= 4;
    return node;
}

int op_priority(node_t *node) {
    if (node->type == DECLARATION_NODE) return ((declaration_node *)node->actual_node)->op_priority;
    if (node->type == BINARY_OP_NODE) return ((binary_op_node *)node->actual_node)->op_priority;
    if (node->type == ASSIGN_OP_NODE) return ((assign_op_node *)node->actual_node)->op_priority;
    else return 0;
}

node_t *get_left_child(node_t *node) {
    if (node->type == DECLARATION_NODE) return ((declaration_node *)node->actual_node)->left_child;
    if (node->type == BINARY_OP_NODE) return ((binary_op_node *)node->actual_node)->left_child;
    if (node->type == ASSIGN_OP_NODE) return ((assign_op_node *)node->actual_node)->left_child;
    else return NULL;
}

node_t *get_right_child(node_t *node) {
    if (node->type == DECLARATION_NODE) return ((declaration_node *)node->actual_node)->right_child;
    if (node->type == BINARY_OP_NODE) return ((binary_op_node *)node->actual_node)->right_child;
    if (node->type == ASSIGN_OP_NODE) return ((assign_op_node *)node->actual_node)->right_child;
    else return NULL;
}

void set_left_child(node_t *node, node_t *child) {
    if (node->type == DECLARATION_NODE) ((declaration_node *)node->actual_node)->left_child = child;
    if (node->type == BINARY_OP_NODE) ((binary_op_node *)node->actual_node)->left_child = child;
    if (node->type == ASSIGN_OP_NODE) ((assign_op_node *)node->actual_node)->left_child = child;
}

void set_right_child(node_t *node, node_t *child) {
    if (node->type == DECLARATION_NODE) ((declaration_node *)node->actual_node)->right_child = child;
    if (node->type == BINARY_OP_NODE) ((binary_op_node *)node->actual_node)->right_child = child;
    if (node->type == ASSIGN_OP_NODE) ((assign_op_node *)node->actual_node)->right_child = child;
}

node_t *assemble_tree(node_t **nodes, int start_index, int end_index) {
    if (start_index == end_index - 1) return nodes[start_index];

    int max_priority = -100; // the bigger, the lesser prioritized
    int least_prioritized_index = -1;

    node_t *current_node;
    for (int i = start_index; i < end_index; i++) {
        current_node = nodes[i];
        if ((current_node->type == DECLARATION_NODE || current_node->type == BINARY_OP_NODE || current_node->type == ASSIGN_OP_NODE) && 
             op_priority(current_node) >= max_priority) {
            max_priority = op_priority(current_node);
            least_prioritized_index = i;
        }
    }

    current_node = nodes[least_prioritized_index];
    if (current_node->type == DECLARATION_NODE) nodes[least_prioritized_index + 1]->type = DECLARATOR_NODE; // Assume: following token must be identifier
    set_left_child(current_node, assemble_tree(nodes, start_index, least_prioritized_index));
    set_right_child(current_node, assemble_tree(nodes, least_prioritized_index + 1, end_index));

    return current_node;
}

node_t *parse(parser_handle parser) {
    token_t **tokens = parser->tokens;
    int token_count = parser->token_count; 

    parser->nodes = malloc((token_count + 3) * sizeof(node_t *)); // Leave room for DECL node to be added

    node_t *root = (node_t *)malloc(sizeof(node_t));
    root->type = COMPOUND_STATEMENT_NODE;
    root->actual_node = malloc(sizeof(compound_statment_node));
    ((compound_statment_node *)root->actual_node)->children = malloc(sizeof(node_t *));
    int children_count = 0;

    int node_count = 0;
    while (has_next(parser)) {
        node_t *node = turn_token_into_node(parser);
        if (node == NULL) {
            if (parser->tokens[parser->current - 1]->type == SEMICOLON) {
                node_t *node = assemble_tree(parser->nodes, 0, node_count);
                ((node_t **)((compound_statment_node *)root->actual_node)->children)[children_count++] = node;
                
                node_count = 0;
                parser->nodes = malloc((token_count + 3) * sizeof(node_t *));
            }
            continue;
        }
        parser->nodes[node_count++] = node;
        if (node->type == TYPE_SPECIFIER_NODE) parser->nodes[node_count++] = generate_decl_token();
    }
    ((compound_statment_node *)root->actual_node)->children_count = children_count;
    
    return root;
}

void free_parser(parser_handle parser) {
    free_tokens(parser->tokens, parser->token_count);
    // FIXME: Unless tokens are explicitly copied, this freeing may lead to confusion.
    free(parser->tokens);
    free(parser->nodes);
    free(parser);
}

void free_node(node_t *node) {
    if (node->type == BINARY_OP_NODE || node->type == ASSIGN_OP_NODE|| node->type == DECLARATION_NODE) {
        free_node(get_left_child(node));
        free_node(get_right_child(node));
    } else if (node->type == IDENTIFIER_NODE) {
        free(((identifier_node *)node->actual_node)->id_name);
    }
    free(node);
}