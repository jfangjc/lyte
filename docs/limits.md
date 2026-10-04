# Version limits and implementation status

[Specification index](design.md)

## Excluded features

The first version excludes:

- Safe references in fields, collections, or globals. Direct `Option<&T>`
  remains allowed in locals and results.
- Writable reference results, escaping closures, and shared-memory concurrency.
- Classes, methods, inheritance, and implicit receivers.
- User-defined copy, assignment, conversion, and operator hooks.
- Custom destructors and automatic cleanup of external resources.
- General integer generic parameters, caller-selected allocators, and
  recoverable allocation failure.

## Rules not defined yet

Stable record and sum layouts, package management, and the complete build model
are undefined. Matching defines direct variant arms; nested patterns, guards,
and catch-all syntax are not defined yet.

## Lifetimes and runtime costs

Lifetime constraints still apply without named lifetime syntax. General cyclic
ownership needs an arena or checked unsafe API.

Borrow checks have no runtime loan tracking. Allocation, cleanup, bounds checks,
and arithmetic checks may still run. Function signatures do not restrict
internal allocation. This specification makes no performance comparisons.

## Implementation status

The [compiler overview](../README.md#compiler-overview) tracks the earlier
subset currently implemented.
