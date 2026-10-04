# Lyte

Lyte is a statically typed systems language with records, sums, free functions,
explicit modules, unique owners, and checked borrowing.

- `let` fixes a binding; `var` allows reassignment and mutation of owned data.
- `&T` borrows for reading; `&mut T` borrows for writing; `take(value)` transfers ownership.
- `match` handles sum types, including `Option` and `Result`. Every Result must be handled or forwarded.
- Blocks are safe by default. Unsafe operations require an explicit `unsafe` block.
- Fixed arrays, borrowed slices, and UTF-8 text are supported by the design.
  Standard-library names require explicit `std.*` imports. Comments start with `#`.

```lyte
module app.main;

fn main(): s32 {
    return 0;
}
```

The compiler implements an earlier subset without ownership or borrow checking.
This example and the specification describe the target language.

## Getting started

You need CMake 3.10+, GCC or Clang, and Make or Ninja.
With Nix flakes enabled, get these tools with:

```bash
nix develop
```

Build from the repository root:

```bash
cmake -S . -B build
cmake --build build
```

Run the compiler with one or more `.lt` source files:

```bash
./build/lyte path/to/main.lt
```

Each file must declare its module. Output is textual LLVM IR in `.s` files,
not a complete native executable.

Run the unit tests:

```bash
./build/unit_test
```

## Documentation

- [Specification index](docs/design.md)
- [Compiler overview](#compiler-overview)
- [Language examples](examples/README.md)

## Compiler overview

```text
.lt files -> module grouping -> lexer -> parser -> module merge
          -> export-name validation -> textual LLVM IR
```

[`cli/compile.c`](cli/compile.c) runs this pipeline.
[`compiler/common.h`](compiler/common.h) defines shared tokens.
The test suite covers the lexer, parser, module/export parsing, pointers, hash
tables, and character literal code generation.

Name resolution, type checking, definite initialization, ownership analysis,
and borrow checking are still missing. They need a typed control-flow
representation that preserves storage locations, binding and reference
permissions, consuming modes, `from` contracts, and explicit block safety.
There is no `compiler/middle/` implementation yet.

Code generation still needs complete control flow, target-aware types, checked
arithmetic and indexing, and cleanup. The heap runtime also needs implementation.
Parser tests alone do not establish memory safety. Add tests for accepted and
rejected programs, emitted IR, and generated program execution.
The [examples](examples/README.md) are not all supported yet.

## Repository

| Directory                   | Contents                                                   |
| --------------------------- | ---------------------------------------------------------- |
| `cli/`                      | Compiler command-line entry point and compilation pipeline |
| `compiler/frontend/source/` | Groups input files by explicit module declaration          |
| `compiler/frontend/lexer/`  | Produces tokens                                            |
| `compiler/frontend/parser/` | Builds the current syntax tree                             |
| `compiler/frontend/module/` | Merges declarations and validates export names             |
| `compiler/backend/codegen/` | Emits early LLVM IR                                        |
| `compiler/utils/`           | File, error, and hash-table helpers                        |
| `test/`                     | Compiler unit tests                                        |
| `examples/`                 | Examples for the current language specification            |
| `docs/`                     | Language specification                                     |
