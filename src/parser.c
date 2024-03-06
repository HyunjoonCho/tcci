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
        node->type = CONSTANT;
        ((constant_node *)node->actual_node)->subtype = token->type;
        if (token->type == INTEGER_CONST) ((constant_node *)node->actual_node)->value.int_value = atoi(token->value);
        else ((constant_node *)node->actual_node)->value.float_value = atof(token->value);
    } else {
        node->actual_node = malloc(sizeof(dummy_node));
        ((dummy_node *)node->actual_node)->subtype = token->type;

        if (token->type == INTEGER_TYPE || token->type == FLOAT_TYPE) {
            node->type = TYPE_SPECIFIER;
        } else if (token->type == IDENTIFIER) {
            node->type = ID;
            ((dummy_node *)node->actual_node)->value.id_name = strdup(token->value);
        } else if (token->type == EQ_ASSIGN) {
            node->type = ASSIGN_OP;
            ((dummy_node *)node->actual_node)->op_priority= 3;
        } else {
            int paren_priority = parser->opened_paren_count;
            node->type = BINARY_OP;
            if (token->type == ADD_OPERATOR || token->type == SUBTRACT_OPERATOR) ((dummy_node *)node->actual_node)->op_priority= 2 - 2 * paren_priority;
            else ((dummy_node *)node->actual_node)->op_priority= 1 - 2 * paren_priority;
        }
    }
    
    return node;
}

node_t *generate_decl_token() {
    node_t *decl_node = malloc(sizeof(node_t));
    decl_node->actual_node = malloc(sizeof(dummy_node));
    decl_node->type = DECL;
    ((dummy_node *)decl_node->actual_node)->subtype = DECLARATION;
    ((dummy_node *)decl_node->actual_node)->op_priority= 4;
    return decl_node;
}

node_t *assemble_tree(node_t **nodes, int start_index, int end_index) {
    if (start_index == end_index - 1) return nodes[start_index];

    int max_priority = -100; // the bigger, the lesser prioritized
    int least_prioritized_index = -1;

    node_t *current_node;
    for (int i = start_index; i < end_index; i++) {
        current_node = nodes[i];
        if ((current_node->type == DECL || current_node->type == BINARY_OP || current_node->type == ASSIGN_OP) && 
             ((dummy_node *)current_node->actual_node)->op_priority>= max_priority) {
            max_priority = ((dummy_node *)current_node->actual_node)->op_priority;
            least_prioritized_index = i;
        }
    }

    current_node = nodes[least_prioritized_index];
    if (current_node->type == DECL) nodes[least_prioritized_index + 1]->type = DECLARATOR; // Assume: following token must be identifier
    ((dummy_node *)current_node->actual_node)->left_child = assemble_tree(nodes, start_index, least_prioritized_index);
    ((dummy_node *)current_node->actual_node)->right_child = assemble_tree(nodes, least_prioritized_index + 1, end_index);

    return current_node;
}

node_t *parse(parser_handle parser) {
    token_t **tokens = parser->tokens;
    int token_count = parser->token_count; 

    parser->nodes = malloc((token_count + 3) * sizeof(node_t *)); // Leave room for DECL node to be added

    int node_count = 0;
    while (has_next(parser)) {
        node_t *node = turn_token_into_node(parser);
        if (node == NULL) continue;
        parser->nodes[node_count++] = node;
        if (node->type == TYPE_SPECIFIER) parser->nodes[node_count++] = generate_decl_token();
    }

    node_t *root = assemble_tree(parser->nodes, 0, node_count);    

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
    if (node->type == BINARY_OP || node->type == ASSIGN_OP|| node->type == DECL) {
        free_node(((dummy_node *)node->actual_node)->left_child);
        free_node(((dummy_node *)node->actual_node)->right_child);
    } else if (((dummy_node *)node->actual_node)->subtype == IDENTIFIER) {
        free(((dummy_node *)node->actual_node)->value.id_name);
    }
    free(node);
}