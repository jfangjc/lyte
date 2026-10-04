# Ownership and borrowing

[Specification index](design.md)

## Ownership and copying

Each allocation has one owner responsible for release. Locals, fields, elements,
and consuming parameters may own values; references only grant access.

Integers, floats, booleans, raw pointers, shared references, and aggregates of
Copy values are Copy. Shared-reference copies keep the same borrow restrictions.
Heap owners, aggregates containing owners, and exclusive references are non-Copy.
Copy depends on the type's contents. Users cannot define copy hooks.

Named owners require `take` in assignments, calls, returns, and aggregate
construction:

```lyte
var first = buffers.create();
let second = take(first);
# first is unavailable.
```

`take` transfers a whole local and makes it unavailable. Only `var` locals may
be reinitialized. At control-flow joins, a value must be initialized and
unconsumed on every path.

Moves and clones are never implicit, even at last use. Duplication requires a
library call that documents allocation and failure.

## Heap storage

```lyte
type Node<T> = heap {
    value: T,
    next: Option<Node<T>>,
}

var node = new Node {
    value: 10,
    next: None,
};
```

`Node<T>` owns a heap allocation; moving the handle leaves the allocation in
place. `new` allocates and infers generic arguments from fields when unambiguous.
Allocation failure aborts without recovery or rollback.

Safe owners cannot form cycles. Graphs may use arenas with non-owning IDs or
raw pointers behind checked APIs. Returning safe references requires checking
ID validity and access permissions.

## Borrow expressions

Borrowing does not allocate, copy the target, or transfer ownership.

| Expression   | Permission                    |
| ------------ | ----------------------------- |
| `&value`     | Shared read access by default |
| `&mut value` | Exclusive write access        |

An explicit local type determines a direct `&value` initializer's borrow mode:

```lyte
var point = Point { x: 0.0, y: 0.0 };
let reader: &Point = &point;
let previous_x = reader.x;
# reader has no later use.
let editor: &mut Point = &point;
editor.x = previous_x + 1.0;
```

Otherwise, `&value` is shared. Calls require explicit `&mut value`; parameter
types cannot supply that mode. Annotations cannot upgrade existing shared references.

Safe references are non-null. Fields and indexing operate through them.
`*reference` accesses a scalar. `&mut T` requires writable source access and
no conflicting loan, regardless of whether the binding is `let` or `var`.

Rebinding a `var` reference changes its target, but the old borrow lasts as long
as references derived from it need it.

References may appear in locals, parameters, and read-only results, but not in
fields, collections, or globals. They cannot target references or return write
access. Direct `Option<&T>` is allowed in locals and results; `Option<&mut T>` is excluded.

## Loan conflicts

A loan reserves access for a borrow. Shared loans may overlap; exclusive loans
conflict with any overlapping active loan. Loans end at their last possible use,
including copies, derived references, and uses on other control-flow paths.

The checker tracks storage roots, fields, and indices. A whole-owner borrow
covers all owned data, including nested owners. Heap storage and calls may
require borrowing the whole root. Different names do not prove disjoint storage.

```lyte
swap(&mut left, &mut right);            # Distinct locals.
swap(&mut pair.left, &mut pair.right);  # Disjoint inline fields.
swap(&mut left, &mut left);             # Error.
swap(&mut items[i], &mut items[j]);     # Error without proof of disjointness.
```

## Reborrowing

Direct initialization from a shared reference copies it. Direct initialization
from an exclusive reference creates a child loan of the same target:

```lyte
let writer = &mut buffer;
let second = writer;
modify(&mut second);
# second's loan ends.
modify(&mut writer);
```

The child borrows the target and suspends its parent while active. An explicit
`&T` annotation creates a shared child. Assignment into a mutable reference
binding follows the same rule; self-assignment fails compilation.

`&mut parent` explicitly reborrows an exclusive target. `&parent` can create
a shared child from either permission. A shared child blocks parent writes.
There are no permanent reference moves or reference-to-reference values.
A shared result derived from an exclusive input keeps the exclusive loan
active until the result's loan ends.

## Returned references

Only shared references may be returned. `from` names their source:

```lyte
fn identity(let point: &Point): &Point from point {
    return point;
}
```

The compiler checks the body against the exported contract used by callers.
Exactly one eligible borrowed input allows inferred `from`; several require
an explicit clause.

`from (left, right)` allows either source and keeps both loans active for the
result's lifetime. The result cannot outlive either. Locals and consuming
parameters cannot supply returned references.

The same rules cover shared slices and `Option<&T>`.
`&Option<T>` instead borrows the optional storage.
There are no named lifetime parameters or writable result syntax.

## Replacing storage

Individual fields cannot be moved out. Consume a whole record by destructuring
and binding every field:

```lyte
var { value, next } = take(node);
```

For heap records, this transfers fields and frees the allocation. Unwanted
owned fields still need cleanup.

`extract` leaves `None`. `exchange` installs a replacement and returns the old
value. Both require exclusive access.
See [standard-library operations](standard-library.md#memory-operations).

Assignment evaluates the right-hand side, releases the old owner, and installs
the replacement. Replacement and extraction cannot invalidate live references
or discard an unchecked Result.

## Cleanup

Normal exits, including `return`, `break`, and `continue`, release initialized
owners that have not been moved. Inner scopes clean up first. Locals release
in reverse declaration order, fields in declaration order, and arrays in
index order.

`drop(take(owner))` releases early only when no live reference needs the storage.
Scope exit cannot leave usable dangling references. Lifetime checks need no
runtime loan tracking.

Abort does not unwind or promise cleanup. External resources need explicit
closing as documented by their APIs; there are no custom destructors or
automatic external-resource cleanup.
