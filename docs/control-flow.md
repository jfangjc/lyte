# Control flow

[Specification index](design.md)

## Evaluation order

Expressions and arguments evaluate left to right. Argument borrows start
immediately and stay active through later arguments and the call. Read needed
values before borrowing for writing.

## Conditions

Conditions must be `bool`. Options and results do not convert to booleans.
`if` and `else` use braced bodies.

## Loops

```lyte
for var i: usize = 0; i < count; i += 1 {
    process(i);
}
```

`break` exits the nearest loop. `continue` starts its next iteration and runs
the `for` step. Each path back to the loop must leave required owners initialized.

## Returns

`return value;` exits the function, including from nested and unsafe blocks.
Only `void` permits `return;` or reaching the closing brace without a return.
Every reachable non-void exit must return the declared type.
There is no inline `use` expression with a different return target.

## Match

`match` is a statement for any sum type with known variants, including generic
sums, Option, and Result. It evaluates the value once and runs one matching
arm. Arms do not fall through or need `break`.

```lyte
type Event =
    | Connected(u64)
    | Disconnected(u64)
    | Tick;

match event {
    Connected(let id) => { connected(id); }
    Disconnected(let id) => { disconnected(id); }
    Tick => {}
}
```

Variant names select arms, even for variants with the same payload type.
Payloads bind with `let` or `var`; payload-free variants have no binding.
Qualified names follow [import rules](modules.md#imports).

Missing or duplicate arms fail compilation. Payload bindings exist only in
their arm. `return` exits the function; `break` and `continue` target the nearest loop.

### Match modes

The selected value determines how the arm accesses its payload:

| Selected value | Payload binding            |
| -------------- | -------------------------- |
| Copy value     | Copied value               |
| `take(value)`  | Owned payload              |
| `&value`       | Shared `&T` payload        |
| `&mut value`   | Exclusive `&mut T` payload |

```lyte
match take(head) {
    None => { return None; }
    Some(var node) => {
        var { value, next } = take(node);
        list.head = take(next);
        return Some(take(value));
    }
}
```

Owning a payload requires consuming the whole sum. Borrowed matches cannot
transfer ownership. In borrowed patterns, `var` permits rebinding the payload
reference without changing target access.

```lyte
var optional: Option<s32> = Some(1);
match &mut optional {
    Some(let value) => { *value += 1; }
    None => {}
}
```

While a payload borrow is active, you cannot replace the parent sum or
change its variant. See [Result handling](errors.md#handling-results).
