#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
    #include "../src/interpreter.h"
}

expr_t *create_literal_expr(int value) {
    expr_t *literal_expr = (expr_t *)malloc(sizeof(expr_t));
    literal_expr->type = INTEGER_EXPR;
    literal_expr->value.int_value = value;
    return literal_expr;
}

expr_t *create_literal_expr(float value) {
    expr_t *literal_expr = (expr_t *)malloc(sizeof(expr_t));
    literal_expr->type = FLOAT_EXPR;
    literal_expr->value.float_value = value;
    return literal_expr;
}

expr_t *create_binary_op_expr(op_type op, expr_t *left, expr_t *right) {
    expr_t *binary_op_expr = (expr_t *)malloc(sizeof(expr_t));
    binary_op_expr->type = BINARY_OP;
    binary_op_expr->op = op;
    binary_op_expr->left_operand = left;
    binary_op_expr->right_operand = right;
    return binary_op_expr;
}

TEST(InterpreterTest, AdditionInteger) {
    expr_t *root = create_binary_op_expr(ADD,
                                         create_literal_expr(5),
                                         create_literal_expr(3));

    literal_t *result = interpret_expr(root);
    EXPECT_EQ(result->type, INTEGER_LITERAL);
    EXPECT_EQ(result->value.int_value, 8);
    free(result);
}

TEST(InterpreterTest, SubtractionFloat) {
    expr_t *root = create_binary_op_expr(SUBTRACT,
                                         create_literal_expr(10.5f),
                                         create_literal_expr(3.5f));

    literal_t *result = interpret_expr(root);
    EXPECT_EQ(result->type, FLOAT_LITERAL);
    EXPECT_FLOAT_EQ(result->value.float_value, 7.0);
    free(result);
}

TEST(InterpreterTest, MultiplicationMixed) {
    expr_t *root = create_binary_op_expr(MULTIPLY,
                                         create_literal_expr(4),
                                         create_literal_expr(2.5f));

    literal_t *result = interpret_expr(root);
    EXPECT_EQ(result->type, FLOAT_LITERAL);
    EXPECT_FLOAT_EQ(result->value.float_value, 10.0);
    free(result);
}