#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "commons.h"
#include "interpreter.h"

token_t *generate_test_token(type_t type, const char *value) {
    token_t *new_token = (token_t *)malloc(sizeof(token_t));
    new_token->type = type;
    new_token->value = strdup(value);

    return new_token;
}

int main() {
    token_t *tokens[] = {
        generate_test_token(INTEGER_TYPE, "int"),
        generate_test_token(IDENTIFIER, "x"),
        generate_test_token(EQ_ASSIGN, "="),
        generate_test_token(INTEGER_CONST, "3"),
        generate_test_token(SEMICOLON, ";"),
    };

    parser_handle parser = parser_init(tokens, 5);
    node_t *root = parse(parser);    
    return 0;
}