#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"

#define ERROR_MESSAGE_SIZE 256

struct interpreter {
    size_t count;
    size_t capacity;
    identifier_t *context;
    node_t *root;
    tcci_status_t status;
    char error_message[ERROR_MESSAGE_SIZE];
};

static void interpreter_set_error(interpreter_handle interpreter, tcci_status_t status,
                                  const char *fmt, ...) {
    va_list args;

    if (interpreter == NULL || interpreter->status != TCCI_STATUS_OK) {
        return;
    }

    interpreter->status = status;
    va_start(args, fmt);
    (void)vsnprintf(interpreter->error_message, sizeof(interpreter->error_message), fmt, args);
    va_end(args);
}

static char *duplicate_string(interpreter_handle interpreter, const char *source) {
    size_t length;
    char *copy;

    if (source == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                              "Identifier name must not be NULL");
        return NULL;
    }

    length = strlen(source);
    copy = (char *)malloc(length + 1);
    if (copy == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_OOM,
                              "Failed to allocate identifier name");
        return NULL;
    }

    memcpy(copy, source, length + 1);
    return copy;
}

static literal_t *new_int_literal(interpreter_handle interpreter, int value) {
    literal_t *literal = (literal_t *)malloc(sizeof(literal_t));

    if (literal == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_OOM,
                              "Failed to allocate integer literal");
        return NULL;
    }

    literal->type = INTEGER_CONST;
    literal->value.int_value = value;
    return literal;
}

static literal_t *new_float_literal(interpreter_handle interpreter, float value) {
    literal_t *literal = (literal_t *)malloc(sizeof(literal_t));

    if (literal == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_OOM,
                              "Failed to allocate float literal");
        return NULL;
    }

    literal->type = FLOAT_CONST;
    literal->value.float_value = value;
    return literal;
}

static literal_t *copy_literal(interpreter_handle interpreter, const literal_t *source) {
    if (source->type == INTEGER_CONST) {
        return new_int_literal(interpreter, source->value.int_value);
    }
    return new_float_literal(interpreter, source->value.float_value);
}

static float literal_as_float(const literal_t *literal) {
    if (literal->type == INTEGER_CONST) {
        return (float)literal->value.int_value;
    }
    return literal->value.float_value;
}

interpreter_handle interpreter_init(node_t *root) {
    interpreter_handle new_interpreter = (interpreter_handle)malloc(sizeof(interpreter));

    if (new_interpreter == NULL) {
        return NULL;
    }

    new_interpreter->capacity = 8;
    new_interpreter->count = 0;
    new_interpreter->context = (identifier_t *)calloc(new_interpreter->capacity, sizeof(identifier_t));
    if (new_interpreter->context == NULL) {
        free(new_interpreter);
        return NULL;
    }

    new_interpreter->root = root;
    new_interpreter->status = TCCI_STATUS_OK;
    new_interpreter->error_message[0] = '\0';

    return new_interpreter;
}

void interpreter_free(interpreter_handle interpreter) {
    size_t i;

    if (interpreter == NULL) {
        return;
    }

    for (i = 0; i < interpreter->count; i++) {
        free(interpreter->context[i].id_name);
    }

    free(interpreter->context);
    free(interpreter);
}

tcci_status_t interpreter_status(interpreter_handle interpreter) {
    if (interpreter == NULL) {
        return TCCI_STATUS_RUNTIME_ERROR;
    }

    return interpreter->status;
}

const char *interpreter_error_message(interpreter_handle interpreter) {
    if (interpreter == NULL) {
        return "Interpreter handle is NULL";
    }

    return interpreter->error_message;
}

static identifier_t *lookup(interpreter_handle interpreter, const char *id_name) {
    size_t i;

    for (i = 0; i < interpreter->count; i++) {
        if (strcmp(interpreter->context[i].id_name, id_name) == 0) {
            return &interpreter->context[i];
        }
    }

    return NULL;
}

literal_t *get_value_of(interpreter_handle interpreter, const char *id_name) {
    identifier_t *identifier = lookup(interpreter, id_name);

    if (identifier == NULL) {
        return NULL;
    }

    return &identifier->literal;
}

