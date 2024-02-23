#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

token_t *generate_test_token(type_t type, const char *value) {
    token_t *new_token = (token_t *)malloc(sizeof(token_t));
    new_token->type = type;
    new_token->value = strdup(value);

    return new_token;
}

void check_op_node(node_t *op_node, type_t op) {
    EXPECT_EQ(op_node->type, BINARY_OP);
    EXPECT_EQ(op_node->subtype, op);
}

void check_constant_node(node_t *const_node, int value) {
    EXPECT_EQ(const_node->type, CONSTANT);
    EXPECT_EQ(const_node->subtype, INTEGER_CONST);
    EXPECT_EQ(const_node->value.int_value, value);
}

void check_constant_node(node_t *const_node, float value) {
    EXPECT_EQ(const_node->type, CONSTANT);
    EXPECT_EQ(const_node->subtype, FLOAT_CONST);
    EXPECT_FLOAT_EQ(const_node->value.float_value, value);    
}

TEST(ParserTest, ParseSimpleIntegerAddition) {
    // 2 + 3
    token_t *tokens[] = {
        generate_test_token(INTEGER_CONST, "2"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER_CONST, "3"),
    };

    node_t *root = parse(tokens, 3);

    check_op_node(root, ADD_OPERATOR);
    check_constant_node(root->left_child, 2);
    check_constant_node(root->right_child, 3);
}

