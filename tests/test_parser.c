#include <string.h>

#include "test_assert.h"
#include "test_utils.h"

static int test_precedence_with_parentheses(void) {
    node_t *root = NULL;
    compound_statement_node *compound;
    node_t *statement;
    binary_op_node *multiply;
    binary_op_node *add;

    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(parse_input("(2 + 3) * 4;", &root, error, sizeof(error)) == 0);

    TEST_ASSERT_INT_EQ(COMPOUND_STATEMENT_NODE, root->type);
    compound = (compound_statement_node *)root->actual_node;
    TEST_ASSERT_INT_EQ(1, compound->children_count);

    statement = compound->children[0];
    TEST_ASSERT_INT_EQ(BINARY_OP_NODE, statement->type);
    multiply = (binary_op_node *)statement->actual_node;
    TEST_ASSERT_INT_EQ(MULTIPLY_OPERATOR, multiply->subtype);

    TEST_ASSERT_INT_EQ(BINARY_OP_NODE, multiply->left_child->type);
    add = (binary_op_node *)multiply->left_child->actual_node;
    TEST_ASSERT_INT_EQ(ADD_OPERATOR, add->subtype);

    TEST_ASSERT_INT_EQ(CONSTANT_NODE, add->left_child->type);
    TEST_ASSERT_INT_EQ(CONSTANT_NODE, add->right_child->type);
    TEST_ASSERT_INT_EQ(CONSTANT_NODE, multiply->right_child->type);

    free_node(root);
    return 0;
}

static int test_declaration_with_assignment_shape(void) {
    node_t *root = NULL;
    compound_statement_node *compound;
    declaration_node *declaration;
    assign_op_node *assign;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(parse_input("int x = 3;", &root, error, sizeof(error)) == 0);

    compound = (compound_statement_node *)root->actual_node;
    TEST_ASSERT_INT_EQ(1, compound->children_count);
    TEST_ASSERT_INT_EQ(DECLARATION_NODE, compound->children[0]->type);

    declaration = (declaration_node *)compound->children[0]->actual_node;
    TEST_ASSERT_INT_EQ(TYPE_SPECIFIER_NODE, declaration->left_child->type);
    TEST_ASSERT_INT_EQ(ASSIGN_OP_NODE, declaration->right_child->type);

    assign = (assign_op_node *)declaration->right_child->actual_node;
    TEST_ASSERT_INT_EQ(DECLARATOR_NODE, assign->left_child->type);
    TEST_ASSERT_INT_EQ(CONSTANT_NODE, assign->right_child->type);

    free_node(root);
    return 0;
}

static int test_multi_statement_program(void) {
    node_t *root = NULL;
    compound_statement_node *compound;
    assign_op_node *assignment;
    binary_op_node *rhs;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(parse_input("int x = 1; x = x + 2;", &root, error, sizeof(error)) == 0);

    compound = (compound_statement_node *)root->actual_node;
    TEST_ASSERT_INT_EQ(2, compound->children_count);
    TEST_ASSERT_INT_EQ(DECLARATION_NODE, compound->children[0]->type);
    TEST_ASSERT_INT_EQ(ASSIGN_OP_NODE, compound->children[1]->type);

    assignment = (assign_op_node *)compound->children[1]->actual_node;
    TEST_ASSERT_INT_EQ(IDENTIFIER_NODE, assignment->left_child->type);
    TEST_ASSERT_INT_EQ(BINARY_OP_NODE, assignment->right_child->type);

    rhs = (binary_op_node *)assignment->right_child->actual_node;
    TEST_ASSERT_INT_EQ(ADD_OPERATOR, rhs->subtype);

    free_node(root);
    return 0;
}

static int parse_should_fail(const char *input, const char *expected_error_fragment) {
    token_buffer_t tokens;
    parser_handle parser;
    node_t *root;
    char error[TEST_ERROR_SIZE];

    TEST_ASSERT(tokenize_input(input, &tokens, error, sizeof(error)) == 0);

    parser = parser_init(tokens.tokens, tokens.count);
    TEST_ASSERT(parser != NULL);

    root = parse(parser);
    TEST_ASSERT(root == NULL);
    TEST_ASSERT_INT_EQ(TCCI_STATUS_PARSER_ERROR, parser_status(parser));
    TEST_ASSERT(strstr(parser_error_message(parser), expected_error_fragment) != NULL);

    free_parser(parser);
    token_buffer_free(&tokens);
    return 0;
}

static int test_missing_semicolon_error(void) {
    return parse_should_fail("int x = 1", "Missing semicolon");
}

static int test_mismatched_parentheses_error(void) {
    return parse_should_fail("(1 + 2;", "Mismatched parentheses");
}

static int test_empty_statement_error(void) {
    return parse_should_fail(";", "Empty statement");
}

static int test_operator_missing_operand_error(void) {
    return parse_should_fail("1 + (2 * );", "operator missing operand");
}

int main(void) {
    const test_case_t cases[] = {
        {"parser_precedence_with_parentheses", test_precedence_with_parentheses},
        {"parser_declaration_with_assignment_shape", test_declaration_with_assignment_shape},
        {"parser_multi_statement_program", test_multi_statement_program},
        {"parser_missing_semicolon_error", test_missing_semicolon_error},
        {"parser_mismatched_parentheses_error", test_mismatched_parentheses_error},
        {"parser_empty_statement_error", test_empty_statement_error},
        {"parser_operator_missing_operand_error", test_operator_missing_operand_error},
    };

    return run_test_cases(cases, sizeof(cases) / sizeof(cases[0]));
}
