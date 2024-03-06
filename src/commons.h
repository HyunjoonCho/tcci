#ifndef COMMONS_H
#define COMMONS_H

typedef enum {
    IDENTIFIER,
    INTEGER_CONST,
    FLOAT_CONST,
    DECLARATION,
    ADD_OPERATOR,
    SUBTRACT_OPERATOR,
    MULTIPLY_OPERATOR,
    DIVIDE_OPERATOR,
    OPEN_PAREN,
    CLOSE_PAREN,
    INTEGER_TYPE,
    FLOAT_TYPE,
    EQ_ASSIGN,
    SEMICOLON,
    NULLTOKEN,
} type_t;

struct token_t {
    type_t type;
    char *value;
};

typedef enum {
    CONSTANT,
    ID, // FIXME: doesn't seem so nice...
    DECL, // FIXME: abbreviation for DECLARATION, this may cause confusion
    DECLARATOR,
    TYPE_SPECIFIER,
    BINARY_OP,
    ASSIGN_OP,
} node_type;

union value_t {
    char *id_name; 
    int int_value;
    float float_value;
};

typedef union value_t value_t;

typedef struct node_t node_t;
typedef struct dummy_node dummy_node;
typedef struct constant_node constant_node;

struct node_t {
    node_type type;
    void *actual_node;
};

struct dummy_node {
    type_t subtype;
    value_t value;
    int op_priority;
    node_t *left_child;
    node_t *right_child;
};

struct constant_node {
    type_t subtype;
    value_t value;
};

typedef struct token_t token_t;

typedef struct lexer lexer;
typedef lexer *lexer_handle;

lexer_handle lexer_init(const char *input);
token_t *get_next_token(lexer_handle lexer);
void free_tokens(token_t **tokens, int token_count);

typedef struct parser parser;
typedef parser *parser_handle;

parser_handle parser_init(token_t **tokens, int token_count);
node_t *parse(parser_handle parser);
void free_parser(parser_handle parser);
void free_node(node_t *root);

#endif