static bool ensure_context_capacity(interpreter_handle interpreter) {
    identifier_t *grown;
    size_t new_capacity;

    if (interpreter->count < interpreter->capacity) {
        return true;
    }

    new_capacity = interpreter->capacity * 2;
    grown = (identifier_t *)realloc(interpreter->context, new_capacity * sizeof(identifier_t));
    if (grown == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_OOM,
                              "Failed to grow interpreter context");
        return false;
    }

    memset(grown + interpreter->capacity, 0,
           (new_capacity - interpreter->capacity) * sizeof(identifier_t));
    interpreter->context = grown;
    interpreter->capacity = new_capacity;
    return true;
}

static bool initialize_identifier_literal(identifier_t *identifier, type_t specifier_type) {
    if (specifier_type == INTEGER_TYPE) {
        identifier->literal.type = INTEGER_CONST;
        identifier->literal.value.int_value = 0;
        return true;
    }

    if (specifier_type == FLOAT_TYPE) {
        identifier->literal.type = FLOAT_CONST;
        identifier->literal.value.float_value = 0.0f;
        return true;
    }

    return false;
}

static void assign_identifier_literal(identifier_t *identifier, const literal_t *value) {
    if (identifier->literal.type == INTEGER_CONST) {
        if (value->type == INTEGER_CONST) {
            identifier->literal.value.int_value = value->value.int_value;
        } else {
            identifier->literal.value.int_value = (int)value->value.float_value;
        }
    } else {
        if (value->type == INTEGER_CONST) {
            identifier->literal.value.float_value = (float)value->value.int_value;
        } else {
            identifier->literal.value.float_value = value->value.float_value;
        }
    }
}

static const char *extract_identifier_name(node_t *identifier_node_ref) {
    if (identifier_node_ref == NULL) {
        return NULL;
    }

    if (identifier_node_ref->type != IDENTIFIER_NODE &&
        identifier_node_ref->type != DECLARATOR_NODE) {
        return NULL;
    }

    return ((identifier_node *)identifier_node_ref->actual_node)->id_name;
}

static bool declare_identifier(interpreter_handle interpreter, node_t *declaration_root) {
    declaration_node *declaration_payload = (declaration_node *)declaration_root->actual_node;
    node_t *specifier = declaration_payload->left_child;
    node_t *rhs = declaration_payload->right_child;
    node_t *declarator = NULL;
    const char *id_name;
    char *id_copy;
    identifier_t *slot;

    if (specifier == NULL || specifier->type != TYPE_SPECIFIER_NODE) {
        interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                              "Declaration is missing a type specifier");
        return false;
    }

    if (rhs == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                              "Declaration is missing a declarator");
        return false;
    }

    if (rhs->type == DECLARATOR_NODE) {
        declarator = rhs;
    } else if (rhs->type == ASSIGN_OP_NODE) {
        declarator = ((assign_op_node *)rhs->actual_node)->left_child;
    }

    id_name = extract_identifier_name(declarator);
    if (id_name == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                              "Declaration target must be an identifier");
        return false;
    }

    if (lookup(interpreter, id_name) != NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                              "Identifier '%s' is already declared", id_name);
        return false;
    }

    if (!ensure_context_capacity(interpreter)) {
        return false;
    }

    id_copy = duplicate_string(interpreter, id_name);
    if (id_copy == NULL) {
        return false;
    }

    slot = &interpreter->context[interpreter->count++];
    slot->id_name = id_copy;

    if (!initialize_identifier_literal(slot,
            ((type_specifier_node *)specifier->actual_node)->subtype)) {
        interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                              "Unsupported declaration type");
        return false;
    }

    return true;
}

