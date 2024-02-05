#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

token_t *generate_test_token(token_type type, const char *value) {
    token_t *new_token = (token_t *)malloc(sizeof(token_t));
    new_token->type = type;
    new_token->value = strdup(value);

    return new_token;
}


void check_op_node(node_t *op_node, op_type op) {
    EXPECT_EQ(op_node->type, BINARY_OP);
    EXPECT_EQ(op_node->subtype.op_t, op);
}

void check_constant_node(node_t *const_node, int value) {
    EXPECT_EQ(const_node->type, CONSTANT);
    EXPECT_EQ(const_node->subtype.const_t, INTEGER_CONST);
    EXPECT_EQ(const_node->value.int_value, value);
}

void check_constant_node(node_t *const_node, float value) {
    EXPECT_EQ(const_node->type, CONSTANT);
    EXPECT_EQ(const_node->subtype.const_t, FLOAT_CONST);
    EXPECT_FLOAT_EQ(const_node->value.float_value, value);    
}

TEST(ParserTest, ParseSimpleIntegerAddition) {
    // 2 + 3
    token_t *tokens[] = {
        generate_test_token(INTEGER, "2"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "3"),
    };

    node_t *root = parse(tokens, 3);

    check_op_node(root, ADD);
    check_constant_node(root->left_operand, 2);
    check_constant_node(root->right_operand, 3);
}

TEST(ParserTest, ParseSimpleFloatAddition) {
    // 2.7 + 3.2
    token_t *tokens[] = {
        generate_test_token(FLOAT, "2.7"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(FLOAT, "3.2"),
    };

    node_t *root = parse(tokens, 3);

    check_op_node(root, ADD);
    check_constant_node(root->left_operand, 2.7f);
    check_constant_node(root->right_operand, 3.2f);
}

TEST(ParserTest, ParseIntegerOpsWithPriority) {
    // 2 - 3 * 4
    token_t *tokens[] = {
        generate_test_token(INTEGER, "2"),
        generate_test_token(SUBTRACT_OPERATOR, "-"),
        generate_test_token(INTEGER, "3"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER, "4"),
    };

    node_t *root = parse(tokens, 5);

    check_op_node(root, SUBTRACT);
    check_constant_node(root->left_operand, 2);

    check_op_node(root->right_operand, MULTIPLY);
    check_constant_node(root->right_operand->left_operand, 3);
    check_constant_node(root->right_operand->right_operand, 4);
}

TEST(ParserTest, ParseMixedOpsWithPriority) {
    // 2 * 7.9 + 9
    token_t *tokens[] = {
        generate_test_token(INTEGER, "2"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(FLOAT, "7.9"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "9"),
    };

    node_t *root = parse(tokens, 5);

    check_op_node(root, ADD);

    check_op_node(root->left_operand, MULTIPLY);
    check_constant_node(root->left_operand->left_operand, 2);
    check_constant_node(root->left_operand->right_operand, 7.9f);
    check_constant_node(root->right_operand, 9);
}

TEST(ParserTest, ParseIntegerDivision) {
    // 8 / 2
    token_t *tokens[] = {
        generate_test_token(INTEGER, "8"),
        generate_test_token(DIVIDE_OPERATOR, "/"),
        generate_test_token(INTEGER, "2"),
    };

    node_t *root = parse(tokens, 3);

    check_op_node(root, DIVIDE);
    check_constant_node(root->left_operand, 8);
    check_constant_node(root->right_operand, 2);
}

TEST(ParserTest, ParseMixedOpsWithParentheses) {
    // (2 + 3) * 4
    token_t *tokens[] = {
        generate_test_token(OPEN_PAREN, "("),
        generate_test_token(INTEGER, "2"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "3"),
        generate_test_token(CLOSE_PAREN, ")"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER, "4"),
    };

    node_t *root = parse(tokens, 7);

    check_op_node(root, MULTIPLY);

    check_op_node(root->left_operand, ADD);
    check_constant_node(root->left_operand->left_operand, 2);
    check_constant_node(root->left_operand->right_operand, 3);
    check_constant_node(root->right_operand, 4);
}

TEST(ParserTest, ParseMoreThanThreeOperators) {
    // 1 + 2 * 3 - 4 / 2
    token_t *tokens[] = {
        generate_test_token(INTEGER, "1"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER, "2"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER, "3"),
        generate_test_token(SUBTRACT_OPERATOR, "-"),
        generate_test_token(INTEGER, "4"),
        generate_test_token(DIVIDE_OPERATOR, "/"),
        generate_test_token(INTEGER, "2"),
    };

    node_t *root = parse(tokens, 9);

    // TODO: dependency to op prioritizing algorithm > better correctness check needed
    // for this case, ADD may come first
    check_op_node(root, SUBTRACT);

    check_op_node(root->left_operand, ADD);
    check_constant_node(root->left_operand->left_operand, 1);

    check_op_node(root->left_operand->right_operand, MULTIPLY);
    check_constant_node(root->left_operand->right_operand->left_operand, 2);
    check_constant_node(root->left_operand->right_operand->right_operand, 3);

    check_op_node(root->right_operand, DIVIDE);
    check_constant_node(root->right_operand->left_operand, 4);
    check_constant_node(root->right_operand->right_operand, 2);
}