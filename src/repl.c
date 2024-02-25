#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commons.h"
#include "interpreter.h"

void print_literal(literal_t *literal) {
    if (literal->type == INTEGER_CONST) printf("%d\n", literal->value.int_value);
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
        size_t len = strcspn(user_input, "\n");
        char *input = malloc(len + 1);
        strncpy(input, user_input, len);
        input[len] = '\0';

        if (strncmp(input, "exit", 4) == 0) {
            printf("Exiting...\n");
            free(input);
            break;
        } else {
            int token_count = 0;

            lexer_handle lexer = lexer_init(input);
            token_t **tokens = malloc(20 * sizeof(token_t)); // Assume: tokens being less than 20

            token_t *token = get_next_token(lexer); // Assume: at least one token?
            while (token->type != NULLTOKEN) {
                tokens[token_count] = token;
                token_count++;
                token = get_next_token(lexer);
            }
            parser_handle parser = parser_init(tokens, token_count);
            node_t *node = parse(parser);
            interpreter_handle interpreter = interpreter_init(node);
            literal_t *literal = interpret(interpreter);
            print_literal(literal);

            free_tokens(tokens, token_count);
            free(tokens);

            free_node(node);
            free(literal);
        }

        free(input);
    }

    return 0;
}