#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

void check_token(token_t *token, type_t expected_type, const char *expected_value) {
    EXPECT_EQ(token->type, expected_type);
    EXPECT_STREQ(token->value, expected_value);
}

TEST(SingleTokenTest, IntegerToken) {
    lexer_handle lexer = lexer_init("123");
    token_t *token = get_next_token(lexer);
    check_token(token, INTEGER, "123");
}

TEST(SingleTokenTest, FloatToken) {
    lexer_handle lexer = lexer_init("12.34");
    token_t *token = get_next_token(lexer);
    check_token(token, FLOAT, "12.34");
}

TEST(SingleTokenTest, AddOperatorToken) {
    lexer_handle lexer = lexer_init("+");
    token_t *token = get_next_token(lexer);
    check_token(token, ADD_OPERATOR, "+");
}

TEST(SingleTokenTest, SubtractOperatorToken) {
    lexer_handle lexer = lexer_init("-");
    token_t *token = get_next_token(lexer);
    check_token(token, SUBTRACT_OPERATOR, "-");
}

TEST(SingleTokenTest, MultiplyOperatorToken) {
    lexer_handle lexer = lexer_init("*");
    token_t *token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
}

TEST(SingleTokenTest, DivideOperatorToken) {
    lexer_handle lexer = lexer_init("/");
    token_t *token = get_next_token(lexer);
    check_token(token, DIVIDE_OPERATOR, "/");
}


TEST(MultiTokenTest, AddIntegerToInteger) {
    lexer_handle lexer = lexer_init("123 + 456");
    token_t *token = get_next_token(lexer);
    check_token(token, INTEGER, "123");
    token = get_next_token(lexer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "456");
}

TEST(MultiTokenTest, SubtractIntegerFromFloat) {
    lexer_handle lexer = lexer_init("1.23 - 1");
    token_t *token = get_next_token(lexer);
    check_token(token, FLOAT, "1.23");
    token = get_next_token(lexer);
    check_token(token, SUBTRACT_OPERATOR, "-");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "1");
}

TEST(MultiTokenTest, ReturnNullAfterLastToken) {
    lexer_handle lexer = lexer_init("1");
    token_t *token = get_next_token(lexer);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, NULLTOKEN);
}

TEST(MultiTokenTest, MoreTokensToRead) {
    lexer_handle lexer = lexer_init("1.23 - 5 * 4.2");
    token_t *token = get_next_token(lexer);
    check_token(token, FLOAT, "1.23");
    token = get_next_token(lexer);
    token = get_next_token(lexer);
    token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(lexer);
    check_token(token, FLOAT, "4.2");
}

TEST(MultiTokenTest, NoSpaceBetweenTokens) {
    lexer_handle lexer = lexer_init("1.23-5*4.2");
    token_t *token = get_next_token(lexer);
    check_token(token, FLOAT, "1.23");
    token = get_next_token(lexer);
    token = get_next_token(lexer);
    token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(lexer);
    check_token(token, FLOAT, "4.2");
}

TEST(MultiTokenTest, PriorityWithParentheses) {
    lexer_handle lexer = lexer_init("(5 + 3) * 2");
    token_t *token = get_next_token(lexer);
    check_token(token, OPEN_PAREN, "(");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "5");
    token = get_next_token(lexer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "3");
    token = get_next_token(lexer);
    check_token(token, CLOSE_PAREN, ")");
    token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "2");
}

TEST(MultiTokenTest, MoreThanThreeOperators) {
    lexer_handle lexer = lexer_init("1 + 2 * 3 - 4 / 2");
    token_t *token = get_next_token(lexer);
    check_token(token, INTEGER, "1");
    token = get_next_token(lexer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "2");
    token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "3");
    token = get_next_token(lexer);
    check_token(token, SUBTRACT_OPERATOR, "-");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "4");
    token = get_next_token(lexer);
    check_token(token, DIVIDE_OPERATOR, "/");
    token = get_next_token(lexer);
    check_token(token, INTEGER, "2");
}

TEST(VariableTest, IntegerDeclaration) {
    lexer_handle lexer = lexer_init("int x;");
    token_t *token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}

TEST(VariableTest, FloatInitialization) {
    lexer_handle lexer = lexer_init("float y = 3.14;");
    token_t *token = get_next_token(lexer);
    EXPECT_EQ(token->type, FLOAT_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, FLOAT);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}

TEST(VariableTest, VariableDeclarationAndInitialization) {
    lexer_handle lexer = lexer_init("int a = 5;");
    token_t *token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}

TEST(VariableTest, MultipleDeclarations) {
    lexer_handle lexer = lexer_init("int x; float y = 3.14; int z = 42;");
    
    // Test for the first declaration
    token_t *token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);

    // Test for the second declaration
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, FLOAT_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, FLOAT);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);

    // Test for the third declaration
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}
