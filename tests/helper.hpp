#ifndef HELPER_HPP
#define HELPER_HPP

extern "C" {
    #include "../src/commons.h"
}

void check_op_node(node_t *op_node, type_t op);
void check_constant_node(node_t *const_node, int value);
void check_constant_node(node_t *const_node, float value);
void node_equals(node_t *actual, node_t *expected);

node_t *create_literal_node(int value);
node_t *create_literal_node(float value);
node_t *create_identifier_node(const char *id_name, node_type n_type, type_t id_type);
node_t *create_binary_op_node(type_t op, node_t *left, node_t *right);
node_t *create_assign_op_node(type_t op, node_t *left, node_t *right);
node_t *create_type_specifier_node(type_t type);
node_t *create_declaration_node(node_t *left, node_t *right);
node_t *create_compound_statements_node(node_t **children, int children_count);
#endif