#include <gtest/gtest.h>

extern "C" {
    #include "../src/commons.h"
}

#include "helper.hpp"

token_t *generate_test_token(type_t type, const char *value) {
    token_t *new_token = (token_t *)malloc(sizeof(token_t));
    new_token->type = type;
    new_token->value = strdup(value);

    return new_token;
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

    node_t *expected = create_binary_op_node(ADD_OPERATOR,
                                             create_literal_node(2),
                                             create_literal_node(3));
    node_equals(root, expected);

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

    node_t *expected = create_binary_op_node(ADD_OPERATOR,
                                             create_literal_node(2.7f),
                                             create_literal_node(3.2f));
    node_equals(root, expected);

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

    node_t *expected = create_binary_op_node(SUBTRACT_OPERATOR,
                                             create_literal_node(2),
                                             create_binary_op_node(MULTIPLY_OPERATOR,
                                                                   create_literal_node(3),
                                                                   create_literal_node(4)));
    node_equals(root, expected);

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

    node_t *expected = create_binary_op_node(ADD_OPERATOR,
                                             create_binary_op_node(MULTIPLY_OPERATOR,
                                                                   create_literal_node(2),
                                                                   create_literal_node(7.9f)),
                                             create_literal_node(9));
    node_equals(root, expected);

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

    node_t *expected = create_binary_op_node(DIVIDE_OPERATOR,
                                             create_literal_node(8),
                                             create_literal_node(2));
    node_equals(root, expected);

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

    node_t *expected = create_binary_op_node(MULTIPLY_OPERATOR,
                                             create_binary_op_node(ADD_OPERATOR,
                                                                   create_literal_node(2),
                                                                   create_literal_node(3)),
                                             create_literal_node(4));
    node_equals(root, expected);

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

    node_t *expected = create_binary_op_node(SUBTRACT_OPERATOR,
                                             create_binary_op_node(ADD_OPERATOR,
                                                                   create_literal_node(1),
                                                                   create_binary_op_node(MULTIPLY_OPERATOR,
                                                                                   create_literal_node(2),
                                                                                   create_literal_node(3))),
                                             create_binary_op_node(DIVIDE_OPERATOR,
                                                                   create_literal_node(4),
                                                                   create_literal_node(2)));
    node_equals(root, expected);
    // TODO: dependency to op prioritizing algorithm > better correctness check needed
    // for this case, ADD may come first

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

    node_t *expected = create_declaration_node(create_type_specifier_node(INTEGER_TYPE),
                                               create_identifier_node("x", DECLARATOR, IDENTIFIER));
    node_equals(root, expected);

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

    node_t *expected = create_declaration_node(create_type_specifier_node(INTEGER_TYPE),
                                               create_assign_op_node(EQ_ASSIGN,
                                                                     create_identifier_node("x", DECLARATOR, IDENTIFIER),
                                                                     create_literal_node(3)));
    node_equals(root, expected);

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

    node_t *expected = create_declaration_node(create_type_specifier_node(FLOAT_TYPE),
                                               create_assign_op_node(EQ_ASSIGN,
                                                                     create_identifier_node("hey", DECLARATOR, IDENTIFIER),
                                                                     create_literal_node(12.76f)));
    node_equals(root, expected);

    free_parser(parser);
}

TEST(ParserCompoundStatements, TwoIntegerStatements) {
    // int x = 12; x = x + 3;
    token_t **tokens = (token_t **)malloc(sizeof(token_t *) * 11);
    tokens[0] = generate_test_token(INTEGER_TYPE, "int");
    tokens[1] = generate_test_token(IDENTIFIER, "x");
    tokens[2] = generate_test_token(EQ_ASSIGN, "=");
    tokens[3] = generate_test_token(INTEGER_CONST, "12");
    tokens[4] = generate_test_token(SEMICOLON, ";");
    tokens[5] = generate_test_token(IDENTIFIER, "x");
    tokens[6] = generate_test_token(EQ_ASSIGN, "=");
    tokens[7] = generate_test_token(IDENTIFIER, "x");
    tokens[8] = generate_test_token(ADD_OPERATOR, "+");
    tokens[9] = generate_test_token(INTEGER_CONST, "3");
    tokens[10] = generate_test_token(SEMICOLON, ";");

    parser_handle parser = parser_init(tokens, 11);
    node_t *root = parse(parser);

    node_t **children = (node_t **)malloc(2 * sizeof(node_t *));
    children[0] = create_declaration_node(create_type_specifier_node(INTEGER_TYPE),
                                          create_assign_op_node(EQ_ASSIGN,
                                                                create_identifier_node("x", DECLARATOR, IDENTIFIER),
                                                                create_literal_node(12)));
    children[1] = create_assign_op_node(EQ_ASSIGN,
                                        create_identifier_node("x", ID, IDENTIFIER),
                                        create_binary_op_node(ADD_OPERATOR,
                                                              create_identifier_node("x", ID, IDENTIFIER),
                                                              create_literal_node(3)));
    node_t *expected = create_compound_statements_node(children, 2);

    node_equals(root, expected);

    free_parser(parser);

}