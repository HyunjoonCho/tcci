#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
    #include "../src/interpreter.h"
}

#include "helper.hpp"

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
    node_t *root = create_declaration_node(create_type_specifier_node(INTEGER_TYPE),
                                           create_identifier_node("x", DECLARATOR_NODE, IDENTIFIER));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    literal_t *id = get_value_of(interpreter, "x");
    EXPECT_EQ(id->type, INTEGER_CONST);
    EXPECT_EQ(id->value.int_value, 0);
}

TEST(InterpreterDeclarations, AssignInteger) {
    node_t *root = create_declaration_node(create_type_specifier_node(INTEGER_TYPE),
                                           create_assign_op_node(EQ_ASSIGN,
                                                                 create_identifier_node("x", DECLARATOR_NODE, IDENTIFIER),
                                                                 create_literal_node(12)));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    literal_t *id = get_value_of(interpreter, "x");
    EXPECT_EQ(id->type, INTEGER_CONST);
    EXPECT_EQ(id->value.int_value, 12);
}

TEST(InterpreterDeclarations, AssignFloat) {
    node_t *root = create_declaration_node(create_type_specifier_node(FLOAT_TYPE),
                                           create_assign_op_node(EQ_ASSIGN,
                                                                 create_identifier_node("dude", DECLARATOR_NODE, IDENTIFIER),
                                                                 create_literal_node(0.57f)));
    interpreter_handle interpreter = interpreter_init(root);
    literal_t *result = interpret(interpreter);
    literal_t *id = get_value_of(interpreter, "dude");
    EXPECT_EQ(id->type, FLOAT_CONST);
    EXPECT_FLOAT_EQ(id->value.float_value, 0.57f);
}