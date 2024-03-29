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
    COMPOUND_STATEMENT,
    SEMICOLON,
    NULLTOKEN,
} type_t;

struct token_t {
    type_t type;
    char *value;
};

typedef enum {
    CONSTANT_NODE,
    IDENTIFIER_NODE,
    DECLARATION_NODE,
    DECLARATOR_NODE,
    TYPE_SPECIFIER_NODE,
    BINARY_OP_NODE,
    ASSIGN_OP_NODE,
    COMPOUND_STATEMENT_NODE,
} node_type;

union value_t {
    int int_value;
    float float_value;
};

typedef union value_t value_t;

typedef struct node_t node_t;
typedef struct constant_node constant_node;
typedef struct binary_op_node binary_op_node;
typedef struct assign_op_node assign_op_node;
typedef struct identifier_node identifier_node; // DECLARTOR node shares this struct
typedef struct type_specifier_node type_specifier_node;
typedef struct declaration_node declaration_node;
typedef struct compound_statment_node compound_statment_node;

struct node_t {
    node_type type;
    void *actual_node;
};

struct constant_node {
    type_t subtype;
    value_t value;
};

struct binary_op_node {
    type_t subtype;
    int op_priority;
    node_t *left_child;
    node_t *right_child;
};

struct assign_op_node {
    type_t subtype;
    int op_priority;
    node_t *left_child;
    node_t *right_child;
};

struct identifier_node {
    char *id_name;
};

struct type_specifier_node {
    type_t subtype;
};

struct declaration_node {
    type_t subtype;
    int op_priority;
    node_t *left_child;
    node_t *right_child;
};

struct compound_statment_node {
    int children_count;
    node_t **children; // declaration || statment
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