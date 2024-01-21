#include <gtest/gtest.h>

extern "C" {
    #include "../src/tokenizer.h"
}

TEST(SingleTokenTest, IntegerToken) {
    tokenizer_handle tokenizer = tokenizer_init("123");
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
