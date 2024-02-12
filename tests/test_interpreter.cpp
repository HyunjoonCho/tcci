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

node_t *create_identifier_node(const char *id_name, type_t id_type) {
    node_t *id_node = (node_t *)malloc(sizeof(node_t));
    id_node->type = ID;
    id_node->subtype = id_type;
    id_node->value.id_name = strdup(id_name);
    return id_node;
}

node_t *create_binary_op_node(type_t op, node_t *left, node_t *right) {
    node_t *binary_op_node = (node_t *)malloc(sizeof(node_t));
    binary_op_node->type = BINARY_OP;
    binary_op_node->subtype = op;
    binary_op_node->left_child = left;
    binary_op_node->right_child = right;
    return binary_op_node;
}

node_t *create_assign_op_node(type_t op, node_t *left, node_t *right) {
    node_t *assign_op_node = (node_t *)malloc(sizeof(node_t));
    assign_op_node->type = ASSIGN_OP;
    assign_op_node->subtype = op;
    assign_op_node->left_child = left;
    assign_op_node->right_child = right;
    return assign_op_node;
}

TEST(InterpreterTest, AdditionInteger) {
    node_t *root = create_binary_op_node(ADD_OPERATOR,
                                         create_literal_node(5),
                                         create_literal_node(3));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, INTEGER_CONST);
    EXPECT_EQ(result->value.int_value, 8);
    free(result);
}

TEST(InterpreterTest, SubtractionFloat) {
    node_t *root = create_binary_op_node(SUBTRACT_OPERATOR,
                                         create_literal_node(10.5f),
                                         create_literal_node(3.5f));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(result->value.float_value, 7.0);
    free(result);
}

TEST(InterpreterTest, MultiplicationMixed) {
    node_t *root = create_binary_op_node(MULTIPLY_OPERATOR,
                                         create_literal_node(4),
                                         create_literal_node(2.5f));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(result->value.float_value, 10.0);
    free(result);
}

TEST(InterpreterTest, DivisionFloat) {
    node_t *root = create_binary_op_node(DIVIDE_OPERATOR,
                                         create_literal_node(15.0f),
                                         create_literal_node(3.0f));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(result->value.float_value, 5.0);
    free(result);
}

TEST(InterpreterTest, DivisionInteger) {
    node_t *root = create_binary_op_node(DIVIDE_OPERATOR,
                                         create_literal_node(4),
                                         create_literal_node(8));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
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
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
    EXPECT_EQ(result->value.float_value, 7.5);
    free(result);
}

TEST(InterpreterTest, AssignInteger) {
    node_t *root = create_assign_op_node(EQ_ASSIGN,
                                         create_identifier_node("x", INTEGER_TYPE),
                                         create_literal_node(12));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    literal_t *id = get_value_of(interpreter, "x");
    EXPECT_EQ(id->type, INTEGER_CONST);
    EXPECT_EQ(id->value.int_value, 12);
}

TEST(InterpreterTest, AssignFloat) {
    node_t *root = create_assign_op_node(EQ_ASSIGN,
                                         create_identifier_node("dude", FLOAT_TYPE),
                                         create_literal_node(0.57f));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    literal_t *id = get_value_of(interpreter, "dude");
    EXPECT_EQ(id->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(id->value.float_value, 0.57f);
}