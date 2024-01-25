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

    // Parse the array of tokens
    expr_t *expr = parse_expression(tokens, 3);

    // Assert the parsed expression
    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, ADD);
    EXPECT_EQ(expr->left_operand->type, LITERAL);
    EXPECT_STREQ(expr->left_operand->value, "2");
    EXPECT_EQ(expr->right_operand->type, LITERAL);
    EXPECT_STREQ(expr->right_operand->value, "3");
}

TEST(ParserTest, ParseSimpleFloatExpression) {
    // Create an array of tokens representing a simple expression: "2 + 3"
    token_t *tokens[] = {
        generate_test_token(FLOAT, "2.7"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(FLOAT, "3.2"),
    };

    // Parse the array of tokens
    expr_t *expr = parse_expression(tokens, 3);

    // Assert the parsed expression
    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, ADD);
    EXPECT_EQ(expr->left_operand->type, LITERAL);
    EXPECT_STREQ(expr->left_operand->value, "2.7");
    EXPECT_EQ(expr->right_operand->type, LITERAL);
    EXPECT_STREQ(expr->right_operand->value, "3.2");
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

    // Parse the array of tokens
    expr_t *expr = parse_expression(tokens, 5);

    // Assert the parsed expression
    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, SUBTRACT);
    EXPECT_EQ(expr->left_operand->type, LITERAL);
    EXPECT_STREQ(expr->left_operand->value, "2");

    // Right operand should be another binary op
    EXPECT_EQ(expr->right_operand->type, BINARY_OP);
    EXPECT_EQ(expr->right_operand->op, MULTIPLY);
    EXPECT_EQ(expr->right_operand->left_operand->type, LITERAL);
    EXPECT_STREQ(expr->right_operand->left_operand->value, "3");
    EXPECT_EQ(expr->right_operand->right_operand->type, LITERAL);
    EXPECT_STREQ(expr->right_operand->right_operand->value, "4");
}

TEST(ParserTest, ParseMixedExpressionWithPriority) {
    // Create an array of tokens representing an expression: "2 - 3 * 4"
    token_t *tokens[] = {
        generate_test_token(INTEGER, "2"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(FLOAT, "7.9"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "9"),
    };

    // Parse the array of tokens
    expr_t *expr = parse_expression(tokens, 5);

    // Assert the parsed expression
    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, ADD);

    EXPECT_EQ(expr->left_operand->type, BINARY_OP);
    EXPECT_EQ(expr->left_operand->op, MULTIPLY);
    EXPECT_EQ(expr->left_operand->left_operand->type, LITERAL);
    EXPECT_STREQ(expr->left_operand->left_operand->value, "2");
    EXPECT_EQ(expr->left_operand->right_operand->type, LITERAL);
    EXPECT_STREQ(expr->left_operand->right_operand->value, "7.9");

    EXPECT_EQ(expr->right_operand->type, LITERAL);
    EXPECT_STREQ(expr->right_operand->value, "9");
}