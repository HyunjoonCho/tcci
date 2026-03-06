#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commons.h"
#include "interpreter.h"

static token_t *generate_test_token(type_t type, const char *value) {
    token_t *new_token = (token_t *)malloc(sizeof(token_t));
    size_t length = strlen(value);

    if (new_token == NULL) {
        return NULL;
    }

    new_token->type = type;
    new_token->value = (char *)malloc(length + 1);
    if (new_token->value == NULL) {
        free(new_token);
        return NULL;
    }

    memcpy(new_token->value, value, length + 1);
    return new_token;
}

int main(void) {
    token_t **tokens = (token_t **)malloc(5 * sizeof(token_t *));
    parser_handle parser;
    node_t *root;

    if (tokens == NULL) {
        fprintf(stderr, "failed to allocate token buffer\n");
        return 1;
    }

    tokens[0] = generate_test_token(INTEGER_TYPE, "int");
    tokens[1] = generate_test_token(IDENTIFIER, "x");
    tokens[2] = generate_test_token(EQ_ASSIGN, "=");
    tokens[3] = generate_test_token(INTEGER_CONST, "3");
    tokens[4] = generate_test_token(SEMICOLON, ";");

    parser = parser_init(tokens, 5);
    if (parser == NULL) {
        fprintf(stderr, "failed to create parser\n");
        free_tokens(tokens, 5);
        free(tokens);
        return 1;
    }

    root = parse(parser);
    if (root == NULL) {
        fprintf(stderr, "parse failed: %s\n", parser_error_message(parser));
        free_parser(parser);
        free_tokens(tokens, 5);
        free(tokens);
        return 1;
    }

    printf("parse ok\n");

    free_node(root);
    free_parser(parser);
    free_tokens(tokens, 5);
    free(tokens);
    return 0;
}
