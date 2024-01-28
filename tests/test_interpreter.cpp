#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
    #include "../src/interpreter.h"
}

expr_t *create_literal_expr(token_type type, const char *value) {
    expr_t *literal_expr = (expr_t *)malloc(sizeof(expr_t));
    literal_expr->type = LITERAL;
    literal_expr->value = strdup(value);
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
                                         create_literal_expr(INTEGER, "5"),
                                         create_literal_expr(INTEGER, "3"));

    literal_t *result = interpret_expr(root);
    EXPECT_TRUE(result->is_int);
    EXPECT_EQ(result->value.int_value, 8);
    free(result);
}

TEST(InterpreterTest, SubtractionFloat) {
    expr_t *root = create_binary_op_expr(SUBTRACT,
                                         create_literal_expr(FLOAT, "10.5"),
                                         create_literal_expr(FLOAT, "3.5"));

    literal_t *result = interpret_expr(root);
    EXPECT_FALSE(result->is_int);
    EXPECT_FLOAT_EQ(result->value.float_value, 7.0);
    free(result);
}

TEST(InterpreterTest, MultiplicationMixed) {
    expr_t *root = create_binary_op_expr(MULTIPLY,
                                         create_literal_expr(INTEGER, "4"),
                                         create_literal_expr(FLOAT, "2.5"));

    literal_t *result = interpret_expr(root);
    EXPECT_FALSE(result->is_int);
    EXPECT_FLOAT_EQ(result->value.float_value, 10.0);
    free(result);
}