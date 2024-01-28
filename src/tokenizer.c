#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commons.h"

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
    } else if (strchr(token, '/') != NULL) {
        return DIVIDE_OPERATOR;
    } else if (strchr(token, '(') != NULL) {
        return OPEN_PAREN;
    } else if (strchr(token, ')') != NULL) {
        return CLOSE_PAREN;
    } else if (strchr(token, '.') != NULL) {
        return FLOAT;
    } else {
        return INTEGER;
    }
}

int is_predefined_char(char c) {
    return strchr("+-*/()", c) != NULL;
}

int is_numeric(char c) {
    return (c >= '0' && c <= '9');
}

void remove_leading_spaces(tokenizer_handle tokenizer) {
    char *token = tokenizer->input + tokenizer->current;
    size_t remaining_length = strlen(token);

    for (int i = 0; i < remaining_length; i++) {
        if (token[i] != ' ') {
            tokenizer->current += i;
            remaining_length -= i;
            break;
        }
    }
}

int get_current_token_length(tokenizer_handle tokenizer) {
    char *token = tokenizer->input + tokenizer->current;
    size_t remaining_length = strlen(token);

    if (is_predefined_char(token[0])) return 1;

    int is_float_token = 0;
    for (int i = 0; i < remaining_length; i++) {
        if (!is_numeric(token[i])) {
            if (token[i] == '.' && !is_float_token) { // no exception handling till now
                is_float_token = 1;
            } else {
                return i;
            }
        }
    }
    return remaining_length;
}

token_t *get_next_token(tokenizer_handle tokenizer) {
    token_t *next_token = malloc(sizeof(token_t));
    if (tokenizer->current >= strlen(tokenizer->input)) {
        next_token->type = NULLTOKEN;
        return next_token;
    }

    remove_leading_spaces(tokenizer);
    int token_length = get_current_token_length(tokenizer);
    char *new_value = malloc(sizeof(token_length));
    strncpy(new_value,tokenizer->input + tokenizer->current, token_length);
    next_token->type = get_token_type_from(new_value);
    next_token->value = new_value;

    tokenizer->current += token_length;

    return next_token;
}

void free_tokens(token_t **tokens, int token_count) {
    for (int i = 0; i < token_count; i++) {
        token_t *token = tokens[i];
        free(token->value);
        free(token);
    }
}