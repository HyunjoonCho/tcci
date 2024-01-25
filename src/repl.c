#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commons.h"
#include "interpreter.h"

void print_literal(literal_t *literal) {
    if (literal->is_int) printf("%d\n", literal->value.int_value);
    else printf("%f\n", literal->value.float_value);
}

int main() {
    char user_input[100]; // Assume: fixed buffer size for input

    while (1) {
        printf(">>> ");
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
            printf("Error or end of input.\n");
            break;
        }
        size_t len = strcspn(user_input, "\n") + 1;
        char *input = malloc(len + 1);
        strncpy(input, user_input, len);

        if (strncmp(input, "exit", 4) == 0) {
            printf("Exiting...\n");
            free(input);
            break;
        } else {
            int token_count = 0;

            tokenizer_handle tokenizer = tokenizer_init(input);
            token_t **tokens = malloc(20 * sizeof(token_t)); // Assume: tokens being less than 20

            token_t *token = get_next_token(tokenizer); // Assume: at least one token?
            while (token->type != NULLTOKEN) {
                tokens[token_count] = token;
                token_count++;
                token = get_next_token(tokenizer);
            }

            expr_t *expr = parse_expression(tokens, token_count);
            literal_t *literal = interpret_expr(expr);
            print_literal(literal);           
        }

        free(input);
    }

    return 0;
}