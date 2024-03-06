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

identifier_t *lookup(interpreter_handle interpreter, const char *id_name) {
    for (int i = 0; i < interpreter->count; i++){
        identifier_t *identifier = interpreter->context[i];
        if (strcmp(identifier->id_name, id_name) == 0) return identifier;
    }
    return NULL;
}

literal_t *get_value_of(interpreter_handle interpreter, const char *id_name) {
    identifier_t *identifier = lookup(interpreter, id_name);
    if (identifier == NULL) return NULL;
    else return &(identifier->literal);
}

identifier_t *evaluate_identifier(node_t *node) {
    if (node->type != ID) return NULL;
    identifier_t *identifier = malloc(sizeof(identifier_t));
    identifier->id_name = strdup(((dummy_node *)node->actual_node)->value.id_name);
    return identifier;
}

void initialize_identifier_literal(identifier_t *identifier, type_t specifier_type) {
    if (specifier_type == INTEGER_TYPE) {
        identifier->literal.type = INTEGER_CONST;
        identifier->literal.value.int_value = 0;
    } else {
        identifier->literal.type = FLOAT_CONST;
        identifier->literal.value.float_value = 0.0f;
    }
}

void initialize_identifier_name(identifier_t *identifier, node_t *decl_node) {
    node_t *declarator;
    if (((dummy_node *)decl_node->actual_node)->right_child->type == DECLARATOR) declarator = ((dummy_node *)decl_node->actual_node)->right_child;
    else if (((dummy_node *)((dummy_node *)decl_node->actual_node)->right_child->actual_node)->subtype == EQ_ASSIGN) declarator = ((dummy_node *)((dummy_node *)decl_node->actual_node)->right_child->actual_node)->left_child;
    identifier->id_name = strdup(((dummy_node *)declarator->actual_node)->value.id_name);
}

literal_t *evaluate_ast(interpreter_handle interpreter, node_t *node) {
    if (node->type == DECL) {
        identifier_t *identifier = malloc(sizeof(identifier_t));
        interpreter->context[interpreter->count++] = identifier;
        initialize_identifier_literal(identifier, ((dummy_node *)((dummy_node *)node->actual_node)->left_child->actual_node)->subtype);
        initialize_identifier_name(identifier, node);
        if (((dummy_node *)((dummy_node *)node->actual_node)->right_child->actual_node)->subtype == EQ_ASSIGN) evaluate_ast(interpreter, ((dummy_node *)node->actual_node)->right_child);
    } else if (node->type == BINARY_OP) {
        literal_t *l = evaluate_ast(interpreter, ((dummy_node *)node->actual_node)->left_child);
        literal_t *r = evaluate_ast(interpreter, ((dummy_node *)node->actual_node)->right_child);
        if (l->type == INTEGER_CONST && r->type == INTEGER_CONST && ((dummy_node *)node->actual_node)->subtype != DIVIDE_OPERATOR) {
            if (((dummy_node *)node->actual_node)->subtype == ADD_OPERATOR) l->value.int_value += r->value.int_value;
            else if (((dummy_node *)node->actual_node)->subtype == SUBTRACT_OPERATOR) l->value.int_value -= r->value.int_value;
            else if (((dummy_node *)node->actual_node)->subtype == MULTIPLY_OPERATOR) l->value.int_value *= r->value.int_value;
        } else {
            if (l->type == INTEGER_CONST) {
                l->value.float_value = (float) l->value.int_value;
                l->type = FLOAT_CONST;
            } 
            if (r->type == INTEGER_CONST) r->value.float_value = (float) r->value.int_value;

            if (((dummy_node *)node->actual_node)->subtype == ADD_OPERATOR) l->value.float_value += r->value.float_value;
            else if (((dummy_node *)node->actual_node)->subtype == SUBTRACT_OPERATOR) l->value.float_value -= r->value.float_value;
            else if (((dummy_node *)node->actual_node)->subtype == MULTIPLY_OPERATOR) l->value.float_value *= r->value.float_value;
            else if (((dummy_node *)node->actual_node)->subtype == DIVIDE_OPERATOR) l->value.float_value /= r->value.float_value;
        } 
        free(r);
        return l;
    } else if (node->type == ASSIGN_OP) {
        identifier_t *identifier = lookup(interpreter, ((dummy_node *)((dummy_node *)node->actual_node)->left_child->actual_node)->value.id_name);
        literal_t *value = evaluate_ast(interpreter, ((dummy_node *)node->actual_node)->right_child);
        identifier->literal.value = value->value;
        free(value);
    } else {
        literal_t *literal = malloc(sizeof(literal_t));
        literal->value = ((constant_node *)node->actual_node)->value;
        if (((constant_node *)node->actual_node)->subtype == INTEGER_CONST) literal->type = INTEGER_CONST;
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