TEST(ParserTest, ParseSimpleFloatAddition) {
    // 2.7 + 3.2
    token_t *tokens[] = {
        generate_test_token(FLOAT_CONST, "2.7"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(FLOAT_CONST, "3.2"),
    };

    node_t *root = parse(tokens, 3);

    check_op_node(root, ADD_OPERATOR);
    check_constant_node(root->left_child, 2.7f);
    check_constant_node(root->right_child, 3.2f);
}

TEST(ParserTest, ParseIntegerOpsWithPriority) {
    // 2 - 3 * 4
    token_t *tokens[] = {
        generate_test_token(INTEGER_CONST, "2"),
        generate_test_token(SUBTRACT_OPERATOR, "-"),
        generate_test_token(INTEGER_CONST, "3"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER_CONST, "4"),
    };

    node_t *root = parse(tokens, 5);

    check_op_node(root, SUBTRACT_OPERATOR);
    check_constant_node(root->left_child, 2);

    check_op_node(root->right_child, MULTIPLY_OPERATOR);
    check_constant_node(root->right_child->left_child, 3);
    check_constant_node(root->right_child->right_child, 4);
}

TEST(ParserTest, ParseMixedOpsWithPriority) {
    // 2 * 7.9 + 9
    token_t *tokens[] = {
        generate_test_token(INTEGER_CONST, "2"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(FLOAT_CONST, "7.9"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER_CONST, "9"),
    };

    node_t *root = parse(tokens, 5);

    check_op_node(root, ADD_OPERATOR);

    check_op_node(root->left_child, MULTIPLY_OPERATOR);
    check_constant_node(root->left_child->left_child, 2);
    check_constant_node(root->left_child->right_child, 7.9f);
    check_constant_node(root->right_child, 9);
}

TEST(ParserTest, ParseIntegerDivision) {
    // 8 / 2
    token_t *tokens[] = {
        generate_test_token(INTEGER_CONST, "8"),
        generate_test_token(DIVIDE_OPERATOR, "/"),
        generate_test_token(INTEGER_CONST, "2"),
    };

    node_t *root = parse(tokens, 3);

    check_op_node(root, DIVIDE_OPERATOR);
    check_constant_node(root->left_child, 8);
    check_constant_node(root->right_child, 2);
}

TEST(ParserTest, ParseMixedOpsWithParentheses) {
    // (2 + 3) * 4
    token_t *tokens[] = {
        generate_test_token(OPEN_PAREN, "("),
        generate_test_token(INTEGER_CONST, "2"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER_CONST, "3"),
        generate_test_token(CLOSE_PAREN, ")"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER_CONST, "4"),
    };

    node_t *root = parse(tokens, 7);

    check_op_node(root, MULTIPLY_OPERATOR);

    check_op_node(root->left_child, ADD_OPERATOR);
    check_constant_node(root->left_child->left_child, 2);
    check_constant_node(root->left_child->right_child, 3);
    check_constant_node(root->right_child, 4);
}

TEST(ParserTest, ParseMoreThanThreeOperators) {
    // 1 + 2 * 3 - 4 / 2
    token_t *tokens[] = {
        generate_test_token(INTEGER_CONST, "1"),
        generate_test_token(ADD_OPERATOR, "+"),
        generate_test_token(INTEGER_CONST, "2"),
        generate_test_token(MULTIPLY_OPERATOR, "*"),
        generate_test_token(INTEGER_CONST, "3"),
        generate_test_token(SUBTRACT_OPERATOR, "-"),
        generate_test_token(INTEGER_CONST, "4"),
        generate_test_token(DIVIDE_OPERATOR, "/"),
        generate_test_token(INTEGER_CONST, "2"),
    };

    node_t *root = parse(tokens, 9);

    // TODO: dependency to op prioritizing algorithm > better correctness check needed
    // for this case, ADD may come first
    check_op_node(root, SUBTRACT_OPERATOR);

    check_op_node(root->left_child, ADD_OPERATOR);
    check_constant_node(root->left_child->left_child, 1);

    check_op_node(root->left_child->right_child, MULTIPLY_OPERATOR);
    check_constant_node(root->left_child->right_child->left_child, 2);
    check_constant_node(root->left_child->right_child->right_child, 3);

    check_op_node(root->right_child, DIVIDE_OPERATOR);
    check_constant_node(root->right_child->left_child, 4);
    check_constant_node(root->right_child->right_child, 2);
}

TEST(ParserTest, ParseSimpleIntegerDeclaration) {
    // int x = 3;
    token_t *tokens[] = {
        generate_test_token(INTEGER_TYPE, "int"),
        generate_test_token(IDENTIFIER, "x"),
        generate_test_token(SEMICOLON, ";"),
    };

    node_t *root = parse(tokens, 3);

    EXPECT_EQ(root->type, DECL);
    EXPECT_EQ(root->subtype, DECLARATION);

    EXPECT_EQ(root->left_child->type, TYPE_SPECIFIER);
    EXPECT_EQ(root->left_child->subtype, INTEGER_TYPE);
    EXPECT_EQ(root->right_child->type, DECLARATOR);
    EXPECT_EQ(root->right_child->subtype, IDENTIFIER);
    EXPECT_STREQ(root->right_child->value.id_name, "x");
}

TEST(ParserTest, ParseSimpleIntegerAssignment) {
    // int x = 3;
    token_t *tokens[] = {
        generate_test_token(INTEGER_TYPE, "int"),
        generate_test_token(IDENTIFIER, "x"),
        generate_test_token(EQ_ASSIGN, "="),
        generate_test_token(INTEGER_CONST, "3"),
        generate_test_token(SEMICOLON, ";"),
    };

    node_t *root = parse(tokens, 5);

    EXPECT_EQ(root->type, DECL);
    EXPECT_EQ(root->subtype, DECLARATION);

    EXPECT_EQ(root->left_child->type, TYPE_SPECIFIER);
    EXPECT_EQ(root->left_child->subtype, INTEGER_TYPE);

    EXPECT_EQ(root->right_child->type, ASSIGN_OP);
    EXPECT_EQ(root->right_child->subtype, EQ_ASSIGN);

    EXPECT_EQ(root->right_child->left_child->type, DECLARATOR);
    EXPECT_EQ(root->right_child->left_child->subtype, IDENTIFIER);
    EXPECT_STREQ(root->right_child->left_child->value.id_name, "x");
    
    check_constant_node(root->right_child->right_child, 3);
}

TEST(ParserTest, ParseSimpleFloatAssignment) {
    // float hey = 12.76;
    token_t *tokens[] = {
        generate_test_token(FLOAT_TYPE, "float"),
        generate_test_token(IDENTIFIER, "hey"),
        generate_test_token(EQ_ASSIGN, "="),
        generate_test_token(FLOAT_CONST, "12.76"),
        generate_test_token(SEMICOLON, ";"),
    };

    node_t *root = parse(tokens, 5);

    EXPECT_EQ(root->type, DECL);
    EXPECT_EQ(root->subtype, DECLARATION);

    EXPECT_EQ(root->left_child->type, TYPE_SPECIFIER);
    EXPECT_EQ(root->left_child->subtype, FLOAT_TYPE);

    EXPECT_EQ(root->right_child->type, ASSIGN_OP);
    EXPECT_EQ(root->right_child->subtype, EQ_ASSIGN);

    EXPECT_EQ(root->right_child->left_child->type, DECLARATOR);
    EXPECT_EQ(root->right_child->left_child->subtype, IDENTIFIER);
    EXPECT_STREQ(root->right_child->left_child->value.id_name, "hey");
    
    check_constant_node(root->right_child->right_child, 12.76f);
}