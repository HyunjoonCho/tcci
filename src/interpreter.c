#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

struct interpreter {
    int count;
    identifier_t **context;
    node_t *root;
};

interpreter_handle interpreter_init(node_t *root) {
    interpreter *new_interpreter = malloc(sizeof(interpreter));
    new_interpreter->count = 0;
    new_interpreter->context = malloc(sizeof(identifier_t *) * 10); // Assume: max 10 identifiers
    new_interpreter->root = root;
    return new_interpreter;
}

literal_t *get_value_of(interpreter_handle interpreter, const char *id_name) {
    for (int i = 0; i < interpreter->count; i++){
        identifier_t *identifier = interpreter->context[i];
        if (strcmp(identifier->id_name, id_name) == 0) return &(identifier->literal);
    }
    return NULL;
}

identifier_t *evaluate_identifier(node_t *node) {
    if (node->type != ID) return NULL;
    identifier_t *identifier = malloc(sizeof(identifier_t));
    identifier->id_name = strdup(node->value.id_name);
    return identifier;
}

literal_t *evaluate_ast(interpreter_handle interpreter, node_t *node) {
    if (node->type == BINARY_OP) {
        literal_t *l = evaluate_ast(interpreter, node->left_child);
        literal_t *r = evaluate_ast(interpreter, node->right_child);
        if (l->type == INTEGER_CONST && r->type == INTEGER_CONST && node->subtype != DIVIDE_OPERATOR) {
            if (node->subtype == ADD_OPERATOR) l->value.int_value += r->value.int_value;
            else if (node->subtype == SUBTRACT_OPERATOR) l->value.int_value -= r->value.int_value;
            else if (node->subtype == MULTIPLY_OPERATOR) l->value.int_value *= r->value.int_value;
        } else {
            if (l->type == INTEGER_CONST) {
                l->value.float_value = (float) l->value.int_value;
                l->type = FLOAT_CONST;
            } 
            if (r->type == INTEGER_CONST) r->value.float_value = (float) r->value.int_value;

            if (node->subtype == ADD_OPERATOR) l->value.float_value += r->value.float_value;
            else if (node->subtype == SUBTRACT_OPERATOR) l->value.float_value -= r->value.float_value;
            else if (node->subtype == MULTIPLY_OPERATOR) l->value.float_value *= r->value.float_value;
            else if (node->subtype == DIVIDE_OPERATOR) l->value.float_value /= r->value.float_value;
        } 
        free(r);
        return l;
    } else if (node->type == ASSIGN_OP) {
        identifier_t *identifier = evaluate_identifier(node->left_child);
        literal_t *value = evaluate_ast(interpreter, node->right_child);
        identifier->literal.type = value->type;
        identifier->literal.value = value->value;
        free(value);
        interpreter->context[interpreter->count++] = identifier;
    } else {
        literal_t *literal = malloc(sizeof(literal_t));
        literal->value = node->value;
        if (node->subtype == INTEGER_CONST) literal->type = INTEGER_CONST;
        else literal->type = FLOAT_CONST;
        return literal;
    }

    literal_t *zero_literal = malloc(sizeof(literal_t));
    zero_literal->type = INTEGER_CONST;
    zero_literal->value.int_value = 0;
    return zero_literal;
}

literal_t *interpret(interpreter_handle interpreter) {
    return evaluate_ast(interpreter, interpreter->root);
}
