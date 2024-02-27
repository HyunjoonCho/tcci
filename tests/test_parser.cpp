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

void teardown(token_t **tokens, int token_count, node_t *root) {
    free_tokens(tokens, token_count); 
    free_node(root);
}

TEST(ParserArithmetic, SimpleIntegerAddition) {
    // 2 + 3
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 3);
    tokens[0] = generate_test_token(INTEGER_CONST, "2");
    tokens[1] = generate_test_token(ADD_OPERATOR, "+");
    tokens[2] = generate_test_token(INTEGER_CONST, "3");

    parser_handle parser = parser_init(tokens, 3);
    node_t *root = parse(parser);

    check_op_node(root, ADD_OPERATOR);
    check_constant_node(root->left_child, 2);
    check_constant_node(root->right_child, 3);

    free_parser(parser);
}

TEST(ParserArithmetic, SimpleFloatAddition) {
    // 2.7 + 3.2
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 3);
    tokens[0] = generate_test_token(FLOAT_CONST, "2.7");
    tokens[1] = generate_test_token(ADD_OPERATOR, "+");
    tokens[2] = generate_test_token(FLOAT_CONST, "3.2");


    parser_handle parser = parser_init(tokens, 3);
    node_t *root = parse(parser);

    check_op_node(root, ADD_OPERATOR);
    check_constant_node(root->left_child, 2.7f);
    check_constant_node(root->right_child, 3.2f);

    free_parser(parser);
}

TEST(ParserArithmetic, IntegerOpsWithPriority) {
    // 2 - 3 * 4
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 5);
    tokens[0] = generate_test_token(INTEGER_CONST, "2");
    tokens[1] = generate_test_token(SUBTRACT_OPERATOR, "-");
    tokens[2] = generate_test_token(INTEGER_CONST, "3");
    tokens[3] = generate_test_token(MULTIPLY_OPERATOR, "*");
    tokens[4] = generate_test_token(INTEGER_CONST, "4");

    parser_handle parser = parser_init(tokens, 5);
    node_t *root = parse(parser);

    check_op_node(root, SUBTRACT_OPERATOR);
    check_constant_node(root->left_child, 2);

    check_op_node(root->right_child, MULTIPLY_OPERATOR);
    check_constant_node(root->right_child->left_child, 3);
    check_constant_node(root->right_child->right_child, 4);

    free_parser(parser);
}

TEST(ParserArithmetic, MixedOpsWithPriority) {
    // 2 * 7.9 + 9
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 5);
    tokens[0] = generate_test_token(INTEGER_CONST, "2");
    tokens[1] = generate_test_token(MULTIPLY_OPERATOR, "*");
    tokens[2] = generate_test_token(FLOAT_CONST, "7.9");
    tokens[3] = generate_test_token(ADD_OPERATOR, "+");
    tokens[4] = generate_test_token(INTEGER_CONST, "9");

    parser_handle parser = parser_init(tokens, 5);
    node_t *root = parse(parser);

    check_op_node(root, ADD_OPERATOR);

    check_op_node(root->left_child, MULTIPLY_OPERATOR);
    check_constant_node(root->left_child->left_child, 2);
    check_constant_node(root->left_child->right_child, 7.9f);
    check_constant_node(root->right_child, 9);

    free_parser(parser);
}

TEST(ParserArithmetic, IntegerDivision) {
    // 8 / 2
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 3);
    tokens[0] = generate_test_token(INTEGER_CONST, "8");
    tokens[1] = generate_test_token(DIVIDE_OPERATOR, "/");
    tokens[2] = generate_test_token(INTEGER_CONST, "2");

    parser_handle parser = parser_init(tokens, 3);
    node_t *root = parse(parser);

    check_op_node(root, DIVIDE_OPERATOR);
    check_constant_node(root->left_child, 8);
    check_constant_node(root->right_child, 2);

    free_parser(parser);
}

