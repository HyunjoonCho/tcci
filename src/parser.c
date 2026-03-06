#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commons.h"

#define ERROR_MESSAGE_SIZE 256

struct parser {
    token_t **tokens;
    int token_count;
    int current;
    int opened_paren_count;
    tcci_status_t status;
    char error_message[ERROR_MESSAGE_SIZE];
};

static void parser_set_error(parser_handle parser, tcci_status_t status, const char *fmt, ...) {
    va_list args;

    if (parser == NULL || parser->status != TCCI_STATUS_OK) {
        return;
    }

    parser->status = status;
    va_start(args, fmt);
    (void)vsnprintf(parser->error_message, sizeof(parser->error_message), fmt, args);
    va_end(args);
}

static char *duplicate_string(parser_handle parser, const char *source) {
    size_t length;
    char *copy;

    if (source == NULL) {
        return NULL;
    }

    length = strlen(source);
    copy = (char *)malloc(length + 1);
    if (copy == NULL) {
        parser_set_error(parser, TCCI_STATUS_OOM, "Failed to allocate identifier string");
        return NULL;
    }

    memcpy(copy, source, length + 1);
    return copy;
}

static node_t *create_node(parser_handle parser, node_type type, size_t payload_size) {
    node_t *node = (node_t *)malloc(sizeof(node_t));

    if (node == NULL) {
        parser_set_error(parser, TCCI_STATUS_OOM, "Failed to allocate AST node");
        return NULL;
    }

    node->actual_node = calloc(1, payload_size);
    if (node->actual_node == NULL) {
        free(node);
        parser_set_error(parser, TCCI_STATUS_OOM, "Failed to allocate AST node payload");
        return NULL;
    }

    node->type = type;
    return node;
}

parser_handle parser_init(token_t **tokens, int token_count) {
    parser_handle new_parser = (parser_handle)malloc(sizeof(parser));

    if (new_parser == NULL) {
        return NULL;
    }

    new_parser->tokens = tokens;
    new_parser->token_count = token_count;
    new_parser->current = 0;
    new_parser->opened_paren_count = 0;
    new_parser->status = TCCI_STATUS_OK;
    new_parser->error_message[0] = '\0';

    return new_parser;
}

void free_parser(parser_handle parser) {
    free(parser);
}

tcci_status_t parser_status(parser_handle parser) {
    if (parser == NULL) {
        return TCCI_STATUS_PARSER_ERROR;
    }

    return parser->status;
}

const char *parser_error_message(parser_handle parser) {
    if (parser == NULL) {
        return "Parser handle is NULL";
    }

    return parser->error_message;
}

static bool has_next(parser_handle parser) {
    return parser->current < parser->token_count;
}

static int op_priority(node_t *node) {
    if (node->type == DECLARATION_NODE) {
        return ((declaration_node *)node->actual_node)->op_priority;
    }
    if (node->type == BINARY_OP_NODE) {
        return ((binary_op_node *)node->actual_node)->op_priority;
    }
    if (node->type == ASSIGN_OP_NODE) {
        return ((assign_op_node *)node->actual_node)->op_priority;
    }

    return 0;
}

static node_t *get_left_child(node_t *node) {
    if (node->type == DECLARATION_NODE) {
        return ((declaration_node *)node->actual_node)->left_child;
    }
    if (node->type == BINARY_OP_NODE) {
        return ((binary_op_node *)node->actual_node)->left_child;
    }
    if (node->type == ASSIGN_OP_NODE) {
        return ((assign_op_node *)node->actual_node)->left_child;
    }

    return NULL;
}

static node_t *get_right_child(node_t *node) {
    if (node->type == DECLARATION_NODE) {
        return ((declaration_node *)node->actual_node)->right_child;
    }
    if (node->type == BINARY_OP_NODE) {
        return ((binary_op_node *)node->actual_node)->right_child;
    }
    if (node->type == ASSIGN_OP_NODE) {
        return ((assign_op_node *)node->actual_node)->right_child;
    }

    return NULL;
}

static void set_left_child(node_t *node, node_t *child) {
    if (node->type == DECLARATION_NODE) {
        ((declaration_node *)node->actual_node)->left_child = child;
    }
    if (node->type == BINARY_OP_NODE) {
        ((binary_op_node *)node->actual_node)->left_child = child;
    }
    if (node->type == ASSIGN_OP_NODE) {
        ((assign_op_node *)node->actual_node)->left_child = child;
    }
}

