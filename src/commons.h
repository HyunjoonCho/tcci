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
    INTEGER_TYPE,
    FLOAT_TYPE,
    IDENTIFIER,
    ASSIGN,
    SEMICOLON,
    NULLTOKEN,
} token_type;

struct token_t {
    token_type type;
    char *value;
};

typedef enum {
    BINARY_OP,
    CONSTANT,
} expr_type;

typedef enum {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
} op_type;

typedef enum {
    INTEGER_CONST,
    FLOAT_CONST,
} const_type;

union literal_value {
    int int_value;
    float float_value;
};

typedef union literal_value literal_value;

union expr_subtype {
    op_type op_t;
    const_type const_t;
};

typedef union expr_subtype expr_subtype;

struct expr_t {
    expr_type type;
    expr_subtype subtype;
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