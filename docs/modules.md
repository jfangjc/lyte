# Modules and imports

[Specification index](design.md)

## Module declarations

Every file starts with an explicit module declaration:

```lyte
module geometry.point;

export { Point, length_squared }
```

Files with the same module name share a namespace. Duplicate declarations and
dependency cycles fail compilation. Directories do not declare modules.

Modules contain records, sums, and free functions. Qualified calls name module
functions. There are no classes, methods, implicit receivers, inheritance, or
type-based function lookup.

Modules do not grant unsafe permission. `unsafe module` is invalid syntax.
Each function uses its own [unsafe blocks and caller contracts](safety.md#unsafe-blocks).

## Exports

Declarations are private unless named in the module's single export list.
Exported records are opaque: clients cannot construct, inspect, or destructure
them unless the representation is exported explicitly:

```lyte
export { transparent Point, length_squared }
```

Exporting a sum's type does not expose constructors. Export each constructor
clients may construct or match. Exhaustive matching requires all constructors.

```lyte
export { State, Idle, Running }
```

Opaque types remain usable in function signatures even when clients cannot
inspect their representation.

## Imports

```lyte
import geometry.point as point;
import std.option { Option, Some, None };
import std.result { Result, Ok, Err };
import std.memory { extract, exchange, drop };
import std.numeric { convert };
```

Alias imports allow qualified access, such as `point.length_squared(&position)`.
Named imports bind only the listed exports. Constructors and patterns may use
qualified names such as `option.Some`.

Imports are file-local and do not include the imported module's imports.
Duplicate bindings and conflicts with local declarations fail compilation;
aliases resolve them. There are no glob imports or implicit parent lookup.

[Standard-library names](standard-library.md) require explicit imports, with no
implicit prelude. Built-in types and syntax need no import.

## Module organization

Types, modules, and files need not match one to one.

Use opaque exports to protect invariants and transparent exports for direct field
access. Avoid mandatory getters, setters, and records that only hold functions.
Pass application state explicitly.

Keep sequential operations together. Extract functions for reuse or complex
logic; use blocks to limit local state and borrows.