static void set_right_child(node_t *node, node_t *child) {
    if (node->type == DECLARATION_NODE) {
        ((declaration_node *)node->actual_node)->right_child = child;
    }
    if (node->type == BINARY_OP_NODE) {
        ((binary_op_node *)node->actual_node)->right_child = child;
    }
    if (node->type == ASSIGN_OP_NODE) {
        ((assign_op_node *)node->actual_node)->right_child = child;
    }
}

static node_t *generate_decl_token(parser_handle parser) {
    node_t *node = create_node(parser, DECLARATION_NODE, sizeof(declaration_node));

    if (node == NULL) {
        return NULL;
    }

    ((declaration_node *)node->actual_node)->subtype = DECLARATION;
    ((declaration_node *)node->actual_node)->op_priority = 4;
    return node;
}

static node_t *token_to_node(parser_handle parser, token_t *token) {
    node_t *node = NULL;

    if (token->type == INTEGER_CONST || token->type == FLOAT_CONST) {
        node = create_node(parser, CONSTANT_NODE, sizeof(constant_node));
        if (node == NULL) {
            return NULL;
        }

        ((constant_node *)node->actual_node)->subtype = token->type;
        if (token->type == INTEGER_CONST) {
            ((constant_node *)node->actual_node)->value.int_value = atoi(token->value);
        } else {
            ((constant_node *)node->actual_node)->value.float_value = (float)atof(token->value);
        }
        return node;
    }

    if (token->type == ADD_OPERATOR || token->type == SUBTRACT_OPERATOR ||
        token->type == MULTIPLY_OPERATOR || token->type == DIVIDE_OPERATOR) {
        int paren_priority = parser->opened_paren_count;

        node = create_node(parser, BINARY_OP_NODE, sizeof(binary_op_node));
        if (node == NULL) {
            return NULL;
        }

        ((binary_op_node *)node->actual_node)->subtype = token->type;
        if (token->type == ADD_OPERATOR || token->type == SUBTRACT_OPERATOR) {
            ((binary_op_node *)node->actual_node)->op_priority = 2 - (2 * paren_priority);
        } else {
            ((binary_op_node *)node->actual_node)->op_priority = 1 - (2 * paren_priority);
        }
        return node;
    }

    if (token->type == INTEGER_TYPE || token->type == FLOAT_TYPE) {
        node = create_node(parser, TYPE_SPECIFIER_NODE, sizeof(type_specifier_node));
        if (node == NULL) {
            return NULL;
        }

        ((type_specifier_node *)node->actual_node)->subtype = token->type;
        return node;
    }

    if (token->type == IDENTIFIER) {
        char *id_name;

        node = create_node(parser, IDENTIFIER_NODE, sizeof(identifier_node));
        if (node == NULL) {
            return NULL;
        }

        id_name = duplicate_string(parser, token->value);
        if (id_name == NULL) {
            free(node->actual_node);
            free(node);
            return NULL;
        }

        ((identifier_node *)node->actual_node)->id_name = id_name;
        return node;
    }

    if (token->type == EQ_ASSIGN) {
        node = create_node(parser, ASSIGN_OP_NODE, sizeof(assign_op_node));
        if (node == NULL) {
            return NULL;
        }

        ((assign_op_node *)node->actual_node)->subtype = token->type;
        ((assign_op_node *)node->actual_node)->op_priority = 3;
        return node;
    }

    parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                     "Unexpected token '%s' while parsing", token->value == NULL ? "<null>" : token->value);
    return NULL;
}

static bool append_node(parser_handle parser, node_t ***nodes, int *count, int *capacity, node_t *node) {
    node_t **grown;
    int new_capacity;

    if (*count < *capacity) {
        (*nodes)[(*count)++] = node;
        return true;
    }

    new_capacity = (*capacity == 0) ? 8 : (*capacity * 2);
    grown = (node_t **)realloc(*nodes, (size_t)new_capacity * sizeof(node_t *));
    if (grown == NULL) {
        parser_set_error(parser, TCCI_STATUS_OOM, "Failed to grow parser node buffer");
        return false;
    }

    *nodes = grown;
    *capacity = new_capacity;
    (*nodes)[(*count)++] = node;
    return true;
}

