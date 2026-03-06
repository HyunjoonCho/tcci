#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/commons.h"

#define TEST_ERROR_SIZE 256

typedef struct {
    token_t **tokens;
    int count;
} token_buffer_t;

static inline void token_buffer_init(token_buffer_t *buffer) {
    buffer->tokens = NULL;
    buffer->count = 0;
}

static inline void token_buffer_free(token_buffer_t *buffer) {
    free_tokens(buffer->tokens, buffer->count);
    free(buffer->tokens);
    buffer->tokens = NULL;
    buffer->count = 0;
}

static inline int token_buffer_append(token_buffer_t *buffer, token_t *token) {
    token_t **grown = (token_t **)realloc(buffer->tokens,
                                          (size_t)(buffer->count + 1) * sizeof(token_t *));
    if (grown == NULL) {
        return 1;
    }

    buffer->tokens = grown;
    buffer->tokens[buffer->count++] = token;
    return 0;
}

static inline int tokenize_input(const char *input, token_buffer_t *out, char *error_message, size_t message_size) {
    lexer_handle lexer = lexer_init(input);

    if (lexer == NULL) {
        (void)snprintf(error_message, message_size, "failed to initialize lexer");
        return 1;
    }

    token_buffer_init(out);

    while (1) {
        token_t *token = get_next_token(lexer);

        if (token == NULL) {
            (void)snprintf(error_message, message_size, "failed to allocate token");
            token_buffer_free(out);
            lexer_free(lexer);
            return 1;
        }

        if (lexer_status(lexer) != TCCI_STATUS_OK) {
            (void)snprintf(error_message, message_size, "%s", lexer_error_message(lexer));
            free(token->value);
            free(token);
            token_buffer_free(out);
            lexer_free(lexer);
            return 1;
        }

        if (token->type == NULLTOKEN) {
            free(token->value);
            free(token);
            break;
        }

        if (token_buffer_append(out, token) != 0) {
            (void)snprintf(error_message, message_size, "failed to grow token buffer");
            free(token->value);
            free(token);
            token_buffer_free(out);
            lexer_free(lexer);
            return 1;
        }
    }

    lexer_free(lexer);
    return 0;
}

static inline int parse_input(const char *input, node_t **out_root, char *error_message, size_t message_size) {
    token_buffer_t tokens;
    parser_handle parser;
    node_t *root;

    if (tokenize_input(input, &tokens, error_message, message_size) != 0) {
        return 1;
    }

    parser = parser_init(tokens.tokens, tokens.count);
    if (parser == NULL) {
        (void)snprintf(error_message, message_size, "failed to initialize parser");
        token_buffer_free(&tokens);
        return 1;
    }

    root = parse(parser);
    if (root == NULL || parser_status(parser) != TCCI_STATUS_OK) {
        (void)snprintf(error_message, message_size, "%s", parser_error_message(parser));
        free_parser(parser);
        token_buffer_free(&tokens);
        return 1;
    }

    free_parser(parser);
    token_buffer_free(&tokens);
    *out_root = root;
    return 0;
}

#endif
