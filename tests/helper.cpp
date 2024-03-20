#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

void check_op_node(node_t *op_node, type_t op) {
    EXPECT_EQ(op_node->type, BINARY_OP_NODE);
    EXPECT_EQ(((binary_op_node *)op_node->actual_node)->subtype, op);
}

void check_constant_node(node_t *const_node, int value) {
    EXPECT_EQ(const_node->type, CONSTANT_NODE);
    EXPECT_EQ(((constant_node *)const_node->actual_node)->subtype, INTEGER_CONST);
    EXPECT_EQ(((constant_node *)const_node->actual_node)->value.int_value, value);
}

void check_constant_node(node_t *const_node, float value) {
    EXPECT_EQ(const_node->type, CONSTANT_NODE);
    EXPECT_EQ(((constant_node *)const_node->actual_node)->subtype, FLOAT_CONST);
    EXPECT_FLOAT_EQ(((constant_node *)const_node->actual_node)->value.float_value, value);    
}

void node_equals(node_t *actual, node_t *expected) {
    EXPECT_EQ(actual->type, expected->type);
    if (actual->type == CONSTANT_NODE) {
        constant_node *actual_ = (constant_node *)actual->actual_node;
        constant_node *expected_ = (constant_node *)expected->actual_node;
        EXPECT_EQ(actual_->subtype, expected_->subtype);
        if (actual_->subtype == FLOAT_CONST) EXPECT_FLOAT_EQ(actual_->value.float_value, expected_->value.float_value);
        else if (actual_->subtype == INTEGER_CONST) EXPECT_EQ(actual_->value.int_value, expected_->value.int_value);
    } else if (actual->type == IDENTIFIER_NODE || actual->type == DECLARATOR_NODE) {
        identifier_node *actual_ = (identifier_node *)actual->actual_node;
        identifier_node *expected_ = (identifier_node *)expected->actual_node;
        EXPECT_STREQ(actual_->id_name, expected_->id_name);
    } else if (actual->type == DECLARATION_NODE) {
        declaration_node *actual_ = (declaration_node *)actual->actual_node;
        declaration_node *expected_ = (declaration_node *)expected->actual_node;
        EXPECT_EQ(actual_->subtype, expected_->subtype);
        node_equals(actual_->left_child, expected_->left_child);
        node_equals(actual_->right_child, expected_->right_child);
    } else if (actual->type == TYPE_SPECIFIER_NODE) {
        type_specifier_node *actual_ = (type_specifier_node *)actual->actual_node;
        type_specifier_node *expected_ = (type_specifier_node *)expected->actual_node;
        EXPECT_EQ(actual_->subtype, expected_->subtype);
    } else if (actual->type == BINARY_OP_NODE) {
        binary_op_node *actual_ = (binary_op_node *)actual->actual_node;
        binary_op_node *expected_ = (binary_op_node *)expected->actual_node;
        EXPECT_EQ(actual_->subtype, expected_->subtype);
        node_equals(actual_->left_child, expected_->left_child);
        node_equals(actual_->right_child, expected_->right_child);        
    } else if (actual->type == ASSIGN_OP_NODE) {
        assign_op_node *actual_ = (assign_op_node *)actual->actual_node;
        assign_op_node *expected_ = (assign_op_node *)expected->actual_node;
        EXPECT_EQ(actual_->subtype, expected_->subtype);
        node_equals(actual_->left_child, expected_->left_child);
        node_equals(actual_->right_child, expected_->right_child);
    } else if (actual->type == COMPOUND_STATEMENT_NODE) {
        compound_statment_node *actual_ = (compound_statment_node *)actual->actual_node;
        compound_statment_node *expected_ = (compound_statment_node *)expected->actual_node;
        EXPECT_EQ(actual_->children_count, expected_->children_count);
        for (int i = 0; i < actual_->children_count; i++) {
            node_equals(actual_->children[i], expected_->children[i]);
        }
    }
}

node_t *create_literal_node(int value) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = CONSTANT_NODE;
    node->actual_node = malloc(sizeof(constant_node));
    ((constant_node *)node->actual_node)->subtype = INTEGER_CONST;
    ((constant_node *)node->actual_node)->value.int_value = value;
    return node;
}

node_t *create_literal_node(float value) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = CONSTANT_NODE;
    node->actual_node = malloc(sizeof(constant_node));
    ((constant_node *)node->actual_node)->subtype = FLOAT_CONST;
    ((constant_node *)node->actual_node)->value.float_value = value;
    return node;
}

node_t *create_identifier_node(const char *id_name, node_type n_type, type_t id_type) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = n_type;
    node->actual_node = malloc(sizeof(identifier_node));
    ((identifier_node *)node->actual_node)->id_name = strdup(id_name);
    return node;
}

node_t *create_binary_op_node(type_t op, node_t *left, node_t *right) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = BINARY_OP_NODE;
    node->actual_node = malloc(sizeof(binary_op_node));
    ((binary_op_node *)node->actual_node)->subtype = op;
    ((binary_op_node *)node->actual_node)->left_child = left;
    ((binary_op_node *)node->actual_node)->right_child = right;
    return node;
}

node_t *create_assign_op_node(type_t op, node_t *left, node_t *right) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = ASSIGN_OP_NODE;
    node->actual_node = malloc(sizeof(assign_op_node));
    ((assign_op_node *)node->actual_node)->subtype = op;
    ((assign_op_node *)node->actual_node)->left_child = left;
    ((assign_op_node *)node->actual_node)->right_child = right;
    return node;
}

node_t *create_type_specifier_node(type_t type) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = TYPE_SPECIFIER_NODE;
    node->actual_node = malloc(sizeof(type_specifier_node));
    ((type_specifier_node *)node->actual_node)->subtype = type;
    return node;
}

node_t *create_declaration_node(node_t *left, node_t *right) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = DECLARATION_NODE;
    node->actual_node = malloc(sizeof(declaration_node));
    ((declaration_node *)node->actual_node)->subtype = DECLARATION;
    ((declaration_node *)node->actual_node)->left_child = left;
    ((declaration_node *)node->actual_node)->right_child = right;
    return node;
}

node_t *create_compound_statements_node(node_t **children, int children_count) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->type = COMPOUND_STATEMENT_NODE;
    node->actual_node = malloc(sizeof(compound_statment_node));
    ((compound_statment_node *)node->actual_node)->children_count = children_count;
    ((compound_statment_node *)node->actual_node)->children = (node_t **)malloc(children_count * sizeof(node_t *));
    for (int i = 0; i < children_count; i++) {
       ((node_t **)((compound_statment_node *)node->actual_node)->children)[i] = children[i];
    }
    return node; 
}
