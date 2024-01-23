typedef enum {
    INTEGER,
    FLOAT,
    ADD_OPERATOR,
    SUBTRACT_OPERATOR,
    MULTIPLY_OPERATOR,
    UNKNOWN,
    NULLTOKEN,
} token_type;

struct token_t {
    token_type type;
    char *value;
};

typedef enum {
    BINARY_OP,
    LITERAL,
} expr_type;

typedef enum {
    ADD,
    SUBTRACT,
    MULTIPLY,
} op_type;

struct expr_t {
    expr_type type;
    op_type op;
    struct expr_t *left_operand;
    struct expr_t *right_operand;
    char *value;
};

typedef struct token_t token_t;
typedef struct expr_t expr_t;

typedef struct tokenizer tokenizer;
typedef tokenizer *tokenizer_handle;

tokenizer_handle tokenizer_init(const char *input);
int get_current_token_length(tokenizer_handle tokenizer);
token_t get_next_token(tokenizer_handle tokenizer);

expr_t parse_expression(token_t *tokens,int token_count);