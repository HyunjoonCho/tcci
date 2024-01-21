#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

struct tokenizer {
    char *input;
};

tokenizer_handle tokenizer_init(const char *input) {
    tokenizer_handle new_tokenizer = malloc(sizeof(tokenizer));
    char *new_input = malloc(sizeof(*input));
    strcpy(new_input, input);
    new_tokenizer->input = new_input;
    return new_tokenizer;
}

token_type get_token_type_from(char *input) {
    if (strchr(input, '+') != NULL) {
        return ADD_OPERATOR;
    } else if (strchr(input, '-') != NULL) {
        return SUBTRACT_OPERATOR;
    } else if (strchr(input, '.') != NULL) {
        return FLOAT;
    } else {
        return INTEGER;
    }
}

token_t get_next_token(tokenizer_handle tokenizer) {
    token_t next_token;
    next_token.type = get_token_type_from(tokenizer->input);
    char *new_value = malloc(sizeof(*tokenizer->input));
    strcpy(new_value,tokenizer->input);
    next_token.value = new_value;
    return next_token;
}