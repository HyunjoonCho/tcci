#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

void check_op_node(node_t *op_node, type_t op) {
    EXPECT_EQ(op_node->type, BINARY_OP);
    EXPECT_EQ(((binary_op_node *)op_node->actual_node)->subtype, op);
}

void check_constant_node(node_t *const_node, int value) {
    EXPECT_EQ(const_node->type, CONSTANT);
    EXPECT_EQ(((constant_node *)const_node->actual_node)->subtype, INTEGER_CONST);
    EXPECT_EQ(((constant_node *)const_node->actual_node)->value.int_value, value);
}

void check_constant_node(node_t *const_node, float value) {
    EXPECT_EQ(const_node->type, CONSTANT);
    EXPECT_EQ(((constant_node *)const_node->actual_node)->subtype, FLOAT_CONST);
    EXPECT_FLOAT_EQ(((constant_node *)const_node->actual_node)->value.float_value, value);    
}

node_t *create_literal_node(int value) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = CONSTANT;
    node->actual_node = malloc(sizeof(constant_node));
    ((constant_node *)node->actual_node)->subtype = INTEGER_CONST;
    ((constant_node *)node->actual_node)->value.int_value = value;
    return node;
}

node_t *create_literal_node(float value) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = CONSTANT;
    node->actual_node = malloc(sizeof(constant_node));
    ((constant_node *)node->actual_node)->subtype = FLOAT_CONST;
    ((constant_node *)node->actual_node)->value.float_value = value;
    return node;
}

node_t *create_identifier_node(const char *id_name, node_type n_type, type_t id_type) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = n_type;
    node->actual_node = malloc(sizeof(identifier_node));
    ((identifier_node *)node->actual_node)->subtype = id_type;
    ((identifier_node *)node->actual_node)->id_name = strdup(id_name);
    return node;
}

node_t *create_binary_op_node(type_t op, node_t *left, node_t *right) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = BINARY_OP;
    node->actual_node = malloc(sizeof(binary_op_node));
    ((binary_op_node *)node->actual_node)->subtype = op;
    ((binary_op_node *)node->actual_node)->left_child = left;
    ((binary_op_node *)node->actual_node)->right_child = right;
    return node;
}

node_t *create_assign_op_node(type_t op, node_t *left, node_t *right) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = ASSIGN_OP;
    node->actual_node = malloc(sizeof(assign_op_node));
    ((assign_op_node *)node->actual_node)->subtype = op;
    ((assign_op_node *)node->actual_node)->left_child = left;
    ((assign_op_node *)node->actual_node)->right_child = right;
    return node;
}

node_t *create_type_specifier_node(type_t type) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = TYPE_SPECIFIER;
    node->actual_node = malloc(sizeof(type_specifier_node));
    ((type_specifier_node *)node->actual_node)->subtype = type;
    return node;
}

node_t *create_declaration_node(node_t *left, node_t *right) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = DECL;
    node->actual_node = malloc(sizeof(declaration_node));
    ((declaration_node *)node->actual_node)->subtype = DECLARATION;
    ((declaration_node *)node->actual_node)->left_child = left;
    ((declaration_node *)node->actual_node)->right_child = right;
    return node;
}
