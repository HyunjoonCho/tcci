#ifndef COMMONS_H
#define COMMONS_H

typedef enum {
    INTEGER,
    FLOAT,
    ADD_OPERATOR,
    SUBTRACT_OPERATOR,
    MULTIPLY_OPERATOR,
    DIVIDE_OPERATOR,
    OPEN_PAREN,
    CLOSE_PAREN,
    NULLTOKEN,
} token_type;

struct token_t {
    token_type type;
    char *value;
};

typedef enum {
    BINARY_OP,
    INTEGER_EXPR,
    FLOAT_EXPR,
} expr_type;

typedef enum {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
} op_type;

union literal_value {
    int int_value;
    float float_value;
};

typedef union literal_value literal_value;

struct expr_t {
    expr_type type;
    op_type op;
    literal_value value;
    int op_priority;
    struct expr_t *left_operand;
    struct expr_t *right_operand;
};

typedef struct token_t token_t;
typedef struct expr_t expr_t;

typedef struct tokenizer tokenizer;
typedef tokenizer *tokenizer_handle;

tokenizer_handle tokenizer_init(const char *input);
token_t *get_next_token(tokenizer_handle tokenizer);
void free_tokens(token_t **tokens, int token_count);

expr_t *parse_expression(token_t **tokens,int token_count);
void free_expr(expr_t *root);

#endif