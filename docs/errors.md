# Error handling

[Specification index](design.md)

Recoverable errors use [Result](standard-library.md#result). There are no
implicit error conversions or error-propagation operators.

## Handling results

Every produced Result carries a handling obligation:

- Every normal path must handle it or forward the obligation.
- Binding, copying, or storing it inside another value does not handle it.
- An Err arm must act on the error, report it, or forward it through Result.
  Empty arms and unused payload bindings fail compilation.
- Ignoring, discarding, wildcard matching, and `drop` cannot bypass this rule.
  Replacement and automatic cleanup cannot discard an unchecked Result.
- Nested Results still need handling inside Options, records, arrays, and
  error payloads, even after a match handles the outer value.
- Shared inspection does not count as handling. Use a consuming match or
  a by-value match of a Copy Result.
- A copy of a checked Copy Result is already checked. Handling one unchecked
  copy leaves other live copies unchecked.

The compiler checks handling on each path, not whether recovery or reporting is correct.

```lyte
import std.result { Result, Ok, Err };

fn relay(take let result: Result<Buffer, Failure>): Result<Buffer, Failure> {
    match take(result) {
        Ok(let buffer) => { return Ok(take(buffer)); }
        Err(let error) => { return Err(take(error)); }
    }
}
```

Buffer and Failure are non-Copy owners. Each arm forwards its payload in a
new Result, which the caller must handle.

## Traps and allocation failure

Runtime traps and allocation failures abort. They do not produce Result,
unwind the stack, or promise cleanup. There is no exception handling.
See [runtime traps](safety.md#runtime-traps).
