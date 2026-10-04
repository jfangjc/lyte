# Language examples

These examples use the [current specification](../docs/design.md). The compiler
implements an earlier subset and cannot compile all of this syntax yet.

| File                                             | Demonstrates                                                                 |
| ------------------------------------------------ | ---------------------------------------------------------------------------- |
| [hello.lt](hello.lt)                             | Minimal entry function and module declaration                                |
| [variables.lt](variables.lt)                     | Immutable locals, mutable locals, arithmetic, module constants               |
| [control_flow.lt](control_flow.lt)               | Conditions, loops, exhaustive matching of a user-defined sum                 |
| [structs_and_modules.lt](structs_and_modules.lt) | Records, transparent exports, constructors, shared reference parameters      |
| [imports.lt](imports.lt)                         | Module aliases, named standard-library imports, Option matching              |
| [pointers.lt](pointers.lt)                       | Shared and writable references, raw pointers inside an explicit unsafe block |

Each file with `main` is a separate program. The imports example also needs
`structs_and_modules.lt`, which declares its `geometry.circle` dependency.
The circle module has no entry function.

The pointer example calls the checked-reference function. Its raw-pointer helper
documents caller requirements and uses an explicit unsafe block. `&value` creates
a checked reference, so it cannot initialize a raw pointer.

The intended return values are 0 for hello and variables, 182 for control flow,
7 for imports, and 10 for pointers.
