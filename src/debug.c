#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "commons.h"
#include "interpreter.h"

int main() {
    tokenizer_handle tokenizer = tokenizer_init("123");
    token_t *token = get_next_token(tokenizer);
    return 0;
}