static literal_t *evaluate_ast(interpreter_handle interpreter, node_t *node) {
    literal_t *left;
    literal_t *right;

    if (interpreter->status != TCCI_STATUS_OK) {
        return NULL;
    }

    if (node == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                              "Cannot evaluate NULL AST node");
        return NULL;
    }

    if (node->type == COMPOUND_STATEMENT_NODE) {
        compound_statement_node *compound = (compound_statement_node *)node->actual_node;
        literal_t *last = NULL;
        int i;

        for (i = 0; i < compound->children_count; i++) {
            literal_t *current = evaluate_ast(interpreter, compound->children[i]);
            if (current == NULL) {
                free(last);
                return NULL;
            }

            free(last);
            last = current;
        }

        if (last == NULL) {
            return new_int_literal(interpreter, 0);
        }

        return last;
    }

    if (node->type == DECLARATION_NODE) {
        declaration_node *declaration_payload = (declaration_node *)node->actual_node;

        if (!declare_identifier(interpreter, node)) {
            return NULL;
        }

        if (declaration_payload->right_child != NULL &&
            declaration_payload->right_child->type == ASSIGN_OP_NODE) {
            return evaluate_ast(interpreter, declaration_payload->right_child);
        }

        {
            const char *id_name = extract_identifier_name(declaration_payload->right_child);
            identifier_t *identifier = lookup(interpreter, id_name);
            if (identifier == NULL) {
                interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                                      "Failed to initialize declared identifier '%s'", id_name);
                return NULL;
            }
            return copy_literal(interpreter, &identifier->literal);
        }
    }

    if (node->type == ASSIGN_OP_NODE) {
        assign_op_node *assign_payload = (assign_op_node *)node->actual_node;
        const char *id_name = extract_identifier_name(assign_payload->left_child);
        identifier_t *identifier;

        if (id_name == NULL) {
            interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                                  "Assignment target must be an identifier");
            return NULL;
        }

        identifier = lookup(interpreter, id_name);
        if (identifier == NULL) {
            interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                                  "Assignment to undeclared identifier '%s'", id_name);
            return NULL;
        }

        right = evaluate_ast(interpreter, assign_payload->right_child);
        if (right == NULL) {
            return NULL;
        }

        assign_identifier_literal(identifier, right);
        free(right);

        return copy_literal(interpreter, &identifier->literal);
    }

    if (node->type == IDENTIFIER_NODE || node->type == DECLARATOR_NODE) {
        const char *id_name = extract_identifier_name(node);
        identifier_t *identifier = lookup(interpreter, id_name);

        if (identifier == NULL) {
            interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                                  "Use of undeclared identifier '%s'", id_name);
            return NULL;
        }

        return copy_literal(interpreter, &identifier->literal);
    }

    if (node->type == CONSTANT_NODE) {
        constant_node *constant = (constant_node *)node->actual_node;
        if (constant->subtype == INTEGER_CONST) {
            return new_int_literal(interpreter, constant->value.int_value);
        }
        return new_float_literal(interpreter, constant->value.float_value);
    }

    if (node->type == BINARY_OP_NODE) {
        binary_op_node *binary = (binary_op_node *)node->actual_node;

        left = evaluate_ast(interpreter, binary->left_child);
        if (left == NULL) {
            return NULL;
        }

        right = evaluate_ast(interpreter, binary->right_child);
        if (right == NULL) {
            free(left);
            return NULL;
        }

        if (binary->subtype == DIVIDE_OPERATOR && literal_as_float(right) == 0.0f) {
            free(left);
            free(right);
            interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                                  "Division by zero");
            return NULL;
        }

        if (left->type == INTEGER_CONST && right->type == INTEGER_CONST &&
            binary->subtype != DIVIDE_OPERATOR) {
            int result = left->value.int_value;

            if (binary->subtype == ADD_OPERATOR) {
                result += right->value.int_value;
            } else if (binary->subtype == SUBTRACT_OPERATOR) {
                result -= right->value.int_value;
            } else if (binary->subtype == MULTIPLY_OPERATOR) {
                result *= right->value.int_value;
            } else {
                free(left);
                free(right);
                interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                                      "Unsupported integer operator");
                return NULL;
            }

            free(left);
            free(right);
            return new_int_literal(interpreter, result);
        }

        {
            float lhs = literal_as_float(left);
            float rhs = literal_as_float(right);
            float result = 0.0f;

            if (binary->subtype == ADD_OPERATOR) {
                result = lhs + rhs;
            } else if (binary->subtype == SUBTRACT_OPERATOR) {
                result = lhs - rhs;
            } else if (binary->subtype == MULTIPLY_OPERATOR) {
                result = lhs * rhs;
            } else if (binary->subtype == DIVIDE_OPERATOR) {
                result = lhs / rhs;
            } else {
                free(left);
                free(right);
                interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                                      "Unsupported float operator");
                return NULL;
            }

            free(left);
            free(right);
            return new_float_literal(interpreter, result);
        }
    }

    interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                          "Unsupported AST node type: %d", (int)node->type);
    return NULL;
}

literal_t *interpret(interpreter_handle interpreter) {
    if (interpreter == NULL) {
        return NULL;
    }

    interpreter->status = TCCI_STATUS_OK;
    interpreter->error_message[0] = '\0';

    if (interpreter->root == NULL) {
        interpreter_set_error(interpreter, TCCI_STATUS_RUNTIME_ERROR,
                              "Interpreter root is NULL");
        return NULL;
    }

    return evaluate_ast(interpreter, interpreter->root);
}
