#include <string.h>

#include "../src/interpreter.h"
#include "test_assert.h"
#include "test_utils.h"

static int run_pipeline(const char *input,
                        literal_t **out_result,
                        interpreter_handle *out_interpreter,
                        node_t **out_root,
                        char *error,
                        size_t error_size) {
    token_buffer_t tokens;
    parser_handle parser;
    node_t *root;
    interpreter_handle interpreter;
    literal_t *result;

    if (tokenize_input(input, &tokens, error, error_size) != 0) {
        return 1;
    }

    parser = parser_init(tokens.tokens, tokens.count);
    if (parser == NULL) {
        (void)snprintf(error, error_size, "failed to initialize parser");
        token_buffer_free(&tokens);
        return 1;
    }

    root = parse(parser);
    if (root == NULL || parser_status(parser) != TCCI_STATUS_OK) {
        (void)snprintf(error, error_size, "%s", parser_error_message(parser));
        free_parser(parser);
        token_buffer_free(&tokens);
        return 1;
    }

    free_parser(parser);
    token_buffer_free(&tokens);

    interpreter = interpreter_init(root);
    if (interpreter == NULL) {
        (void)snprintf(error, error_size, "failed to initialize interpreter");
        free_node(root);
        return 1;
    }

    result = interpret(interpreter);
    if (result == NULL || interpreter_status(interpreter) != TCCI_STATUS_OK) {
        (void)snprintf(error, error_size, "%s", interpreter_error_message(interpreter));
        interpreter_free(interpreter);
        free_node(root);
        return 1;
    }

    *out_result = result;
    *out_interpreter = interpreter;
    *out_root = root;
    return 0;
}

static int test_pipeline_arithmetic_program(void) {
    literal_t *result = NULL;
    interpreter_handle interpreter = NULL;
    node_t *root = NULL;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(run_pipeline("1 + 2 * 3;", &result, &interpreter, &root, error, sizeof(error)) == 0);
    TEST_ASSERT_INT_EQ(INTEGER_CONST, result->type);
    TEST_ASSERT_INT_EQ(7, result->value.int_value);

    free(result);
    interpreter_free(interpreter);
    free_node(root);
    return 0;
}

static int test_pipeline_declaration_and_assignment_program(void) {
    literal_t *result = NULL;
    interpreter_handle interpreter = NULL;
    node_t *root = NULL;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(run_pipeline("float y = 3.5; int x = 2; y = y + x; y;",
                             &result, &interpreter, &root, error, sizeof(error)) == 0);
    TEST_ASSERT_INT_EQ(FLOAT_CONST, result->type);
    TEST_ASSERT_FLOAT_EQ(5.5f, result->value.float_value, 0.0001f);

    free(result);
    interpreter_free(interpreter);
    free_node(root);
    return 0;
}

static int test_pipeline_parser_error(void) {
    token_buffer_t tokens;
    parser_handle parser;
    node_t *root;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(tokenize_input("int x = 1", &tokens, error, sizeof(error)) == 0);

    parser = parser_init(tokens.tokens, tokens.count);
    TEST_ASSERT(parser != NULL);

    root = parse(parser);
    TEST_ASSERT(root == NULL);
    TEST_ASSERT_INT_EQ(TCCI_STATUS_PARSER_ERROR, parser_status(parser));
    TEST_ASSERT(strstr(parser_error_message(parser), "Missing semicolon") != NULL);

    free_parser(parser);
    token_buffer_free(&tokens);
    return 0;
}

static int test_pipeline_runtime_error(void) {
    node_t *root = NULL;
    interpreter_handle interpreter;
    literal_t *result;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(parse_input("int x = 1; y = x + 1;", &root, error, sizeof(error)) == 0);

    interpreter = interpreter_init(root);
    TEST_ASSERT(interpreter != NULL);

    result = interpret(interpreter);
    TEST_ASSERT(result == NULL);
    TEST_ASSERT_INT_EQ(TCCI_STATUS_RUNTIME_ERROR, interpreter_status(interpreter));
    TEST_ASSERT(strstr(interpreter_error_message(interpreter), "undeclared") != NULL);

    interpreter_free(interpreter);
    free_node(root);
    return 0;
}

int main(void) {
    const test_case_t cases[] = {
        {"integration_pipeline_arithmetic_program", test_pipeline_arithmetic_program},
        {"integration_pipeline_declaration_and_assignment_program", test_pipeline_declaration_and_assignment_program},
        {"integration_pipeline_parser_error", test_pipeline_parser_error},
        {"integration_pipeline_runtime_error", test_pipeline_runtime_error},
    };

    return run_test_cases(cases, sizeof(cases) / sizeof(cases[0]));
}
