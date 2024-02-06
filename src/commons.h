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
} type_t;

struct token_t {
    type_t type;
    char *value;
};

typedef enum {
    BINARY_OP,
    CONSTANT,
} node_type;

union literal_value {
    int int_value;
    float float_value;
};

typedef union literal_value literal_value;

typedef struct node_t node_t;
struct node_t {
    node_type type;
    type_t subtype;
    literal_value value;
    int op_priority;
    node_t *left_child;
    node_t *right_child;
};

typedef struct token_t token_t;

typedef struct lexer lexer;
typedef lexer *lexer_handle;

lexer_handle lexer_init(const char *input);
token_t *get_next_token(lexer_handle lexer);
void free_tokens(token_t **tokens, int token_count);

node_t *parse(token_t **tokens,int token_count);
void free_node(node_t *root);

#endif