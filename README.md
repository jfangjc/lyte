<p align="center">
    <img src="assets/logo.png" alt="Lyte logo" style="width:65%">
    <br/>
    Uncompromised Hardware Control. Strictly Enforced Modularity.
    <br/>
    <br/>
</p>

---

# The Lyte Programming Language

Lyte is a statically typed, ahead-of-time (AOT) compiled systems language. It uses function-first behavior, module isolation, explicit exports, and explicit unsafe blocks.

```lyte
module app.main

fn main(): s32 {
    return 0;
}
```

> **The Lyte compiler is still very early in development.**

## Key Principles

- **Function-first behavior**: behavior is defined with `fn`, not methods or classes.
- **Module isolation**: modules define namespace and privacy boundaries.
- **Explicit exports**: public module contracts are listed in `export`.
- **Minimal keyword set**: syntax vocabulary is small and stable.
- **Unsafe is explicit**: low-level operations require `unsafe`.

## Project Structure

```text
lyte/
|-- cli/                    # CLI entry point and command-line compilation flow
|-- compiler/               # Core compiler library
|   |-- common.h            # Shared token and type definitions
|   |-- frontend/           # Parsing pipeline
|   |   |-- source/         # Pre-lexer
|   |   |-- lexer/          # Tokeniser
|   |   |-- parser/         # Parser and AST node definitions
|   |   `-- module/         # Module AST merging and export validation
|   |-- middle/             # Semantic analysis
|   |   |-- resolver/       # Dependency graph validation
|   |   |-- linker/         # Module linking and export resolution
|   |   |-- scope/          # Module privacy validation
|   |   `-- types/          # Type checking
|   |-- backend/            # Code generation
|   |   `-- codegen/        # Emits LLVM IR from the AST
|   `-- utils/              # Shared utilities
|-- docs/                   # Language specification and documentation
`-- test/                   # Unit test suite
```

## Documentation

#### [Getting Started](docs/get-started.md)
Instructions for building the compiler and running tests.

#### [Language Specification](docs/design.md)
The Lyte language specification, module model, type model, and keyword set.

#### [Types Reference](docs/types.md)
Built-in type names and pointer syntax.

#### [Project Overview](docs/overview.md)
Compiler architecture and project structure details.

## Warnings

**The Lyte compiler is still very early in development.**
