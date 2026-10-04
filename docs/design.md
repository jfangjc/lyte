# Lyte language specification

These pages define Lyte's first borrow-checked version. The compiler implements
an earlier subset. Examples show intended behavior and may omit modules and
imports. `# error` marks rejected code.

## Language rules

Read in order, or use the rule index below.

| Topic                                | Rules                                                                        |
| ------------------------------------ | ---------------------------------------------------------------------------- |
| [Syntax and bindings](syntax.md)     | Source files, keywords, `let` and `var`, initialization, constants           |
| [Types and collections](types.md)    | Numbers, records, sums, aliases, generics, arrays, slices, literals          |
| [Modules and imports](modules.md)    | Namespaces, opaque records, sum constructors, exports, imports               |
| [Functions](functions.md)            | Parameter bindings, borrowing, consuming calls                               |
| [Control flow](control-flow.md)      | Evaluation order, conditions, loops, returns, sum matching                   |
| [Ownership and borrowing](memory.md) | Copy, `take`, heap storage, loans, reborrowing, returned references, cleanup |
| [Error handling](errors.md)          | Mandatory Result handling, forwarding, nested Results                        |
| [Safety](safety.md)                  | Safe-by-default blocks, `unsafe`, runtime traps, compiler guarantees         |

## Standard library and version scope

Standard-library names require explicit `std.*` imports. Selected declarations
have compiler-checked rules.

| Topic                                                 | Rules                                                              |
| ----------------------------------------------------- | ------------------------------------------------------------------ |
| [Standard library](standard-library.md)               | Modules, declarations, and operation contracts                     |
| [Version limits and implementation status](limits.md) | Excluded features, undefined rules, runtime costs, compiler status |

## Find a rule

| Syntax or concept                                   | Reference                                                              |
| --------------------------------------------------- | ---------------------------------------------------------------------- |
| `let`, `var`, initialization                        | [Bindings](syntax.md#bindings)                                         |
| Compile-time `let`                                  | [Constant expressions](syntax.md#constant-expressions)                 |
| Records, sums, aliases                              | [Type declarations](types.md#records-sums-and-aliases)                 |
| `import`, `export`, `transparent`                   | [Modules and imports](modules.md)                                      |
| Parameter modes and consuming calls                 | [Parameters](functions.md#parameters)                                  |
| Evaluation order                                    | [Evaluation order](control-flow.md#evaluation-order)                   |
| `if`, `for`, `return`, `break`, `continue`          | [Control flow](control-flow.md)                                        |
| `match`, sum payloads                               | [Matching](control-flow.md#match)                                      |
| `&T`, `&mut T`, inferred borrows                    | [Borrow expressions](memory.md#borrow-expressions)                     |
| Reference aliasing and last use                     | [Loan conflicts](memory.md#loan-conflicts)                             |
| `second = writer`                                   | [Reborrowing](memory.md#reborrowing)                                   |
| `from`, lifetime relationships                      | [Returned references](memory.md#returned-references)                   |
| `take`, `Copy`                                      | [Ownership](memory.md#ownership-and-copying)                           |
| `heap`, `new`                                       | [Heap storage](memory.md#heap-storage)                                 |
| `extract`, `exchange`, `drop`                       | [Memory operations](standard-library.md#memory-operations)             |
| `Option`, `Some`, `None`                            | [Option](standard-library.md#option)                                   |
| `Result`, `Ok`, `Err`                               | [Result](standard-library.md#result)                                   |
| Result obligations                                  | [Error handling](errors.md#handling-results)                           |
| `convert<T>`                                        | [Numeric conversions](standard-library.md#numeric-conversions)         |
| Text literals                                       | [Literal types](types.md#text-literals)                                |
| `String` and text functions                         | [UTF-8 text](standard-library.md#utf-8-text)                           |
| `unsafe`, `*T`, block permissions                   | [Unsafe blocks](safety.md#unsafe-blocks)                               |
| `unsafe fn`, caller requirements, module invariants | [Safety contracts](safety.md#function-contracts-and-module-invariants) |
| Runtime failures                                    | [Runtime traps](safety.md#runtime-traps)                               |
| Unsupported features                                | [Version limits](limits.md#excluded-features)                          |

See the README for the [compiler overview](../README.md#compiler-overview)
and [build instructions](../README.md#getting-started).
