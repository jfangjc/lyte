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
| [ownership.lt](ownership.lt)                    | Owning `ref` records, `new`, single-source and multiple-source `from` contracts |
| [linked_list.lt](linked_list.lt)                | Recursive `ref` nodes, `take`, borrowed matching, `Option<&Node>` from an owner |

Each file with `main` is a separate program. The imports example also needs
`structs_and_modules.lt`, which declares its `geometry.circle` dependency.
The circle module has no entry function.

The pointer example calls the checked-reference function. Its raw-pointer helper
documents caller requirements and uses an explicit unsafe block. `&value` creates
a checked reference, so it cannot initialize a raw pointer.

The linked-list example builds `10 -> 20 -> None`. The head owns the whole chain;
`next` returns a borrowed node tied to its input by `from node`. Matching
`&node.next` borrows the payload, so traversal does not consume the list. Releasing
the head releases the chain. Safe owning pointers cannot form cycles.

The intended return values are 0 for hello and variables, 182 for control flow,
7 for imports, 10 for pointers and ownership, and 30 for linked list.
