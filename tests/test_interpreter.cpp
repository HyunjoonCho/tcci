#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
    #include "../src/interpreter.h"
}

node_t *create_literal_node(int value) {
    node_t *literal_node = (node_t *)malloc(sizeof(node_t));
    literal_node->type = CONSTANT;
    literal_node->actual_node = malloc(sizeof(constant_node));
    ((constant_node *)literal_node->actual_node)->subtype = INTEGER_CONST;
    ((constant_node *)literal_node->actual_node)->value.int_value = value;
    return literal_node;
}

node_t *create_literal_node(float value) {
    node_t *literal_node = (node_t *)malloc(sizeof(node_t));
    literal_node->type = CONSTANT;
    literal_node->actual_node = malloc(sizeof(constant_node));
    ((constant_node *)literal_node->actual_node)->subtype = FLOAT_CONST;
    ((constant_node *)literal_node->actual_node)->value.float_value = value;
    return literal_node;
}

node_t *create_identifier_node(const char *id_name, node_type n_type, type_t id_type) {
    node_t *id_node = (node_t *)malloc(sizeof(node_t));
    id_node->type = n_type;
    id_node->actual_node = malloc(sizeof(dummy_node));
    ((dummy_node *)id_node->actual_node)->subtype = id_type;
    ((dummy_node *)id_node->actual_node)->value.id_name = strdup(id_name);
    return id_node;
}

node_t *create_binary_op_node(type_t op, node_t *left, node_t *right) {
    node_t *binary_op_node = (node_t *)malloc(sizeof(node_t));
    binary_op_node->type = BINARY_OP;
    binary_op_node->actual_node = malloc(sizeof(dummy_node));
    ((dummy_node *)binary_op_node->actual_node)->subtype = op;
    ((dummy_node *)binary_op_node->actual_node)->left_child = left;
    ((dummy_node *)binary_op_node->actual_node)->right_child = right;
    return binary_op_node;
}

node_t *create_assign_op_node(type_t op, node_t *left, node_t *right) {
    node_t *assign_op_node = (node_t *)malloc(sizeof(node_t));
    assign_op_node->type = ASSIGN_OP;
    assign_op_node->actual_node = malloc(sizeof(dummy_node));
    ((dummy_node *)assign_op_node->actual_node)->subtype = op;
    ((dummy_node *)assign_op_node->actual_node)->left_child = left;
    ((dummy_node *)assign_op_node->actual_node)->right_child = right;
    return assign_op_node;
}

node_t *create_type_specifier_node(type_t type) {
    node_t *type_specifier_node = (node_t *)malloc(sizeof(node_t));
    type_specifier_node->type = TYPE_SPECIFIER;
    type_specifier_node->actual_node = malloc(sizeof(dummy_node));
    ((dummy_node *)type_specifier_node->actual_node)->subtype = type;
    return type_specifier_node;
}

node_t *create_declaration_node(node_t *left, node_t *right) {
    node_t *declaration_node = (node_t *)malloc(sizeof(node_t));
    declaration_node->type = DECL;
    declaration_node->actual_node = malloc(sizeof(dummy_node));
    ((dummy_node *)declaration_node->actual_node)->subtype = DECLARATION;
    ((dummy_node *)declaration_node->actual_node)->left_child = left;
    ((dummy_node *)declaration_node->actual_node)->right_child = right;
    return declaration_node;
}

TEST(InterpreterArithmetic, AdditionInteger) {
    node_t *root = create_binary_op_node(ADD_OPERATOR,
                                         create_literal_node(5),
                                         create_literal_node(3));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, INTEGER_CONST);
    EXPECT_EQ(result->value.int_value, 8);
    free(result);
}

TEST(InterpreterArithmetic, SubtractionFloat) {
    node_t *root = create_binary_op_node(SUBTRACT_OPERATOR,
                                         create_literal_node(10.5f),
                                         create_literal_node(3.5f));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(result->value.float_value, 7.0);
    free(result);
}

TEST(InterpreterArithmetic, MultiplicationMixed) {
    node_t *root = create_binary_op_node(MULTIPLY_OPERATOR,
                                         create_literal_node(4),
                                         create_literal_node(2.5f));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(result->value.float_value, 10.0);
    free(result);
}

TEST(InterpreterArithmetic, DivisionFloat) {
    node_t *root = create_binary_op_node(DIVIDE_OPERATOR,
                                         create_literal_node(15.0f),
                                         create_literal_node(3.0f));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(result->value.float_value, 5.0);
    free(result);
}

TEST(InterpreterArithmetic, DivisionInteger) {
    node_t *root = create_binary_op_node(DIVIDE_OPERATOR,
                                         create_literal_node(4),
                                         create_literal_node(8));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    EXPECT_EQ(result->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(result->value.float_value, 0.5);
    free(result);
}

TEST(InterpreterArithmetic, MoreThanThreeOperators) {
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

TEST(InterpreterDeclarations, DeclareInteger) {
    node_t *root = create_declaration_node(
                                          create_type_specifier_node(INTEGER_TYPE),
                                          create_identifier_node("x", DECLARATOR, INTEGER_TYPE));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    literal_t *id = get_value_of(interpreter, "x");
    EXPECT_EQ(id->type, INTEGER_CONST);
    EXPECT_EQ(id->value.int_value, 0);
}

TEST(InterpreterDeclarations, AssignInteger) {
    node_t *root = create_declaration_node(
                                          create_type_specifier_node(INTEGER_TYPE),
                                          create_assign_op_node(EQ_ASSIGN,
                                                               create_identifier_node("x", DECLARATOR, INTEGER_TYPE),
                                                               create_literal_node(12)));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    literal_t *id = get_value_of(interpreter, "x");
    EXPECT_EQ(id->type, INTEGER_CONST);
    EXPECT_EQ(id->value.int_value, 12);
}

TEST(InterpreterDeclarations, AssignFloat) {
    node_t *root = create_declaration_node(
                                          create_type_specifier_node(FLOAT_TYPE),
                                          create_assign_op_node(EQ_ASSIGN,
                                                               create_identifier_node("dude", DECLARATOR, FLOAT_TYPE),
                                                               create_literal_node(0.57f)));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    literal_t *id = get_value_of(interpreter, "dude");
    EXPECT_EQ(id->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(id->value.float_value, 0.57f);
}