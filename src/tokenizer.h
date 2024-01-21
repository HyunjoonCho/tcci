typedef enum {
    INTEGER,
    FLOAT,
    ADD_OPERATOR,
    SUBTRACT_OPERATOR,
    UNKNOWN,
} token_type;

struct token_t {
    token_type type;
    char *value;
};

typedef struct token_t token_t;

typedef struct tokenizer tokenizer;
typedef tokenizer *tokenizer_handle;

tokenizer_handle tokenizer_init(const char *input);
token_t get_next_token(tokenizer_handle tokenizer);