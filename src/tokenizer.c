#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "commons.h"

struct tokenizer {
    int current;
    char *input;
};

tokenizer_handle tokenizer_init(const char *input) {
    tokenizer_handle new_tokenizer = malloc(sizeof(tokenizer));
    new_tokenizer->current = 0;
    char *new_input = malloc(strlen(input) + 1);
    strcpy(new_input, input);
    new_tokenizer->input = new_input;
    return new_tokenizer;
}

bool is_predefined_char(char c) {
    return strchr("+-*/();=", c) != NULL;
}

bool is_numeric(char c) {
    return (c >= '0' && c <= '9');
}

bool is_alphabet(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

short is_number_token(char *token, int token_length) {
    bool is_float = false;
    for (int i = 0; i < token_length; i++) {
        if (!is_numeric(token[i])) {
            if (!is_float && token[i] == '.') is_float = true;
            else return -1; 
        } 
    }
    return is_float ? 1 : 0;
}

token_type get_token_type_from(char *token, int token_length) {
    if (token_length == 1 && strchr(token, '+') != NULL) {
        return ADD_OPERATOR;
    } else if (token_length == 1 && strchr(token, '-') != NULL) {
        return SUBTRACT_OPERATOR;
    } else if (token_length == 1 && strchr(token, '*') != NULL) {
        return MULTIPLY_OPERATOR;
    } else if (token_length == 1 && strchr(token, '/') != NULL) {
        return DIVIDE_OPERATOR;
    } else if (token_length == 1 && strchr(token, '(') != NULL) {
        return OPEN_PAREN;
    } else if (token_length == 1 && strchr(token, ')') != NULL) {
        return CLOSE_PAREN;
    } else if (token_length == 1 && strchr(token, '.') != NULL) {
        return FLOAT;
    } else if (token_length == 1 && strchr(token, ';') != NULL) {
        return SEMICOLON;
    } else if (token_length == 1 && strchr(token, '=') != NULL) {
        return ASSIGN;
    } else if (strcmp(token, "int") == 0){
        return INTEGER_TYPE;
    } else if (strcmp(token, "float") == 0) {
        return FLOAT_TYPE;
    } else {
        short number_token_type = is_number_token(token, token_length);
        if (number_token_type == 0) return INTEGER;
        else if (number_token_type == 1) return FLOAT;
        else return IDENTIFIER;
    }
}

void remove_leading_spaces(tokenizer_handle tokenizer) {
    char *token = tokenizer->input + tokenizer->current;
    size_t remaining_length = strlen(token);

    for (int i = 0; i < remaining_length; i++) {
        if (token[i] != ' ') {
            tokenizer->current += i;
            break;
        }
    }
}

int get_next_token_length(tokenizer_handle tokenizer) {
    char *token = tokenizer->input + tokenizer->current;
    size_t remaining_length = strlen(token);

    if (is_predefined_char(token[0])) return 1;

    short token_type = 0;
    if (is_numeric(token[0])) token_type = 0;
    if (is_alphabet(token[0])) token_type = 1;

    for (int i = 0; i < remaining_length; i++) {
        if ((token_type == 0 && (!is_numeric(token[i]) && token[i] != '.')) || 
            (token_type == 1 && (!is_alphabet(token[i]) && token[i] != '_'))) {
            return i;
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
    int token_length = get_next_token_length(tokenizer);
    char *new_value = malloc(token_length + 1);
    strncpy(new_value, tokenizer->input + tokenizer->current, token_length);
    new_value[token_length] = '\0';
    next_token->type = get_token_type_from(new_value, token_length);
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