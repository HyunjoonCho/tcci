#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
    #include "../src/interpreter.h"
}

node_t *create_literal_node(int value) {
    node_t *literal_node = (node_t *)malloc(sizeof(node_t));
    literal_node->type = CONSTANT;
    literal_node->subtype = INTEGER_CONST;
    literal_node->value.int_value = value;
    return literal_node;
}

node_t *create_literal_node(float value) {
    node_t *literal_node = (node_t *)malloc(sizeof(node_t));
    literal_node->type = CONSTANT;
    literal_node->subtype = FLOAT_CONST;
    literal_node->value.float_value = value;
    return literal_node;
}

node_t *create_binary_op_node(type_t op, node_t *left, node_t *right) {
    node_t *binary_op_node = (node_t *)malloc(sizeof(node_t));
    binary_op_node->type = BINARY_OP;
    binary_op_node->subtype = op;
    binary_op_node->left_child = left;
    binary_op_node->right_child = right;
    return binary_op_node;
}

TEST(InterpreterTest, AdditionInteger) {
    node_t *root = create_binary_op_node(ADD_OPERATOR,
                                         create_literal_node(5),
                                         create_literal_node(3));

    literal_t *result = interpret(root);
    EXPECT_EQ(result->type, INTEGER_LITERAL);
    EXPECT_EQ(result->value.int_value, 8);
    free(result);
}

TEST(InterpreterTest, SubtractionFloat) {
    node_t *root = create_binary_op_node(SUBTRACT_OPERATOR,
                                         create_literal_node(10.5f),
                                         create_literal_node(3.5f));

    literal_t *result = interpret(root);
    EXPECT_EQ(result->type, FLOAT_LITERAL);
    EXPECT_FLOAT_EQ(result->value.float_value, 7.0);
    free(result);
}

TEST(InterpreterTest, MultiplicationMixed) {
    node_t *root = create_binary_op_node(MULTIPLY_OPERATOR,
                                         create_literal_node(4),
                                         create_literal_node(2.5f));

    literal_t *result = interpret(root);
    EXPECT_EQ(result->type, FLOAT_LITERAL);
    EXPECT_FLOAT_EQ(result->value.float_value, 10.0);
    free(result);
}

TEST(InterpreterTest, DivisionFloat) {
    node_t *root = create_binary_op_node(DIVIDE_OPERATOR,
                                         create_literal_node(15.0f),
                                         create_literal_node(3.0f));

    literal_t *result = interpret(root);
    EXPECT_EQ(result->type, FLOAT_LITERAL);
    EXPECT_FLOAT_EQ(result->value.float_value, 5.0);
    free(result);
}

TEST(InterpreterTest, DivisionInteger) {
    node_t *root = create_binary_op_node(DIVIDE_OPERATOR,
                                         create_literal_node(4),
                                         create_literal_node(8));

    literal_t *result = interpret(root);
    EXPECT_EQ(result->type, FLOAT_LITERAL);
    EXPECT_FLOAT_EQ(result->value.float_value, 0.5);
    free(result);
}

TEST(InterpreterTest, MoreThanThreeOperators) {
    node_t *root = create_binary_op_node(SUBTRACT_OPERATOR,
                                         create_binary_op_node(ADD_OPERATOR,
                                                              create_binary_op_node(MULTIPLY_OPERATOR,
                                                                                   create_literal_node(2),
                                                                                   create_literal_node(3)),
                                                              create_binary_op_node(DIVIDE_OPERATOR,
                                                                                   create_literal_node(5),
                                                                                   create_literal_node(2))),
                                         create_literal_node(1));

    literal_t *result = interpret(root);
    EXPECT_EQ(result->type, FLOAT_LITERAL);
    EXPECT_EQ(result->value.float_value, 7.5);
    free(result);
}