<p align="center">
    <img src="assets/logo.png" alt="Lyte logo" style="width:65%">
    <br/>
    Uncompromised Hardware Control. Strictly Enforced Modularity.
    <br/>
    <br/>
</p>

---

# The Lyte Programming Language
Lyte is a statically typed, ahead-of-time (AOT) compiled systems programming language. It delivers uncompromised hardware control paired with a strictly enforced modular architecture, using a unique 3-Tier Architecture (`fn` / `module` / `interface`) to enforce Dependency Inversion and prohibit state-bleeding across file boundaries.

```
entry main {
    # every Lyte program starts here
}
```

> **The Lyte compiler is still very early in development.**

## Key Principles
- **No classes** — state and behavior are strictly decoupled
- **Composition over inheritance** — complex types are built by embedding structs
- **Explicit memory control** — no garbage collector, no borrow checker
- **Minimalist control flow** — `while` is the only loop construct
- **Flat dependency graph** — no header files, no circular imports
- **Explicit entry point** — programs begin with an `entry` declaration, not a magic `main` function

## Project Structure
```
lyte/
├── cli/                    # CLI entry point — parses flags and orchestrates the build
├── compiler/               # Core compiler library
│   ├── common.h            # Shared token and type definitions
│   ├── frontend/           # Parsing pipeline
│   │   ├── lexer/          # Tokeniser — converts source text into tokens
│   │   ├── parser/         # Parser — transforms tokens into the AST
│   │   └── ast/            # AST node definitions and utilities
│   ├── middle/             # Semantic analysis (planned)
│   │   ├── resolver/       # Dependency graph validation (acyclic enforcement)
│   │   ├── linker/         # Merges attached file ASTs into their parent
│   │   ├── scope/          # Tier 1/2 boundary validation
│   │   └── types/          # Type checking and interface contract validation
│   ├── backend/            # Code generation
│   │   └── codegen/        # Emits LLVM IR from the AST
│   ├── utils/              # Shared utilities (hash table, error handling)
│   └── vfs/                # Virtual file system (planned — collection/attach support)
├── docs/                   # Language specification and documentation
└── test/                   # Unit test suite with custom test framework
```

## Documentation

#### [Getting Started](docs/get-started.md)
Instructions for building the compiler and running tests.

### Learn Lyte

#### [Language Specification](docs/design.md)
The complete Lyte language specification — type system, 3-Tier Architecture, modularity rules, and formal EBNF grammar.

#### [Types Reference](docs/types.md)
Primitive type table with C equivalents.

#### [Project Overview](docs/overview.md)
Compiler architecture and project structure details.

#### [FAQ](TODO)
Frequently Asked Questions about Lyte.

## Warnings
**The Lyte compiler is still very early in development.**