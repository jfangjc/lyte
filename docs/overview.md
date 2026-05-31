# Lyte Project Overview
Lyte is an AOT-compiled systems programming language.

## Compiler Pipeline

```text
Source (.lt) -> Lexer -> Parser AST -> [Resolver -> Linker -> Scope -> Types] -> Codegen -> LLVM IR
                         ^                              ^                          ^
                     frontend/                       middle/                   backend/
```

1. **Source** (`compiler/frontend/source/`) - Groups files by module before lexing.
1. **Lexer** (`compiler/frontend/lexer/`) - Tokenises source files into the keyword set defined by the language spec.
1. **Parser** (`compiler/frontend/parser/`) - Recursive descent parser and AST node definitions.
1. **Module** (`compiler/frontend/module/`) - Merges parsed file ASTs into module ASTs and validates export contracts.
1. **Resolver** (`compiler/middle/resolver/`) - Validates the acyclic dependency graph.
1. **Linker** (`compiler/middle/linker/`) - Resolves imports and exported module contracts.
1. **Scope** (`compiler/middle/scope/`) - Enforces module privacy boundaries.
1. **Types** (`compiler/middle/types/`) - Type checking and contract validation.
1. **Codegen** (`compiler/backend/codegen/`) - Emits LLVM IR by walking the AST.

## Shared Components

- **`compiler/common.h`** - Token enum shared across all compiler phases.
- **`compiler/utils/`** - Shared helpers such as hash table, file loading, and error reporting.

## Testing

The project uses a basic custom unit test framework (`test/framework.h`). Tests are registered in `test/main.c` and cover the lexer, parser, hash table, and pointer parsing.