static node_t *assemble_tree(parser_handle parser, node_t **nodes, int start_index, int end_index) {
    int max_priority = -100000;
    int least_prioritized_index = -1;
    int i;
    node_t *current_node;
    node_t *left_child;
    node_t *right_child;

    if (end_index - start_index == 1) {
        return nodes[start_index];
    }

    for (i = start_index; i < end_index; i++) {
        current_node = nodes[i];
        if ((current_node->type == DECLARATION_NODE || current_node->type == BINARY_OP_NODE ||
             current_node->type == ASSIGN_OP_NODE) &&
            op_priority(current_node) >= max_priority) {
            max_priority = op_priority(current_node);
            least_prioritized_index = i;
        }
    }

    if (least_prioritized_index < 0) {
        parser_set_error(parser, TCCI_STATUS_PARSER_ERROR, "Invalid expression: missing operator");
        return NULL;
    }

    if (least_prioritized_index == start_index || least_prioritized_index == end_index - 1) {
        parser_set_error(parser, TCCI_STATUS_PARSER_ERROR, "Invalid expression: operator missing operand");
        return NULL;
    }

    current_node = nodes[least_prioritized_index];

    left_child = assemble_tree(parser, nodes, start_index, least_prioritized_index);
    if (left_child == NULL) {
        return NULL;
    }

    right_child = assemble_tree(parser, nodes, least_prioritized_index + 1, end_index);
    if (right_child == NULL) {
        return NULL;
    }

    if (current_node->type == DECLARATION_NODE) {
        if (right_child->type == IDENTIFIER_NODE) {
            right_child->type = DECLARATOR_NODE;
        } else if (right_child->type == ASSIGN_OP_NODE && get_left_child(right_child) != NULL &&
                   get_left_child(right_child)->type == IDENTIFIER_NODE) {
            get_left_child(right_child)->type = DECLARATOR_NODE;
        }
    }

    set_left_child(current_node, left_child);
    set_right_child(current_node, right_child);
    return current_node;
}

static void free_statement_forest(node_t **nodes, int node_count) {
    bool *is_child;
    int i;
    int j;

    if (nodes == NULL || node_count <= 0) {
        return;
    }

    is_child = (bool *)calloc((size_t)node_count, sizeof(bool));
    if (is_child == NULL) {
        return;
    }

    for (i = 0; i < node_count; i++) {
        node_t *left = get_left_child(nodes[i]);
        node_t *right = get_right_child(nodes[i]);

        for (j = 0; j < node_count; j++) {
            if (nodes[j] == left || nodes[j] == right) {
                is_child[j] = true;
            }
        }
    }

    for (i = 0; i < node_count; i++) {
        if (!is_child[i]) {
            free_node(nodes[i]);
        }
    }

    free(is_child);
}

static bool validate_statement(parser_handle parser, node_t *statement_root) {
    if (statement_root->type == DECLARATION_NODE) {
        node_t *left = get_left_child(statement_root);
        node_t *right = get_right_child(statement_root);

        if (left == NULL || left->type != TYPE_SPECIFIER_NODE) {
            parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                             "Declaration requires an explicit type specifier");
            return false;
        }

        if (right == NULL) {
            parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                             "Declaration requires a declarator");
            return false;
        }

        if (right->type == DECLARATOR_NODE) {
            return true;
        }

        if (right->type == ASSIGN_OP_NODE && get_left_child(right) != NULL &&
            get_left_child(right)->type == DECLARATOR_NODE) {
            return true;
        }

        parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                         "Declaration must assign to a declarator identifier");
        return false;
    }

    if (statement_root->type == ASSIGN_OP_NODE) {
        node_t *left = get_left_child(statement_root);

        if (left == NULL || left->type != IDENTIFIER_NODE) {
            parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                             "Assignment target must be an identifier");
            return false;
        }
    }

    return true;
}

