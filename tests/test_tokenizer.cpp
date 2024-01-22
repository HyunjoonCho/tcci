#include <gtest/gtest.h>

extern "C" {
    #include "../src/tokenizer.h"
}

TEST(SingleTokenTest, IntegerToken) {
    tokenizer_handle tokenizer = tokenizer_init("123");
    int length = get_current_token_length(tokenizer);
    EXPECT_EQ(length, 3);
    token_t token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, INTEGER);
    EXPECT_STREQ(token.value, "123");
}

TEST(SingleTokenTest, FloatToken) {
    tokenizer_handle tokenizer = tokenizer_init("12.34");
    token_t token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, FLOAT);
    EXPECT_STREQ(token.value, "12.34");
}

TEST(SingleTokenTest, AddOperatorToken) {
    tokenizer_handle tokenizer = tokenizer_init("+");
    token_t token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, ADD_OPERATOR);
    EXPECT_STREQ(token.value, "+");
}

TEST(SingleTokenTest, SubtractOperatorToken) {
    tokenizer_handle tokenizer = tokenizer_init("-");
    token_t token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, SUBTRACT_OPERATOR);
    EXPECT_STREQ(token.value, "-");
}


TEST(SingleTokenTest, MultiplyOperatorToken) {
    tokenizer_handle tokenizer = tokenizer_init("*");
    token_t token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, MULTIPLY_OPERATOR);
    EXPECT_STREQ(token.value, "*");
}

TEST(MultiTokenTest, AddIntegerToInteger) {
    tokenizer_handle tokenizer = tokenizer_init("123 + 456");
    token_t token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, INTEGER);
    EXPECT_STREQ(token.value, "123");
    token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, ADD_OPERATOR);
    EXPECT_STREQ(token.value, "+");
    token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, INTEGER);
    EXPECT_STREQ(token.value, "456");
}

TEST(MultiTokenTest, SubtractIntegerFromFloat) {
    tokenizer_handle tokenizer = tokenizer_init("1.23 - 1");
    token_t token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, FLOAT);
    EXPECT_STREQ(token.value, "1.23");
    token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, SUBTRACT_OPERATOR);
    EXPECT_STREQ(token.value, "-");
    token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, INTEGER);
    EXPECT_STREQ(token.value, "1");
}


TEST(MultiTokenTest, ReturnNullAfterLastToken) {
    tokenizer_handle tokenizer = tokenizer_init("1");
    token_t token = get_next_token(tokenizer);
    token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, NULLTOKEN);
}

TEST(MultiTokenTest, MoreTokensToRead) {
    tokenizer_handle tokenizer = tokenizer_init("1.23 - 5 * 4.2");
    token_t token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, FLOAT);
    EXPECT_STREQ(token.value, "1.23");
    token = get_next_token(tokenizer);
    token = get_next_token(tokenizer);
    token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, MULTIPLY_OPERATOR);
    EXPECT_STREQ(token.value, "*");
    token = get_next_token(tokenizer);
    EXPECT_EQ(token.type, FLOAT);
    EXPECT_STREQ(token.value, "4.2");
}