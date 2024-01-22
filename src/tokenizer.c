#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"

struct tokenizer {
    int current;
    char *input;
};

tokenizer_handle tokenizer_init(const char *input) {
    tokenizer_handle new_tokenizer = malloc(sizeof(tokenizer));
    new_tokenizer->current = 0;
    char *new_input = malloc(sizeof(*input));
    strcpy(new_input, input);
    new_tokenizer->input = new_input;
    return new_tokenizer;
}

token_type get_token_type_from(char *token) {
    if (strchr(token, '+') != NULL) {
        return ADD_OPERATOR;
    } else if (strchr(token, '-') != NULL) {
        return SUBTRACT_OPERATOR;
    } else if (strchr(token, '*') != NULL) {
        return MULTIPLY_OPERATOR;
    } else if (strchr(token, '.') != NULL) {
        return FLOAT;
    } else {
        return INTEGER;
    }
}

int get_current_token_length(tokenizer_handle tokenizer) {
    char *token = tokenizer->input + tokenizer->current;
    size_t remaining_length = strlen(token);

    int token_length = remaining_length;
    for (int i = 1; i < remaining_length; i++) {
        if (token[i] == ' ') {
            token_length = i;
            break;
        }
    }
    return token_length;
}

token_t get_next_token(tokenizer_handle tokenizer) {
    if (tokenizer->current >= strlen(tokenizer->input)) {
        token_t null_token;
        null_token.type = NULLTOKEN;
        return null_token;
    }

    int token_length = get_current_token_length(tokenizer);

    token_t next_token;
    char *new_value = malloc(sizeof(token_length));
    strncpy(new_value,tokenizer->input + tokenizer->current, token_length);
    next_token.type = get_token_type_from(new_value);
    next_token.value = new_value;

    tokenizer->current += token_length + 1;

    return next_token;
}