node_t *parse(parser_handle parser) {
    node_t *root;
    compound_statement_node *compound;
    node_t **statement_nodes = NULL;
    int statement_count = 0;
    int statement_capacity = 0;
    node_t **children = NULL;
    int children_count = 0;
    int children_capacity = 0;

    if (parser == NULL) {
        return NULL;
    }

    parser->current = 0;
    parser->opened_paren_count = 0;
    parser->status = TCCI_STATUS_OK;
    parser->error_message[0] = '\0';

    while (has_next(parser)) {
        token_t *token = parser->tokens[parser->current++];

        if (token->type == OPEN_PAREN) {
            parser->opened_paren_count++;
            continue;
        }

        if (token->type == CLOSE_PAREN) {
            parser->opened_paren_count--;
            if (parser->opened_paren_count < 0) {
                parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                                 "Mismatched parentheses: unexpected ')' token");
                goto fail;
            }
            continue;
        }

        if (token->type == SEMICOLON) {
            node_t *statement_root;

            if (statement_count == 0) {
                parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                                 "Empty statement is not allowed");
                goto fail;
            }

            if (parser->opened_paren_count != 0) {
                parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                                 "Mismatched parentheses before ';'");
                goto fail;
            }

            statement_root = assemble_tree(parser, statement_nodes, 0, statement_count);
            if (statement_root == NULL) {
                goto fail;
            }
            statement_count = 0;

            if (!validate_statement(parser, statement_root)) {
                free_node(statement_root);
                goto fail;
            }

            if (!append_node(parser, &children, &children_count, &children_capacity, statement_root)) {
                free_node(statement_root);
                goto fail;
            }
            continue;
        }

        {
            node_t *node = token_to_node(parser, token);

            if (node == NULL) {
                goto fail;
            }

            if (!append_node(parser, &statement_nodes, &statement_count, &statement_capacity, node)) {
                free_node(node);
                goto fail;
            }

            if (node->type == TYPE_SPECIFIER_NODE) {
                node_t *decl = generate_decl_token(parser);
                if (decl == NULL) {
                    goto fail;
                }

                if (!append_node(parser, &statement_nodes, &statement_count, &statement_capacity, decl)) {
                    free_node(decl);
                    goto fail;
                }
            }
        }
    }

    if (parser->opened_paren_count != 0) {
        parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                         "Mismatched parentheses: missing ')' token");
        goto fail;
    }

    if (statement_count > 0) {
        parser_set_error(parser, TCCI_STATUS_PARSER_ERROR,
                         "Missing semicolon at end of statement");
        goto fail;
    }

    root = (node_t *)malloc(sizeof(node_t));
    if (root == NULL) {
        parser_set_error(parser, TCCI_STATUS_OOM, "Failed to allocate compound root node");
        goto fail;
    }

    compound = (compound_statement_node *)malloc(sizeof(compound_statement_node));
    if (compound == NULL) {
        free(root);
        parser_set_error(parser, TCCI_STATUS_OOM, "Failed to allocate compound root payload");
        goto fail;
    }

    root->type = COMPOUND_STATEMENT_NODE;
    root->actual_node = compound;
    compound->children_count = children_count;
    compound->children = children;

    free(statement_nodes);
    return root;

fail:
    if (children != NULL) {
        int i;
        for (i = 0; i < children_count; i++) {
            free_node(children[i]);
        }
        free(children);
    }

    if (statement_nodes != NULL) {
        free_statement_forest(statement_nodes, statement_count);
        free(statement_nodes);
    }

    return NULL;
}

void free_node(node_t *node) {
    if (node == NULL) {
        return;
    }

    if (node->type == BINARY_OP_NODE) {
        binary_op_node *payload = (binary_op_node *)node->actual_node;
        free_node(payload->left_child);
        free_node(payload->right_child);
        free(payload);
    } else if (node->type == ASSIGN_OP_NODE) {
        assign_op_node *payload = (assign_op_node *)node->actual_node;
        free_node(payload->left_child);
        free_node(payload->right_child);
        free(payload);
    } else if (node->type == DECLARATION_NODE) {
        declaration_node *payload = (declaration_node *)node->actual_node;
        free_node(payload->left_child);
        free_node(payload->right_child);
        free(payload);
    } else if (node->type == IDENTIFIER_NODE || node->type == DECLARATOR_NODE) {
        identifier_node *payload = (identifier_node *)node->actual_node;
        free(payload->id_name);
        free(payload);
    } else if (node->type == CONSTANT_NODE) {
        free(node->actual_node);
    } else if (node->type == TYPE_SPECIFIER_NODE) {
        free(node->actual_node);
    } else if (node->type == COMPOUND_STATEMENT_NODE) {
        int i;
        compound_statement_node *payload = (compound_statement_node *)node->actual_node;

        for (i = 0; i < payload->children_count; i++) {
            free_node(payload->children[i]);
        }

        free(payload->children);
        free(payload);
    } else {
        free(node->actual_node);
    }

    free(node);
}