TEST(ParserArithmetic, MixedOpsWithParentheses) {
    // (2 + 3) * 4
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 7);
    tokens[0] = generate_test_token(OPEN_PAREN, "(");
    tokens[1] = generate_test_token(INTEGER_CONST, "2");
    tokens[2] = generate_test_token(ADD_OPERATOR, "+");
    tokens[3] = generate_test_token(INTEGER_CONST, "3");
    tokens[4] = generate_test_token(CLOSE_PAREN, ")");
    tokens[5] = generate_test_token(MULTIPLY_OPERATOR, "*");
    tokens[6] = generate_test_token(INTEGER_CONST, "4");

    parser_handle parser = parser_init(tokens, 7);
    node_t *root = parse(parser);

    check_op_node(root, MULTIPLY_OPERATOR);

    check_op_node(root->left_child, ADD_OPERATOR);
    check_constant_node(root->left_child->left_child, 2);
    check_constant_node(root->left_child->right_child, 3);
    check_constant_node(root->right_child, 4);

    free_parser(parser);
}

TEST(ParserArithmetic, MoreThanThreeOperators) {
    // 1 + 2 * 3 - 4 / 2
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 9);
    tokens[0] = generate_test_token(INTEGER_CONST, "1");
    tokens[1] = generate_test_token(ADD_OPERATOR, "+");
    tokens[2] = generate_test_token(INTEGER_CONST, "2");
    tokens[3] = generate_test_token(MULTIPLY_OPERATOR, "*");
    tokens[4] = generate_test_token(INTEGER_CONST, "3");
    tokens[5] = generate_test_token(SUBTRACT_OPERATOR, "-");
    tokens[6] = generate_test_token(INTEGER_CONST, "4");
    tokens[7] = generate_test_token(DIVIDE_OPERATOR, "/");
    tokens[8] = generate_test_token(INTEGER_CONST, "2");


    parser_handle parser = parser_init(tokens, 9);
    node_t *root = parse(parser);

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

    free_parser(parser);
}

TEST(ParserDeclarations, SimpleIntegerDeclaration) {
    // int x = 3;
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 5);
    tokens[0] = generate_test_token(INTEGER_TYPE, "int");
    tokens[1] = generate_test_token(IDENTIFIER, "x");
    tokens[2] = generate_test_token(SEMICOLON, ";");

    parser_handle parser = parser_init(tokens, 3);
    node_t *root = parse(parser);

    EXPECT_EQ(root->type, DECL);
    EXPECT_EQ(root->subtype, DECLARATION);

    EXPECT_EQ(root->left_child->type, TYPE_SPECIFIER);
    EXPECT_EQ(root->left_child->subtype, INTEGER_TYPE);
    EXPECT_EQ(root->right_child->type, DECLARATOR);
    EXPECT_EQ(root->right_child->subtype, IDENTIFIER);
    EXPECT_STREQ(root->right_child->value.id_name, "x");

    free_parser(parser);
}

TEST(ParserDeclarations, SimpleIntegerAssignment) {
    // int x = 3;
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 5);
    tokens[0] = generate_test_token(INTEGER_TYPE, "int");
    tokens[1] = generate_test_token(IDENTIFIER, "x");
    tokens[2] = generate_test_token(EQ_ASSIGN, "=");
    tokens[3] = generate_test_token(INTEGER_CONST, "3");
    tokens[4] = generate_test_token(SEMICOLON, ";");


    parser_handle parser = parser_init(tokens, 5);
    node_t *root = parse(parser);

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

    free_parser(parser);
}

TEST(ParserDeclarations, SimpleFloatAssignment) {
    // float hey = 12.76;
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 5);
    tokens[0] = generate_test_token(FLOAT_TYPE, "float");
    tokens[1] = generate_test_token(IDENTIFIER, "hey");
    tokens[2] = generate_test_token(EQ_ASSIGN, "=");
    tokens[3] = generate_test_token(FLOAT_CONST, "12.76");
    tokens[4] = generate_test_token(SEMICOLON, ";");


    parser_handle parser = parser_init(tokens, 5);
    node_t *root = parse(parser);

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

    free_parser(parser);
}