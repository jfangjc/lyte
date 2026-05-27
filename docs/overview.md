# Lyte Project Overview
Lyte is an AOT-compiled systems programming language.

## Compiler Pipeline

```
Source (.lt) → Lexer → Parser → AST → [Resolver → Linker → Scope → Types] → Codegen → LLVM IR
                         ▲                              ▲                          ▲
                     frontend/                       middle/                   backend/
```

1. **Source** (`compiler/frontend/source/`) — Group files by module and other pre lexer code
1. **Lexer** (`compiler/frontend/lexer/`) — Tokenises source files into the keyword set defined by the language spec.
1. **Parser** (`compiler/frontend/parser/`) — Recursive descent parser producing the AST.
1. **AST** (`compiler/frontend/ast/`) — AST node definitions and tree utilities.
1. **Resolver** (`compiler/middle/resolver/`) — Validates the acyclic dependency graph.
1. **Linker** (`compiler/middle/linker/`) — Merges `attach`ed file ASTs into their parent file's AST.
1. **Scope** (`compiler/middle/scope/`) — Enforces Tier 1/2 boundaries (`struct` and `fn` stay private).
1. **Types** (`compiler/middle/types/`) — Type checking, interface contract validation, and fat pointer generation.
1. **Codegen** (`compiler/backend/codegen/`) — Emits LLVM IR by walking the AST.

## Shared Components
- **`compiler/common.h`** — Token enum shared across all compiler phases.
- **`compiler/utils/`** — Hash table (`ht.c`) and error reporting (`error.c`).

## Testing
The project uses a basic custom unit test framework (`test/framework.h`). Tests are registered in `test/main.c` and cover the lexer, parser, AST, hash table, and pointer parsing.
