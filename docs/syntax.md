# Syntax and bindings

[Specification index](design.md)

## Source files

Source files use `.lt`. Identifiers are case-sensitive ASCII letters, digits,
and underscores, with no leading digit. [Text literals](types.md#text-literals)
use UTF-8: double quotes for strings, single quotes for one character.

Braces delimit blocks. Simple statements, imports, aliases, module declarations,
and module constants end with semicolons. Braced declarations and export lists
need no semicolon. Commas separate arguments and fields. Multiline lists may
have a trailing comma.

`#` outside a literal starts a line comment. Use `#` on each line for multiline
comments. There are no block comments or preprocessor directives. The formatter
supplies one standard layout.

```lyte
module app.main;

fn main(): s32 {
    return 0;
}
```

## Keywords

```text
module export import as transparent
type fn ref new
let var mut take from
if else for match return break continue
unsafe
true false
```

Keywords are reserved. `Copy` is a built-in type constraint, not a keyword.
`safe` is an ordinary identifier.

## Bindings

```lyte
var count: usize = 0;
let limit = read_limit();

count += 1; # correct
limit = 20; # wrong
```

| Binding | Reassign | Modify owned data | Borrow owned data for writing   |
| ------- | -------- | ----------------- | ------------------------------- |
| `let`   | No       | No                | No                              |
| `var`   | Yes      | Yes               | Yes, subject to borrow checking |

For references, `let` or `var` controls reassignment. The reference type
controls access to the target:

| Declaration          | Reassign | Write to target |
| -------------------- | -------- | --------------- |
| `let reader: &T`     | No       | No              |
| `var reader: &T`     | Yes      | No              |
| `let writer: &mut T` | No       | Yes             |
| `var writer: &mut T` | Yes      | Yes             |

`mut` appears only in reference types and borrow expressions.
`let mut` and `mut Buffer` are invalid syntax.

Values must be initialized before use; there is no implicit zero or default.
Signatures, fields, and module constants require types. Local types may be inferred.

Both `let` and `var` can transfer ownership with `take`. The receiving binding
controls access, so `let` does not permanently freeze an allocation.
A `var` binding cannot upgrade a shared reference.

## Constant expressions

Local initializers may run at runtime. Module constants use typed `let`
declarations with compile-time initializers. Mutable module globals are excluded.

The constant evaluator accepts:

- Scalar and text literals, earlier module constants, and inline Copy arrays,
  records, and sums.
- Primitive arithmetic, comparisons, boolean operations, field access, and
  array indexing.
- The checked `convert<T>` primitive.

Constant expressions exclude heap allocation, other calls, pointers, references,
and mutable storage. Sum constructors count as aggregate construction. Cyclic
dependencies and invalid operations fail compilation. Evaluation uses target
numeric widths and checked arithmetic.

Array dimensions use the same evaluator. `if` is a statement and cannot
appear in a constant expression.

See [control flow](control-flow.md) for evaluation order, loops, returns,
and matching. See [unsafe blocks](safety.md#unsafe-blocks) for block permissions.
