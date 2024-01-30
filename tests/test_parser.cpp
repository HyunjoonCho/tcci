#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

token_t *generate_test_token(token_type type, const char *value) {
    token_t *new_token = (token_t *)malloc(sizeof(token_t));
    new_token->type = type;

    char *new_value = (char *)malloc(sizeof(*value));
    strcpy(new_value, value);
    new_token->value = new_value;

    return new_token;
}

TEST(ParserTest, ParseSimpleIntegerExpression) {
    // Create an array of tokens representing a simple expression: "2 + 3"
    token_t *tokens[] = {
        generate_test_token(INTEGER, "2"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "3"),
    };

    expr_t *expr = parse_expression(tokens, 3);

    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, ADD);
    EXPECT_EQ(expr->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->value.int_value, 2);
    EXPECT_EQ(expr->right_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->right_operand->value.int_value, 3);
}

TEST(ParserTest, ParseSimpleFloatExpression) {
    // Create an array of tokens representing a simple expression: "2 + 3"
    token_t *tokens[] = {
        generate_test_token(FLOAT, "2.7"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(FLOAT, "3.2"),
    };

    expr_t *expr = parse_expression(tokens, 3);

    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, ADD);
    EXPECT_EQ(expr->left_operand->type, FLOAT_EXPR);
    EXPECT_FLOAT_EQ(expr->left_operand->value.float_value, 2.7);
    EXPECT_EQ(expr->right_operand->type, FLOAT_EXPR);
    EXPECT_FLOAT_EQ(expr->right_operand->value.float_value, 3.2);
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

    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, SUBTRACT);
    EXPECT_EQ(expr->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->value.int_value, 2);

    EXPECT_EQ(expr->right_operand->type, BINARY_OP);
    EXPECT_EQ(expr->right_operand->op, MULTIPLY);
    EXPECT_EQ(expr->right_operand->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->right_operand->left_operand->value.int_value, 3);
    EXPECT_EQ(expr->right_operand->right_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->right_operand->right_operand->value.int_value, 4);
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

    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, ADD);

    EXPECT_EQ(expr->left_operand->type, BINARY_OP);
    EXPECT_EQ(expr->left_operand->op, MULTIPLY);
    EXPECT_EQ(expr->left_operand->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->left_operand->value.int_value, 2);
    EXPECT_EQ(expr->left_operand->right_operand->type, FLOAT_EXPR);
    EXPECT_FLOAT_EQ(expr->left_operand->right_operand->value.float_value, 7.9);

    EXPECT_EQ(expr->right_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->right_operand->value.int_value, 9);
}

TEST(ParserTest, ParseIntegerExpressionWithDivision) {
    // Create an array of tokens representing an expression: "8 / 2"
    token_t *tokens[] = {
        generate_test_token(INTEGER, "8"),
        generate_test_token(DIVIDE_OPERATOR, "/"),
        generate_test_token(INTEGER, "2"),
    };

    // Parse the array of tokens
    expr_t *expr = parse_expression(tokens, 3);

    // Assert the parsed expression
    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, DIVIDE);
    EXPECT_EQ(expr->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->value.int_value, 8);
    EXPECT_EQ(expr->right_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->right_operand->value.int_value, 2);
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

    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, MULTIPLY);

    EXPECT_EQ(expr->left_operand->type, BINARY_OP);
    EXPECT_EQ(expr->left_operand->op, ADD);
    EXPECT_EQ(expr->left_operand->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->left_operand->value.int_value, 2);
    EXPECT_EQ(expr->left_operand->right_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->right_operand->value.int_value, 3);

    EXPECT_EQ(expr->right_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->right_operand->value.int_value, 4);
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
    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, SUBTRACT);

    EXPECT_EQ(expr->left_operand->type, BINARY_OP);
    EXPECT_EQ(expr->left_operand->op, ADD);
    EXPECT_EQ(expr->left_operand->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->left_operand->value.int_value, 1);
    EXPECT_EQ(expr->left_operand->right_operand->type, BINARY_OP);
    EXPECT_EQ(expr->left_operand->right_operand->op, MULTIPLY);
    EXPECT_EQ(expr->left_operand->right_operand->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->right_operand->left_operand->value.int_value, 2);
    EXPECT_EQ(expr->left_operand->right_operand->right_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->left_operand->right_operand->right_operand->value.int_value, 3);

    EXPECT_EQ(expr->right_operand->type, BINARY_OP);
    EXPECT_EQ(expr->right_operand->op, DIVIDE);
    EXPECT_EQ(expr->right_operand->left_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->right_operand->left_operand->value.int_value, 4);
    EXPECT_EQ(expr->right_operand->right_operand->type, INTEGER_EXPR);
    EXPECT_EQ(expr->right_operand->right_operand->value.int_value, 2);
}