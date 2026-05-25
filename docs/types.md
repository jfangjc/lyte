# Primitive Types

| Lyte   | C Equivalent | Description                      |
|--------|--------------|----------------------------------|
| `s8`   | `int8_t`     | Signed 8-bit integer             |
| `s16`  | `int16_t`    | Signed 16-bit integer            |
| `s32`  | `int32_t`    | Signed 32-bit integer            |
| `s64`  | `int64_t`    | Signed 64-bit integer            |
| `u8`   | `uint8_t`    | Unsigned 8-bit integer           |
| `u16`  | `uint16_t`   | Unsigned 16-bit integer          |
| `u32`  | `uint32_t`   | Unsigned 32-bit integer          |
| `u64`  | `uint64_t`   | Unsigned 64-bit integer          |
| `f32`  | `float`      | 32-bit IEEE 754 floating point   |
| `f64`  | `double`     | 64-bit IEEE 754 floating point   |
| `f128` | `long double`| 128-bit IEEE 754 floating point  |
| `ssize`| `ssize_t`    | Signed pointer-sized integer     |
| `usize`| `size_t`     | Unsigned pointer-sized integer   |

# Pointer Types

Bindings are declared with `var` for mutable values or `const` for immutable values.

| Syntax | Meaning                                          |
|--------|--------------------------------------------------|
| `T`    | Stack-allocated value of type `T`                |
| `*T`   | Raw pointer to a value of type `T`               |
| `&x`   | Address-of — retrieves the memory address of `x` |
| `*x`   | Dereference — accesses the value at a pointer    |
