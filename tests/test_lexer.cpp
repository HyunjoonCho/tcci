#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

void check_token(token_t *token, type_t expected_type, const char *expected_value) {
    EXPECT_EQ(token->type, expected_type);
    EXPECT_STREQ(token->value, expected_value);
}

TEST(LexerArithmeticSingleToken, IntegerToken) {
    lexer_handle lexer = lexer_init("123");
    token_t *token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "123");
}

TEST(LexerArithmeticSingleToken, FloatToken) {
    lexer_handle lexer = lexer_init("12.34");
    token_t *token = get_next_token(lexer);
    check_token(token, FLOAT_CONST, "12.34");
}

TEST(LexerArithmeticSingleToken, AddOperatorToken) {
    lexer_handle lexer = lexer_init("+");
    token_t *token = get_next_token(lexer);
    check_token(token, ADD_OPERATOR, "+");
}

TEST(LexerArithmeticSingleToken, SubtractOperatorToken) {
    lexer_handle lexer = lexer_init("-");
    token_t *token = get_next_token(lexer);
    check_token(token, SUBTRACT_OPERATOR, "-");
}

TEST(LexerArithmeticSingleToken, MultiplyOperatorToken) {
    lexer_handle lexer = lexer_init("*");
    token_t *token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
}

TEST(LexerArithmeticSingleToken, DivideOperatorToken) {
    lexer_handle lexer = lexer_init("/");
    token_t *token = get_next_token(lexer);
    check_token(token, DIVIDE_OPERATOR, "/");
}

TEST(LexerArithmeticMultiTokens, AddIntegerToInteger) {
    lexer_handle lexer = lexer_init("123 + 456");
    token_t *token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "123");
    token = get_next_token(lexer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "456");
}

TEST(LexerArithmeticMultiTokens, SubtractIntegerFromFloat) {
    lexer_handle lexer = lexer_init("1.23 - 1");
    token_t *token = get_next_token(lexer);
    check_token(token, FLOAT_CONST, "1.23");
    token = get_next_token(lexer);
    check_token(token, SUBTRACT_OPERATOR, "-");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "1");
}

TEST(LexerArithmeticMultiTokens, ReturnNullAfterLastToken) {
    lexer_handle lexer = lexer_init("1");
    token_t *token = get_next_token(lexer);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, NULLTOKEN);
}

TEST(LexerArithmeticMultiTokens, MoreTokensToRead) {
    lexer_handle lexer = lexer_init("1.23 - 5 * 4.2");
    token_t *token = get_next_token(lexer);
    check_token(token, FLOAT_CONST, "1.23");
    token = get_next_token(lexer);
    token = get_next_token(lexer);
    token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(lexer);
    check_token(token, FLOAT_CONST, "4.2");
}

TEST(LexerArithmeticMultiTokens, NoSpaceBetweenTokens) {
    lexer_handle lexer = lexer_init("1.23-5*4.2");
    token_t *token = get_next_token(lexer);
    check_token(token, FLOAT_CONST, "1.23");
    token = get_next_token(lexer);
    token = get_next_token(lexer);
    token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(lexer);
    check_token(token, FLOAT_CONST, "4.2");
}

TEST(LexerArithmeticMultiTokens, PriorityWithParentheses) {
    lexer_handle lexer = lexer_init("(5 + 3) * 2");
    token_t *token = get_next_token(lexer);
    check_token(token, OPEN_PAREN, "(");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "5");
    token = get_next_token(lexer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "3");
    token = get_next_token(lexer);
    check_token(token, CLOSE_PAREN, ")");
    token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "2");
}

TEST(LexerArithmeticMultiTokens, MoreThanThreeOperators) {
    lexer_handle lexer = lexer_init("1 + 2 * 3 - 4 / 2");
    token_t *token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "1");
    token = get_next_token(lexer);
    check_token(token, ADD_OPERATOR, "+");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "2");
    token = get_next_token(lexer);
    check_token(token, MULTIPLY_OPERATOR, "*");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "3");
    token = get_next_token(lexer);
    check_token(token, SUBTRACT_OPERATOR, "-");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "4");
    token = get_next_token(lexer);
    check_token(token, DIVIDE_OPERATOR, "/");
    token = get_next_token(lexer);
    check_token(token, INTEGER_CONST, "2");
}

TEST(LexerDeclarations, IntegerDeclaration) {
    lexer_handle lexer = lexer_init("int x;");
    token_t *token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}

TEST(LexerDeclarations, FloatInitialization) {
    lexer_handle lexer = lexer_init("float y = 3.14;");
    token_t *token = get_next_token(lexer);
    EXPECT_EQ(token->type, FLOAT_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, EQ_ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, FLOAT_CONST);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}

TEST(LexerDeclarations, VariableDeclarationAndInitialization) {
    lexer_handle lexer = lexer_init("int a = 5;");
    token_t *token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, EQ_ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_CONST);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}

TEST(LexerCompoundStatements, MultipleDeclarations) {
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
    EXPECT_EQ(token->type, EQ_ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, FLOAT_CONST);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);

    // Test for the third declaration
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, EQ_ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_CONST);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}

TEST(LexerCompoundStatements, DeclareAndAdd) {
    lexer_handle lexer = lexer_init("int x; int y = 12; x = y + 16;");
    
    // Test for the first declaration
    token_t *token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);

    // Test for the second declaration
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_TYPE);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, EQ_ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_CONST);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);

    // Test for the third declaration
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, EQ_ASSIGN);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, IDENTIFIER);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, ADD_OPERATOR);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, INTEGER_CONST);
    token = get_next_token(lexer);
    EXPECT_EQ(token->type, SEMICOLON);
}