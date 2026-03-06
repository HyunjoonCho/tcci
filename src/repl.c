#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commons.h"
#include "interpreter.h"

static void print_literal(const literal_t *literal) {
    if (literal->type == INTEGER_CONST) {
        printf("%d\n", literal->value.int_value);
    } else {
        printf("%f\n", literal->value.float_value);
    }
}

static bool append_token(token_t ***tokens, int *count, int *capacity, token_t *token) {
    token_t **grown;
    int new_capacity;

    if (*count < *capacity) {
        (*tokens)[(*count)++] = token;
        return true;
    }

    new_capacity = (*capacity == 0) ? 16 : (*capacity * 2);
    grown = (token_t **)realloc(*tokens, (size_t)new_capacity * sizeof(token_t *));
    if (grown == NULL) {
        return false;
    }

    *tokens = grown;
    *capacity = new_capacity;
    (*tokens)[(*count)++] = token;
    return true;
}

static bool is_blank_line(const char *line) {
    while (*line != '\0') {
        if (!isspace((unsigned char)*line)) {
            return false;
        }
        line++;
    }
    return true;
}

static void print_error(const char *phase, tcci_status_t status, const char *message) {
    fprintf(stderr, "[%s][%d] %s\n", phase, (int)status, message);
}

int main(void) {
    char *line = NULL;
    size_t line_capacity = 0;

    while (true) {
        ssize_t read_count;
        lexer_handle lexer = NULL;
        token_t **tokens = NULL;
        int token_count = 0;
        int token_capacity = 0;
        parser_handle parser = NULL;
        node_t *root = NULL;
        interpreter_handle interpreter = NULL;
        literal_t *result = NULL;

        printf(">>> ");
        fflush(stdout);

        read_count = getline(&line, &line_capacity, stdin);
        if (read_count < 0) {
            printf("Exiting...\n");
            break;
        }

        if (read_count > 0 && line[read_count - 1] == '\n') {
            line[read_count - 1] = '\0';
        }

        if (strcmp(line, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        if (is_blank_line(line)) {
            continue;
        }

        lexer = lexer_init(line);
        if (lexer == NULL) {
            print_error("lexer", TCCI_STATUS_OOM, "Failed to create lexer");
            continue;
        }

        while (true) {
            token_t *token = get_next_token(lexer);

            if (token == NULL) {
                print_error("lexer", TCCI_STATUS_OOM, "Failed to allocate token");
                break;
            }

            if (lexer_status(lexer) != TCCI_STATUS_OK) {
                free(token->value);
                free(token);
                print_error("lexer", lexer_status(lexer), lexer_error_message(lexer));
                break;
            }

            if (token->type == NULLTOKEN) {
                free(token->value);
                free(token);
                break;
            }

            if (!append_token(&tokens, &token_count, &token_capacity, token)) {
                free(token->value);
                free(token);
                print_error("lexer", TCCI_STATUS_OOM, "Failed to grow token buffer");
                break;
            }
        }

        if (lexer_status(lexer) != TCCI_STATUS_OK) {
            free_tokens(tokens, token_count);
            free(tokens);
            lexer_free(lexer);
            continue;
        }

        parser = parser_init(tokens, token_count);
        if (parser == NULL) {
            print_error("parser", TCCI_STATUS_OOM, "Failed to create parser");
            free_tokens(tokens, token_count);
            free(tokens);
            lexer_free(lexer);
            continue;
        }

        root = parse(parser);
        if (parser_status(parser) != TCCI_STATUS_OK || root == NULL) {
            print_error("parser", parser_status(parser), parser_error_message(parser));
            free_parser(parser);
            free_tokens(tokens, token_count);
            free(tokens);
            lexer_free(lexer);
            continue;
        }

        interpreter = interpreter_init(root);
        if (interpreter == NULL) {
            print_error("runtime", TCCI_STATUS_OOM, "Failed to create interpreter");
            free_node(root);
            free_parser(parser);
            free_tokens(tokens, token_count);
            free(tokens);
            lexer_free(lexer);
            continue;
        }

        result = interpret(interpreter);
        if (interpreter_status(interpreter) != TCCI_STATUS_OK || result == NULL) {
            print_error("runtime", interpreter_status(interpreter), interpreter_error_message(interpreter));
        } else {
            print_literal(result);
        }

        free(result);
        interpreter_free(interpreter);
        free_node(root);
        free_parser(parser);
        free_tokens(tokens, token_count);
        free(tokens);
        lexer_free(lexer);
    }

    free(line);
    return 0;
}
