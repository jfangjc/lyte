# Types and collections

[Specification index](design.md)

## Built-in types

| Type             | Meaning                                              |
| ---------------- | ---------------------------------------------------- |
| `s8 s16 s32 s64` | Signed integers                                      |
| `u8 u16 u32 u64` | Unsigned integers                                    |
| `ssize usize`    | Signed and unsigned integers of pointer width        |
| `f32 f64`        | IEEE 754 floating-point values                       |
| `bool`           | `true` or `false`                                    |
| `char`           | One Unicode scalar value                             |
| `void`           | No function result                                   |
| `&T`             | Shared checked reference                             |
| `&mut T`         | Exclusive checked reference                          |
| `*T`             | Raw pointer without lifetime or ownership guarantees |
| `[T; N]`         | Inline fixed array                                   |
| `&[T]`           | Shared borrowed slice                                |
| `&mut [T]`       | Writable borrowed slice                              |

Numeric literals use the expected type, defaulting to `s32` for integers and
`f64` for floats. Out-of-range literals fail compilation. Numeric conversions
must use [checked conversion](standard-library.md#numeric-conversions).

## Records, sums, and aliases

### Records

A record is a distinct type with named fields.

```lyte
type Point = {
    x: f32,
    y: f32,
}

let point = Point { x: 1.0, y: 2.0 };
```

Ordinary records live inline in local, field, or element storage.
[Heap records](memory.md#heap-storage) hold owning handles instead.

### Sums

A sum is a distinct type holding one named variant, optionally with a payload.
Variant names construct values:

```lyte
type State =
    | Idle
    | Running(usize);

let stopped: State = Idle;
let active: State = Running(3);
```

Use [match](control-flow.md#match) to select a variant and access its payload.
Variants with the same payload type remain distinct. Payloads follow the
[ownership and Copy rules](memory.md#ownership-and-copying).
Clients need exported constructors to construct or match variants.
See [exports](modules.md#exports).

### Aliases

An alias names an existing type:

```lyte
type UserId = u64;
```

Two aliases of `u64` are interchangeable. Use a record such as
`type UserId = { value: u64 }` for a distinct identifier type.

### Inline size

Inline records, sums, and arrays must have finite size. Recursive structures
need indirection, such as an owning [heap handle](memory.md#heap-storage).

## Generics

Generics use angle brackets. An unconstrained `T` may own data.
Code that copies it must require `T: Copy`.
See [copying](memory.md#ownership-and-copying) and [library sums](standard-library.md#option).

## Fixed arrays

```lyte
var values: [s32; 3] = [10, 20, 30];
let zeros: [s32; 4] = [0; 4];
values[1] = 25;
```

`N` is a compile-time `usize`. Arrays hold initialized elements inline and are
Copy exactly when their elements are Copy. Elements cannot contain safe
references, including in fields.

Array literals evaluate left to right. Named owning elements require `take`.
`[value; N]` evaluates `value` once and requires a Copy value. Empty `[]` needs an
expected element type and has length zero.

Indices have type `usize`. Out-of-bounds access traps.
Indexing cannot move an owning element out. Use `exchange` to replace it.

## Borrowed slices

A slice holds a data address and a `usize` length without owning or allocating
elements. Reference permission, loan, and Copy rules apply. Elements cannot
contain safe references.

Local slices and parameters are allowed. Only shared slices may be returned,
with [from contracts](memory.md#returned-references).

Each line below is a separate borrowing example:

```lyte
let prefix: &[s32] = &values[0..2];
let editor: &mut [s32] = &values[0..2];
```

Ranges include `start` and exclude `end`. Bounds must satisfy
`start <= end <= length`; otherwise, the operation traps.
Whole-array borrowing produces a slice when an explicit local type or parameter
expects one. Calls still require `&mut values` for writable slices.
Slice indices are checked against length.

The first borrow checker proves distinct constant indices, but not runtime
indices. It may also prove disjoint constant ranges. Safe slicing is always checked.

## Text literals

Double-quoted text has type `std.text.Literal<N>`, where `N` is its UTF-8 byte length.
`"hello"` is `Literal<5>`. Literals are inline Copy values and do not allocate.
Source text must be valid UTF-8. Supported escapes are `\n`, `\r`, `\t`, `\0`,
`\\`, and `\"`.

Single-quoted `char` literals contain one Unicode scalar value, such as `'a'`,
`'é'`, or `'😀'`. They support `\n`, `\r`, `\t`, `\0`, `\\`, `\'`, and `\"`.
Empty literals, multiple scalars, invalid UTF-8, and raw line breaks are errors.
An `e` followed by a combining accent is two scalars and is rejected.

The compiler assigns text literal types using the standard-library declaration
without importing its name into scope. User-defined integer generic parameters
are not supported.

See [UTF-8 text](standard-library.md#utf-8-text) for owning strings and text functions.
