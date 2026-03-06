#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commons.h"

#define ERROR_MESSAGE_SIZE 256

struct lexer {
    size_t current;
    char *input;
    tcci_status_t status;
    char error_message[ERROR_MESSAGE_SIZE];
};

static void lexer_set_error(lexer_handle lexer, tcci_status_t status, const char *fmt, ...) {
    va_list args;

    if (lexer == NULL || lexer->status != TCCI_STATUS_OK) {
        return;
    }

    lexer->status = status;
    va_start(args, fmt);
    (void)vsnprintf(lexer->error_message, sizeof(lexer->error_message), fmt, args);
    va_end(args);
}

static token_t *allocate_token(lexer_handle lexer, type_t type, const char *start, size_t length) {
    token_t *token = (token_t *)malloc(sizeof(token_t));
    if (token == NULL) {
        lexer_set_error(lexer, TCCI_STATUS_OOM, "Failed to allocate token object");
        return NULL;
    }

    token->type = type;
    token->value = NULL;

    if (start != NULL) {
        token->value = (char *)malloc(length + 1);
        if (token->value == NULL) {
            free(token);
            lexer_set_error(lexer, TCCI_STATUS_OOM, "Failed to allocate token value");
            return NULL;
        }

        memcpy(token->value, start, length);
        token->value[length] = '\0';
    }

    return token;
}

static void skip_whitespace(lexer_handle lexer) {
    while (lexer->input[lexer->current] != '\0' && isspace((unsigned char)lexer->input[lexer->current])) {
        lexer->current++;
    }
}

static bool is_symbol(char c) {
    return strchr("+-*/();=", c) != NULL;
}

static type_t symbol_to_type(char c) {
    switch (c) {
    case '+':
        return ADD_OPERATOR;
    case '-':
        return SUBTRACT_OPERATOR;
    case '*':
        return MULTIPLY_OPERATOR;
    case '/':
        return DIVIDE_OPERATOR;
    case '(':
        return OPEN_PAREN;
    case ')':
        return CLOSE_PAREN;
    case ';':
        return SEMICOLON;
    case '=':
        return EQ_ASSIGN;
    default:
        return NULLTOKEN;
    }
}

static type_t identifier_to_type(const char *identifier) {
    if (strcmp(identifier, "int") == 0) {
        return INTEGER_TYPE;
    }
    if (strcmp(identifier, "float") == 0) {
        return FLOAT_TYPE;
    }
    return IDENTIFIER;
}

lexer_handle lexer_init(const char *input) {
    lexer_handle new_lexer = (lexer_handle)malloc(sizeof(lexer));
    if (new_lexer == NULL) {
        return NULL;
    }

    if (input == NULL) {
        input = "";
    }

    new_lexer->input = strdup(input);
    if (new_lexer->input == NULL) {
        free(new_lexer);
        return NULL;
    }

    new_lexer->current = 0;
    new_lexer->status = TCCI_STATUS_OK;
    new_lexer->error_message[0] = '\0';

    return new_lexer;
}

void lexer_free(lexer_handle lexer) {
    if (lexer == NULL) {
        return;
    }

    free(lexer->input);
    free(lexer);
}

tcci_status_t lexer_status(lexer_handle lexer) {
    if (lexer == NULL) {
        return TCCI_STATUS_LEXER_ERROR;
    }
    return lexer->status;
}

const char *lexer_error_message(lexer_handle lexer) {
    if (lexer == NULL) {
        return "Lexer handle is NULL";
    }
    return lexer->error_message;
}

static token_t *lex_number(lexer_handle lexer) {
    size_t start = lexer->current;
    bool seen_dot = false;

    while (lexer->input[lexer->current] != '\0') {
        char c = lexer->input[lexer->current];
        if (isdigit((unsigned char)c)) {
            lexer->current++;
            continue;
        }

        if (c == '.') {
            if (seen_dot) {
                lexer_set_error(lexer, TCCI_STATUS_LEXER_ERROR,
                                "Invalid number with multiple dots near index %zu", lexer->current);
                return allocate_token(lexer, NULLTOKEN, NULL, 0);
            }
            seen_dot = true;
            lexer->current++;
            continue;
        }

        break;
    }

    return allocate_token(lexer, seen_dot ? FLOAT_CONST : INTEGER_CONST,
                          lexer->input + start, lexer->current - start);
}

static token_t *lex_identifier(lexer_handle lexer) {
    size_t start = lexer->current;

    lexer->current++;
    while (lexer->input[lexer->current] != '\0') {
        char c = lexer->input[lexer->current];
        if (!isalnum((unsigned char)c) && c != '_') {
            break;
        }
        lexer->current++;
    }

    token_t *token = allocate_token(lexer, IDENTIFIER, lexer->input + start, lexer->current - start);
    if (token == NULL || token->value == NULL) {
        return token;
    }

    token->type = identifier_to_type(token->value);
    return token;
}

token_t *get_next_token(lexer_handle lexer) {
    if (lexer == NULL) {
        return NULL;
    }

    if (lexer->status != TCCI_STATUS_OK) {
        return allocate_token(lexer, NULLTOKEN, NULL, 0);
    }

    skip_whitespace(lexer);

    if (lexer->input[lexer->current] == '\0') {
        return allocate_token(lexer, NULLTOKEN, NULL, 0);
    }

    char current_char = lexer->input[lexer->current];
    if (is_symbol(current_char)) {
        lexer->current++;
        return allocate_token(lexer, symbol_to_type(current_char), &current_char, 1);
    }

    if (isdigit((unsigned char)current_char)) {
        return lex_number(lexer);
    }

    if (isalpha((unsigned char)current_char) || current_char == '_') {
        return lex_identifier(lexer);
    }

    lexer_set_error(lexer, TCCI_STATUS_LEXER_ERROR,
                    "Unexpected character '%c' at index %zu", current_char, lexer->current);
    lexer->current++;
    return allocate_token(lexer, NULLTOKEN, NULL, 0);
}

void free_tokens(token_t **tokens, int token_count) {
    int i;

    if (tokens == NULL) {
        return;
    }

    for (i = 0; i < token_count; i++) {
        if (tokens[i] == NULL) {
            continue;
        }

        free(tokens[i]->value);
        free(tokens[i]);
    }
}
