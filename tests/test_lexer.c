#include <string.h>

#include "test_assert.h"
#include "test_utils.h"

static int test_trailing_spaces(void) {
    token_buffer_t tokens;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(tokenize_input("123   ", &tokens, error, sizeof(error)) == 0);
    TEST_ASSERT_INT_EQ(1, tokens.count);
    TEST_ASSERT_INT_EQ(INTEGER_CONST, tokens.tokens[0]->type);
    TEST_ASSERT_STR_EQ("123", tokens.tokens[0]->value);

    token_buffer_free(&tokens);
    return 0;
}

static int test_tabs_and_newlines(void) {
    token_buffer_t tokens;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(tokenize_input("int\tx\n=\t42;", &tokens, error, sizeof(error)) == 0);
    TEST_ASSERT_INT_EQ(5, tokens.count);
    TEST_ASSERT_INT_EQ(INTEGER_TYPE, tokens.tokens[0]->type);
    TEST_ASSERT_INT_EQ(IDENTIFIER, tokens.tokens[1]->type);
    TEST_ASSERT_STR_EQ("x", tokens.tokens[1]->value);
    TEST_ASSERT_INT_EQ(EQ_ASSIGN, tokens.tokens[2]->type);
    TEST_ASSERT_INT_EQ(INTEGER_CONST, tokens.tokens[3]->type);
    TEST_ASSERT_STR_EQ("42", tokens.tokens[3]->value);
    TEST_ASSERT_INT_EQ(SEMICOLON, tokens.tokens[4]->type);

    token_buffer_free(&tokens);
    return 0;
}

static int test_invalid_character_error(void) {
    token_buffer_t tokens;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(tokenize_input("1 @ 2", &tokens, error, sizeof(error)) != 0);
    TEST_ASSERT(strstr(error, "Unexpected character") != NULL);
    return 0;
}

static int test_identifier_patterns(void) {
    token_buffer_t tokens;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(tokenize_input("x1 = _x + 1;", &tokens, error, sizeof(error)) == 0);
    TEST_ASSERT_INT_EQ(6, tokens.count);
    TEST_ASSERT_INT_EQ(IDENTIFIER, tokens.tokens[0]->type);
    TEST_ASSERT_STR_EQ("x1", tokens.tokens[0]->value);
    TEST_ASSERT_INT_EQ(EQ_ASSIGN, tokens.tokens[1]->type);
    TEST_ASSERT_INT_EQ(IDENTIFIER, tokens.tokens[2]->type);
    TEST_ASSERT_STR_EQ("_x", tokens.tokens[2]->value);
    TEST_ASSERT_INT_EQ(ADD_OPERATOR, tokens.tokens[3]->type);
    TEST_ASSERT_INT_EQ(INTEGER_CONST, tokens.tokens[4]->type);
    TEST_ASSERT_STR_EQ("1", tokens.tokens[4]->value);
    TEST_ASSERT_INT_EQ(SEMICOLON, tokens.tokens[5]->type);

    token_buffer_free(&tokens);
    return 0;
}

static int test_eof_behavior(void) {
    lexer_handle lexer = lexer_init("1");
    token_t *first;
    token_t *eof1;
    token_t *eof2;

    TEST_ASSERT(lexer != NULL);

    first = get_next_token(lexer);
    TEST_ASSERT(first != NULL);
    TEST_ASSERT_INT_EQ(INTEGER_CONST, first->type);

    eof1 = get_next_token(lexer);
    TEST_ASSERT(eof1 != NULL);
    TEST_ASSERT_INT_EQ(NULLTOKEN, eof1->type);

    eof2 = get_next_token(lexer);
    TEST_ASSERT(eof2 != NULL);
    TEST_ASSERT_INT_EQ(NULLTOKEN, eof2->type);

    TEST_ASSERT_INT_EQ(TCCI_STATUS_OK, lexer_status(lexer));

    free(first->value);
    free(first);
    free(eof1->value);
    free(eof1);
    free(eof2->value);
    free(eof2);
    lexer_free(lexer);
    return 0;
}

int main(void) {
    const test_case_t cases[] = {
        {"lexer_trailing_spaces", test_trailing_spaces},
        {"lexer_tabs_and_newlines", test_tabs_and_newlines},
        {"lexer_invalid_character_error", test_invalid_character_error},
        {"lexer_identifier_patterns", test_identifier_patterns},
        {"lexer_eof_behavior", test_eof_behavior},
    };

    return run_test_cases(cases, sizeof(cases) / sizeof(cases[0]));
}
