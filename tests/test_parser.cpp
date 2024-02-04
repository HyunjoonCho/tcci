#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

token_t *generate_test_token(token_type type, const char *value) {
    token_t *new_token = (token_t *)malloc(sizeof(token_t));
    new_token->type = type;
    new_token->value = strdup(value);

    return new_token;
}


void check_op_expr(expr_t *op_expr, op_type op) {
    EXPECT_EQ(op_expr->type, BINARY_OP);
    EXPECT_EQ(op_expr->subtype.op_t, op);
}

void check_constant_expr(expr_t *const_expr, int value) {
    EXPECT_EQ(const_expr->type, CONSTANT);
    EXPECT_EQ(const_expr->subtype.const_t, INTEGER_CONST);
    EXPECT_EQ(const_expr->value.int_value, value);
}

void check_constant_expr(expr_t *const_expr, float value) {
    EXPECT_EQ(const_expr->type, CONSTANT);
    EXPECT_EQ(const_expr->subtype.const_t, FLOAT_CONST);
    EXPECT_FLOAT_EQ(const_expr->value.float_value, value);    
}

TEST(ParserTest, ParseSimpleIntegerExpression) {
    // Create an array of tokens representing a simple expression: "2 + 3"
    token_t *tokens[] = {
        generate_test_token(INTEGER, "2"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "3"),
    };

    expr_t *expr = parse_expression(tokens, 3);

    check_op_expr(expr, ADD);
    check_constant_expr(expr->left_operand, 2);
    check_constant_expr(expr->right_operand, 3);
}

TEST(ParserTest, ParseSimpleFloatExpression) {
    // Create an array of tokens representing a simple expression: "2 + 3"
    token_t *tokens[] = {
        generate_test_token(FLOAT, "2.7"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(FLOAT, "3.2"),
    };

    expr_t *expr = parse_expression(tokens, 3);

    check_op_expr(expr, ADD);
    check_constant_expr(expr->left_operand, 2.7f);
    check_constant_expr(expr->right_operand, 3.2f);
}

TEST(ParserTest, ParseIntegerExpressionWithPriority) {
    // Create an array of tokens representing an expression: "2 - 3 * 4"
    token_t *tokens[] = {
        generate_test_token(INTEGER, "2"),
        generate_test_token(SUBTRACT_OPERATOR, "-"),
        generate_test_token(INTEGER, "3"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER, "4"),
    };

    expr_t *expr = parse_expression(tokens, 5);

    check_op_expr(expr, SUBTRACT);
    check_constant_expr(expr->left_operand, 2);

    check_op_expr(expr->right_operand, MULTIPLY);
    check_constant_expr(expr->right_operand->left_operand, 3);
    check_constant_expr(expr->right_operand->right_operand, 4);
}

TEST(ParserTest, ParseMixedExpressionWithPriority) {
    // Create an array of tokens representing an expression: "2 * 7.9 + 9"
    token_t *tokens[] = {
        generate_test_token(INTEGER, "2"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(FLOAT, "7.9"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "9"),
    };

    expr_t *expr = parse_expression(tokens, 5);

    check_op_expr(expr, ADD);

    check_op_expr(expr->left_operand, MULTIPLY);
    check_constant_expr(expr->left_operand->left_operand, 2);
    check_constant_expr(expr->left_operand->right_operand, 7.9f);
    check_constant_expr(expr->right_operand, 9);
}

TEST(ParserTest, ParseIntegerExpressionWithDivision) {
    // Create an array of tokens representing an expression: "8 / 2"
    token_t *tokens[] = {
        generate_test_token(INTEGER, "8"),
        generate_test_token(DIVIDE_OPERATOR, "/"),
        generate_test_token(INTEGER, "2"),
    };

    expr_t *expr = parse_expression(tokens, 3);

    check_op_expr(expr, DIVIDE);
    check_constant_expr(expr->left_operand, 8);
    check_constant_expr(expr->right_operand, 2);
}

TEST(ParserTest, ParseMixedExpressionWithParentheses) {
    // Create an array of tokens representing an expression: "(2 + 3) * 4"
    token_t *tokens[] = {
        generate_test_token(OPEN_PAREN, "("),
        generate_test_token(INTEGER, "2"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "3"),
        generate_test_token(CLOSE_PAREN, ")"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER, "4"),
    };

    expr_t *expr = parse_expression(tokens, 7);

    check_op_expr(expr, MULTIPLY);

    check_op_expr(expr->left_operand, ADD);
    check_constant_expr(expr->left_operand->left_operand, 2);
    check_constant_expr(expr->left_operand->right_operand, 3);
    check_constant_expr(expr->right_operand, 4);
}

TEST(ParserTest, ParseMoreThanThreeOperators) {
    // Create an array of tokens representing an expression: "1 + 2 * 3 - 4 / 2"
    token_t *tokens[] = {
        generate_test_token(INTEGER, "1"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "2"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER, "3"),
        generate_test_token(SUBTRACT_OPERATOR, "-"),
        generate_test_token(INTEGER, "4"),
        generate_test_token(DIVIDE_OPERATOR, "/"),
        generate_test_token(INTEGER, "2"),
    };

    expr_t *expr = parse_expression(tokens, 9);

    // TODO: dependency to op prioritizing algorithm > better correctness check needed
    // for this case, ADD may come first
    check_op_expr(expr, SUBTRACT);

    check_op_expr(expr->left_operand, ADD);
    check_constant_expr(expr->left_operand->left_operand, 1);

    check_op_expr(expr->left_operand->right_operand, MULTIPLY);
    check_constant_expr(expr->left_operand->right_operand->left_operand, 2);
    check_constant_expr(expr->left_operand->right_operand->right_operand, 3);

    check_op_expr(expr->right_operand, DIVIDE);
    check_constant_expr(expr->right_operand->left_operand, 4);
    check_constant_expr(expr->right_operand->right_operand, 2);
}