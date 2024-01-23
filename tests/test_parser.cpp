#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

TEST(ParserTest, ParseSimpleExpression) {
    // Create an array of tokens representing a simple expression: "2 + 3"
    token_t tokens[] = {
        {INTEGER, "2"},
        {ADD_OPERATOR, "+"},
        {INTEGER, "3"},
    };

    // Parse the array of tokens
    expr_t *expr = parse_expression(tokens, sizeof(tokens) / sizeof(tokens[0]));

    // Assert the parsed expression
    EXPECT_EQ(expr->type, BINARY_OP);
    EXPECT_EQ(expr->op, ADD);
    EXPECT_EQ(expr->left_operand->type, LITERAL);
    EXPECT_STREQ(expr->left_operand->value, "2");
    EXPECT_EQ(expr->right_operand->type, LITERAL);
    EXPECT_STREQ(expr->right_operand->value, "3");
}

TEST(ParserTest, ParseExpressionWithPriority) {
    // Create an array of tokens representing an expression: "2 - 3 * 4"
    token_t tokens[] = {
        {INTEGER, "2"},
        {SUBTRACT_OPERATOR, "-"},
        {INTEGER, "3"},
        {MULTIPLY_OPERATOR, "*"},
        {INTEGER, "4"},
    };

    // Parse the array of tokens
    expr_t *expr = parse_expression(tokens, sizeof(tokens) / sizeof(tokens[0]));

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
