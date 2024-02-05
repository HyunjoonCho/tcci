#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "commons.h"
#include "interpreter.h"

int main() {
    lexer_handle lexer = lexer_init("123");
    token_t *token = get_next_token(lexer);
    return 0;
}