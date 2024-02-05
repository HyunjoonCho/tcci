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
} node_type;

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

union node_subtype {
    op_type op_t;
    const_type const_t;
};

typedef union node_subtype node_subtype;

struct node_t {
    node_type type;
    node_subtype subtype;
    literal_value value;
    int op_priority;
    struct node_t *left_operand;
    struct node_t *right_operand;
};

typedef struct token_t token_t;
typedef struct node_t node_t;

typedef struct tokenizer tokenizer;
typedef tokenizer *tokenizer_handle;

tokenizer_handle tokenizer_init(const char *input);
token_t *get_next_token(tokenizer_handle tokenizer);
void free_tokens(token_t **tokens, int token_count);

node_t *parse(token_t **tokens,int token_count);
void free_node(node_t *root);

#endif