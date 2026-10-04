# Functions

[Specification index](design.md)

## Parameters

Parameters require `let` or `var` and a type; functions require a result type.
A `take` parameter consumes its argument.

```lyte
fn inspect(let point: &Point): f32 {
    return point.x;
}

fn shift(let point: &mut Point, let amount: f32): void {
    point.x += amount;
}

fn relay(take let value: Buffer): Buffer {
    return take(value);
}
```

`let` and `var` control the parameter binding. Reference types control access
to the target.

| Parameter           | Argument      | Contract                      |
| ------------------- | ------------- | ----------------------------- |
| `let value: T`      | `value`       | Copy into read-only local     |
| `var value: T`      | `value`       | Copy into writable local      |
| `let value: &T`     | `&value`      | Shared borrow                 |
| `let value: &mut T` | `&mut value`  | Exclusive writable borrow     |
| `take let value: T` | `take(value)` | Transfer into read-only owner |
| `take var value: T` | `take(value)` | Transfer into writable owner  |

`var` reference parameters allow rebinding without changing target permissions.
Changing a copied parameter affects only the local copy; changing the caller's
value requires `&mut T`.

Non-reference, non-Copy parameters require `take`, including unconstrained generics.
References borrow; `take` cannot consume them or transfer their targets.

A new owner has one destination and needs no `take` at the call site:

```lyte
store(buffers.create());
store(take(buffer));
```

See [evaluation order](control-flow.md#evaluation-order),
[returns](control-flow.md#returns), and
[returned references](memory.md#returned-references).
See [matching](control-flow.md#match) for consuming or borrowing sum payloads.

## Unsafe functions

Calling `unsafe fn` requires an explicit unsafe block and satisfaction of its
documented caller requirements. Its body is safe by default. A safe function
may use unsafe operations internally if it meets their requirements for every
accepted input. See [safety contracts](safety.md#function-contracts-and-module-invariants).
