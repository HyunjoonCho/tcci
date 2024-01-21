#include <stdlib.h>
#include "tokenizer.h"

struct tokenizer {
    char input[100];
};

tokenizer_handle tokenizer_init(char *input) {
    tokenizer_handle new_tokenizer = malloc(sizeof(tokenizer));
    return 0;
}

token_t get_next_token(tokenizer_handle tokenizer) {
    token_t next_token;
    next_token.type = UNKNOWN;
    next_token.value = "";
    return next_token;
}