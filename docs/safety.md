# Safety

[Specification index](design.md)

## Runtime traps

These failures trap in every build mode:

- Integer overflow, division by zero, and signed minimum divided by minus one.
- Invalid shift counts and failed numeric conversions.
- Out-of-bounds indices and invalid slice ranges.

Named wrapping operations provide modular arithmetic. Floating-point operations
follow their documented IEEE format.

Traps and allocation failures abort without unwinding or promised cleanup.
Recoverable errors use [Result](errors.md).

## Unsafe blocks

Every block is safe by default. Unsafe permission applies only to an explicit
`unsafe { ... }` block inside a function.

| Form             | Rule                                                   |
| ---------------- | ------------------------------------------------------ |
| `unsafe { ... }` | Permits unsafe operations in that block                |
| `unsafe fn`      | Declares safety requirements that callers must satisfy |
| `unsafe module`  | Invalid syntax                                         |

Permission does not extend to nested blocks, `if` or `for` bodies, or `match`
arms. Each needs its own unsafe block for unsafe operations. An unsafe block
may cover a whole function implementation, but even an `unsafe fn` body starts
safe. There is no `safe` keyword or special safe-block syntax.

```lyte
# source must be live, aligned, initialized, readable, and free of
# conflicting writes during this call.
unsafe fn read_raw(let source: *s32): s32 {
    unsafe {
        return *source;
    }
}
```

Unsafe calls also need an unsafe block inside an unsafe function. Declarations
do not inherit permission from their surroundings.

```lyte
unsafe {
    let value = *source;       # allowed in this unsafe block
    {
        let copy = value;      # ordinary nested block is safe
        let raw = *source;     # error: requires an unsafe block here
    }
}
```

This example assumes `source` is a valid raw pointer to a Copy value.

Unsafe operations include raw dereference, pointer arithmetic, raw allocation
and release, unchecked indexing, validity-sensitive representation casts,
raw-to-safe construction, and unverifiable foreign calls.

Raw addresses use an explicit library operation with an access contract.
`&value` creates a safe borrow, not a raw pointer.

Unsafe code still passes type, initialization, and safe-reference checks.
The programmer must establish allocation bounds, alignment, initialization,
lifetime, representation validity, access permissions, and allocator compatibility.
Comments and unchecked assertions do not prove pointer validity.

## Function contracts and module invariants

Use `unsafe fn` for documented caller guarantees the compiler cannot check.
A safe wrapper must satisfy internal unsafe operations' requirements for every
input its signature accepts.

Calls, visibility, exports, imports, and module membership never grant unsafe permission.

Other module functions must preserve invariants relied on by unsafe operations,
even when those functions contain no unsafe code. Reviewing correctness may
therefore require more code than the unsafe block itself.

## Safety contract

A correct implementation prevents uninitialized reads, use after free, double
release, invalid typed values, and out-of-bounds access in safe code. This depends
on correct unsafe libraries, foreign contracts, compiler, and runtime.

Borrow checking precedes code generation. The typed representation must retain
binding and reference permissions, consuming modes, storage paths, block safety,
and `from` relationships. Unsupported constructs must be rejected.

Diagnostics identify the original borrow, conflict, and later use. Suggested
fixes must preserve behavior; the compiler never inserts clones or unsafe
operations to hide conflicts. Human and generated code share formatting and
diagnostic rules. Exported API descriptions include permissions, ownership
modes, result types, and `from` contracts.

See [version limits](limits.md) for unsupported features and implementation status.
