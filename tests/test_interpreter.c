#include <string.h>

#include "../src/interpreter.h"
#include "test_assert.h"
#include "test_utils.h"

static int run_program(const char *input,
                       interpreter_handle *out_interpreter,
                       node_t **out_root,
                       literal_t **out_result,
                       char *error,
                       size_t error_size) {
    interpreter_handle interpreter;
    node_t *root = NULL;
    literal_t *result;

    if (parse_input(input, &root, error, error_size) != 0) {
        return 1;
    }

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

    *out_interpreter = interpreter;
    *out_root = root;
    *out_result = result;
    return 0;
}

static int test_integer_arithmetic(void) {
    interpreter_handle interpreter = NULL;
    node_t *root = NULL;
    literal_t *result = NULL;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(run_program("2 + 3 * 4;", &interpreter, &root, &result, error, sizeof(error)) == 0);
    TEST_ASSERT_INT_EQ(INTEGER_CONST, result->type);
    TEST_ASSERT_INT_EQ(14, result->value.int_value);

    free(result);
    interpreter_free(interpreter);
    free_node(root);
    return 0;
}

static int test_mixed_arithmetic(void) {
    interpreter_handle interpreter = NULL;
    node_t *root = NULL;
    literal_t *result = NULL;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(run_program("4 * 2.5;", &interpreter, &root, &result, error, sizeof(error)) == 0);
    TEST_ASSERT_INT_EQ(FLOAT_CONST, result->type);
    TEST_ASSERT_FLOAT_EQ(10.0f, result->value.float_value, 0.0001f);

    free(result);
    interpreter_free(interpreter);
    free_node(root);
    return 0;
}

static int test_declaration_default_value(void) {
    interpreter_handle interpreter = NULL;
    node_t *root = NULL;
    literal_t *result = NULL;
    literal_t *x_value;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(run_program("int x;", &interpreter, &root, &result, error, sizeof(error)) == 0);

    x_value = get_value_of(interpreter, "x");
    TEST_ASSERT(x_value != NULL);
    TEST_ASSERT_INT_EQ(INTEGER_CONST, x_value->type);
    TEST_ASSERT_INT_EQ(0, x_value->value.int_value);

    free(result);
    interpreter_free(interpreter);
    free_node(root);
    return 0;
}

static int test_assignment_with_identifier_usage(void) {
    interpreter_handle interpreter = NULL;
    node_t *root = NULL;
    literal_t *result = NULL;
    literal_t *x_value;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(run_program("int x = 12; x = x + 3;", &interpreter, &root, &result, error, sizeof(error)) == 0);
    TEST_ASSERT_INT_EQ(INTEGER_CONST, result->type);
    TEST_ASSERT_INT_EQ(15, result->value.int_value);

    x_value = get_value_of(interpreter, "x");
    TEST_ASSERT(x_value != NULL);
    TEST_ASSERT_INT_EQ(15, x_value->value.int_value);

    free(result);
    interpreter_free(interpreter);
    free_node(root);
    return 0;
}

static int test_compound_execution_returns_last_result(void) {
    interpreter_handle interpreter = NULL;
    node_t *root = NULL;
    literal_t *result = NULL;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(run_program("int x = 1; int y = 2; x = x + y; x + 10;",
                            &interpreter, &root, &result, error, sizeof(error)) == 0);

    TEST_ASSERT_INT_EQ(INTEGER_CONST, result->type);
    TEST_ASSERT_INT_EQ(13, result->value.int_value);

    free(result);
    interpreter_free(interpreter);
    free_node(root);
    return 0;
}

static int test_undeclared_identifier_error(void) {
    node_t *root = NULL;
    interpreter_handle interpreter;
    literal_t *result;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(parse_input("x = 1;", &root, error, sizeof(error)) == 0);

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
        {"interpreter_integer_arithmetic", test_integer_arithmetic},
        {"interpreter_mixed_arithmetic", test_mixed_arithmetic},
        {"interpreter_declaration_default_value", test_declaration_default_value},
        {"interpreter_assignment_with_identifier_usage", test_assignment_with_identifier_usage},
        {"interpreter_compound_execution_returns_last_result", test_compound_execution_returns_last_result},
        {"interpreter_undeclared_identifier_error", test_undeclared_identifier_error},
    };

    return run_test_cases(cases, sizeof(cases) / sizeof(cases[0]));
}
