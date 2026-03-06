# tcci

Toy C compiler/interpreter project for learning compiler fundamentals.

## Current language subset

- Arithmetic expressions: `+`, `-`, `*`, `/`, parentheses
- Declarations: `int x;`, `float y;`
- Assignments: `x = 3;`, `y = x + 1.5;`
- Compound programs with semicolon-terminated statements

## Build and test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run REPL

```bash
./build/repl
```

Type `exit` to quit.

## Runtime and ownership model

- `lexer_init` / `lexer_free`: lexer owns its input buffer.
- `get_next_token` allocates tokens; caller owns tokens and frees with `free_tokens`.
- `parser_init` borrows caller-provided tokens; `free_parser` does not free tokens.
- `parse` returns an AST root; caller frees it with `free_node`.
- `interpreter_init` borrows AST root; caller frees interpreter with `interpreter_free`.
- Lexer/parser/interpreter expose structured status and error message APIs.

## References

- [chibicc](https://github.com/rui314/chibicc)
- [KAIST CS420](https://github.com/kaist-cp/cs420)
