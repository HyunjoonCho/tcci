#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

struct tokenizer {
    char *input;
};

tokenizer_handle tokenizer_init(char *input) {
    tokenizer_handle new_tokenizer = malloc(sizeof(tokenizer));
    char *new_input = malloc(sizeof(*input));
    strcpy(new_input, input);
    new_tokenizer->input = new_input;
    return new_tokenizer;
}

token_t get_next_token(tokenizer_handle tokenizer) {
    token_t next_token;
    if (strchr(tokenizer->input, '+') != NULL) {
        next_token.type = ADD_OPERATOR;
    } else if (strchr(tokenizer->input, '-') != NULL) {
        next_token.type = SUBTRACT_OPERATOR;
    } else if (strchr(tokenizer->input, '.') != NULL) {
        next_token.type = FLOAT;
    } else {
        next_token.type = INTEGER;
    }
    char *new_value = malloc(sizeof(*tokenizer->input));
    strcpy(new_value,tokenizer->input);
    next_token.value = new_value;
    return next_token;
}