# Lyte Project Overview

Lyte is an AOT-compiled systems programming language. The compiler is written in C and follows a standard multi-phase pipeline.

## Compiler Pipeline

```
Source (.lyt) → Lexer → Parser → AST → [Resolver → Linker → Scope → Types] → Codegen → LLVM IR
                         ▲                              ▲                          ▲
                     frontend/                       middle/                   backend/
```

1. **Lexer** (`compiler/frontend/lexer/`) — Tokenises source files into the keyword set defined by the language spec. Comments use `#`.
2. **Parser** (`compiler/frontend/parser/`) — Recursive descent parser producing the AST. Handles `entry` declarations, `fn` declarations, and the minimalist grammar (no `for`, no `try/catch`).
3. **AST** (`compiler/frontend/ast/`) — AST node definitions and tree utilities.
4. **Resolver** (`compiler/middle/resolver/`) — *(Planned)* Validates the acyclic dependency graph.
5. **Linker** (`compiler/middle/linker/`) — *(Planned)* Merges `attach`ed file ASTs into their parent file's AST.
6. **Scope** (`compiler/middle/scope/`) — *(Planned)* Enforces Tier 1/2 boundaries (`struct` and `fn` stay private).
7. **Types** (`compiler/middle/types/`) — *(Planned)* Type checking, interface contract validation, and fat pointer generation.
8. **Codegen** (`compiler/backend/codegen/`) — Emits LLVM IR by walking the AST.

## Shared Components
- **`compiler/common.h`** — Token enum shared across all compiler phases.
- **`compiler/utils/`** — Hash table (`ht.c`) and error reporting (`error.c`).
- **`compiler/vfs/`** — *(Planned)* Virtual file system for `collection` and `attach` support.

## Testing
The project uses a custom unit test framework (`test/framework.h`). Tests are registered in `test/main.c` and cover the lexer, parser, AST, hash table, and pointer parsing.

```bash
cd build && cmake .. && make && ./unit_test
```
