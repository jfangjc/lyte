# Lyte Project Overview
Lyte is a toy compiler written in C.

## Project Structure
lyte/
├── cli/                  # 1. The Driver
│   └── main              # Orchestrates the build process and parses flags
│
├── compiler/             # 2. The Core API (Designed as a library)
│   ├── vfs/              # Virtual File System (Crucial for Fragments)
│   │   └── workspace     # Loads collections, tracks parent-fragment relationships
│   │
│   ├── frontend/         # Phase A: Parsing the Minimalist Syntax
│   │   ├── lexer/        # Tokenizer (handles the EBNF keywords)
│   │   ├── parser/       # Transforms tokens into the AST
│   │   └── ast/          # Abstract Syntax Tree nodes (very small due to no for-loops/try-catch)
│   │
│   ├── middle/           # Phase B: The Enforcer (Lyte's heaviest phase)
│   │   ├── resolver/     # 1. Graph checking (Enforces the acyclic dependency rule)
│   │   ├── linker/       # 2. Merges `fragment` ASTs into their parent file's AST
│   │   ├── scope/        # 3. Validates Tier 1/2 boundaries (ensures `struct` stays private)
│   │   └── types/        # 4. Type Checker (Validates raw pointers, interface contracts)
│   │
│   └── backend/          # Phase C: AOT Code Generation
│       ├── ir/           # Lowers Lyte AST to an Intermediate Representation
│       ├── vtable/       # Dynamically generates the VTables for Interface Fat Pointers
│       └── codegen/      # Emits the final LLVM IR or C code
│
├── lsp/                  # 3. Language Server (For IDE support)
│   └── server            # Hooks into `compiler/` for strict boundary autocomplete
│
├── std/                  # 4. Lyte's Standard Library
│   ├── core/             # Fundamental memory types, pointers, and string layouts
│   └── collections/      # E.g., `collection Std;`
│
├── tools/                # 5. Tooling
│   └── formatter/        # Code formatter (Lytefmt)
│
└── tests/                # 6. Test Suite
    ├── ui/               # Tests compiler errors (e.g., circular import errors)
    └── codegen/          # Validates that Fat Pointers and memory layout compile correctly

## Design
The compiler follows a standard pipeline:
1. **Scanner**: Converts source code into tokens.
2. **Parser**: Converts tokens into an AST.
3. **Code Generator**: Traverses the AST and generates LLVM IR.

## Testing
The project uses a custom unit test framework defined in `test/framework.h` and `test/framework.c`. Tests are automatically registered using constructor attributes.
