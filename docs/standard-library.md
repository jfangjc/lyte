# Standard library

[Specification index](design.md)

Standard-library modules use the `std` namespace and require
[explicit imports](modules.md#imports). There is no implicit prelude.

| Module        | Exports                                    |
| ------------- | ------------------------------------------ |
| `std.option`  | `Option<T>`, `Some`, `None`                |
| `std.result`  | `Result<T, E>`, `Ok`, `Err`                |
| `std.memory`  | `extract`, `exchange`, `drop`              |
| `std.numeric` | `convert<T>`                               |
| `std.text`    | `String`, `Literal<N>`, and text functions |

Only specific standard-library declarations get compiler support; same-named
user declarations do not. Array and slice borrowing need no library import.

## Option

```lyte
import std.option { Option, Some, None };
```

```lyte
type Option<T> =
    | None
    | Some(T);
```

`Some(value)` holds a value, including an empty string or zero. `None` holds
no value. Matches must cover both variants.

Option follows the ownership, Copy, matching, and cleanup rules for sums.
Constructing an Option does not allocate. Named owned payloads require `take`.
There is no `T?` shorthand or error-propagation operator.

Only standard-library `Option<&T>` may hold a reference under the rules in
[borrowing](memory.md#borrow-expressions). Other records and collections cannot
store safe references.

## Result

```lyte
import std.result { Result, Ok, Err };
```

```lyte
type Result<T, E> =
    | Ok(T)
    | Err(E);
```

`Ok(value)` holds success; `Err(error)` holds an error. Sum ownership, Copy,
matching, and cleanup rules apply. Named owners require `take`; construction
does not allocate.

The compiler requires [handling every Result](errors.md#handling-results),
including Results stored inside other values.

## Memory operations

`std.memory` exports these compiler-checked operations:

```lyte
fn extract<T>(let slot: &mut Option<T>): Option<T>
fn exchange<T>(let slot: &mut T, take let replacement: T): T
fn drop<T>(take let value: T): void
```

| Operation  | Effect                                                |
| ---------- | ----------------------------------------------------- |
| `extract`  | Return the previous option and leave None             |
| `exchange` | Install the replacement and return the previous value |
| `drop`     | Perform normal memory cleanup                         |

```lyte
var head = extract(&mut list.head);
let old = exchange(&mut state.buffer, take(replacement));
```

Named owned replacements require `take`. Access, initialization, ownership, and
borrow checks prevent invalid or uninitialized storage from becoming visible.
These operations cannot bypass Result handling or permit other partial moves.
User code cannot define compiler primitives.

## Numeric conversions

```lyte
import std.numeric { convert };
```

`convert<T>(value)` checks numeric conversions. Unsupported conversions and
invalid constant conversions fail compilation; out-of-range runtime values trap.

There are no implicit numeric conversions. See [numeric types](types.md#built-in-types)
and [constant expressions](syntax.md#constant-expressions).

## UTF-8 text

`std.text` exports opaque owning `String` and inline Copy `Literal<N>`.
[Text literals](types.md#text-literals) supply the bytes for creating a String:

```lyte
import std.text as text;

var message: text.String = text.create("hello");
text.append(&mut message, " world");
```

`create` copies literal bytes into heap storage. `append` may grow that storage.
Both accept literals of any compile-time byte length and abort on allocation
failure. There is no implicit literal-to-String conversion.

Text operations preserve UTF-8:

| Operation                    | Contract                                                      |
| ---------------------------- | ------------------------------------------------------------- |
| `text.bytes(&message)`       | Returns `&[u8]` from `message`                                |
| `text.byte_length(&message)` | Returns `usize`                                               |
| Substring operations         | Validate byte boundaries; return Result on invalid boundaries |

Safe code cannot obtain writable String bytes or index a String as characters.
References to String storage block mutation or growth that would conflict
with those references.
