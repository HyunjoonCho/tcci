#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

void check_token(token_t *token, token_type expected_type, const char *expected_value) {
    EXPECT_EQ(token->type, expected_type);
    EXPECT_STREQ(token->value, expected_value);
}

TEST(SingleTokenTest, IntegerToken) {
    tokenizer_handle tokenizer = tokenizer_init("123");
    token_t *token = get_next_token(tokenizer);
    check_token(token, INTEGER, "123");
}

TEST(SingleTokenTest, FloatToken) {
    tokenizer_handle tokenizer = tokenizer_init("12.34");
    token_t *token = get_next_token(tokenizer);
    check_token(token, FLOAT, "12.34");
}

TEST(SingleTokenTest, AddOperatorToken) {
    tokenizer_handle tokenizer = tokenizer_init("+");
    token_t *token = get_next_token(tokenizer);
    check_token(token, ADD_OPERATOR, "+");
}

TEST(SingleTokenTest, SubtractOperatorToken) {
    tokenizer_handle tokenizer = tokenizer_init("-");
    token_t *token = get_next_token(tokenizer);
    check_token(token, SUBTRACT_OPERATOR, "-");
}

TEST(SingleTokenTest, MultiplyOperatorToken) {
    tokenizer_handle tokenizer = tokenizer_init("*");
    token_t *token = get_next_token(tokenizer);
    check_token(token, MULTIPLY_OPERATOR, "*");
}

TEST(SingleTokenTest, DivideOperatorToken) {
    tokenizer_handle tokenizer = tokenizer_init("/");
    token_t *token = get_next_token(tokenizer);
    check_token(token, DIVIDE_OPERATOR, "/");
}


TEST(MultiTokenTest, AddIntegerToInteger) {
    tokenizer_handle tokenizer = tokenizer_init("123 + 456");
    token_t *token = get_next_token(tokenizer);
    check_token(token, INTEGER, "123");
    token = get_next_token(tokenizer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "456");
}

TEST(MultiTokenTest, SubtractIntegerFromFloat) {
    tokenizer_handle tokenizer = tokenizer_init("1.23 - 1");
    token_t *token = get_next_token(tokenizer);
    check_token(token, FLOAT, "1.23");
    token = get_next_token(tokenizer);
    check_token(token, SUBTRACT_OPERATOR, "-");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "1");
}

TEST(MultiTokenTest, ReturnNullAfterLastToken) {
    tokenizer_handle tokenizer = tokenizer_init("1");
    token_t *token = get_next_token(tokenizer);
    token = get_next_token(tokenizer);
    EXPECT_EQ(token->type, NULLTOKEN);
}

TEST(MultiTokenTest, MoreTokensToRead) {
    tokenizer_handle tokenizer = tokenizer_init("1.23 - 5 * 4.2");
    token_t *token = get_next_token(tokenizer);
    check_token(token, FLOAT, "1.23");
    token = get_next_token(tokenizer);
    token = get_next_token(tokenizer);
    token = get_next_token(tokenizer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(tokenizer);
    check_token(token, FLOAT, "4.2");
}

TEST(MultiTokenTest, NoSpaceBetweenTokens) {
    tokenizer_handle tokenizer = tokenizer_init("1.23-5*4.2");
    token_t *token = get_next_token(tokenizer);
    check_token(token, FLOAT, "1.23");
    token = get_next_token(tokenizer);
    token = get_next_token(tokenizer);
    token = get_next_token(tokenizer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(tokenizer);
    check_token(token, FLOAT, "4.2");
}

TEST(MultiTokenTest, PriorityWithParentheses) {
    tokenizer_handle tokenizer = tokenizer_init("(5 + 3) * 2");
    token_t *token = get_next_token(tokenizer);
    check_token(token, OPEN_PAREN, "(");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "5");
    token = get_next_token(tokenizer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "3");
    token = get_next_token(tokenizer);
    check_token(token, CLOSE_PAREN, ")");
    token = get_next_token(tokenizer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "2");
}

TEST(MultiTokenTest, MoreThanThreeOperators) {
    tokenizer_handle tokenizer = tokenizer_init("1 + 2 * 3 - 4 / 2");
    token_t *token = get_next_token(tokenizer);
    check_token(token, INTEGER, "1");
    token = get_next_token(tokenizer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "2");
    token = get_next_token(tokenizer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "3");
    token = get_next_token(tokenizer);
    check_token(token, SUBTRACT_OPERATOR, "-");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "4");
    token = get_next_token(tokenizer);
    check_token(token, DIVIDE_OPERATOR, "/");
    token = get_next_token(tokenizer);
    check_token(token, INTEGER, "2");
}