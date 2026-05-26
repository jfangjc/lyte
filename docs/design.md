# Language design

The language is a **function-first, module-isolated systems language**.

The main idea is **Functions define behavior. Modules isolate code. Exports define contracts. Everything else is private.**

# Project model

A project contains source tree and build manifest

The project manifest should describes:
- source directories
- entry function
- compiler options
- formater/linter rule

# Module model

A module is similar to namespace

Example:
```
module app.user
```

Anything `export` is visible outside the module, otherwise private to the module.

There are only two visibility levels:
- exported: visible outside the module
- default: private to the module

**Files have no visibility meaning.**

A module may span many files.

```
src/app/user/mod.lt
src/app/user/find.lt
src/app/user/create.lt
src/app/user/validate.lt
```

Each file says:

``` ts
module app.user
```

All files with the same module name share the same private namespace.

So this is valid:

```
# parse.lt
module app.config

fn parse(bytes: Bytes, out config: Config): Status {
    ...
}
```

```
# load.lt
module app.config

fn load(path: Path, out config: Config): Status {
    var bytes: Bytes

    readBytes(path, out bytes)!
    parse(bytes, out config)!

    return OK
}
```

`load` can call `parse` because both are inside module `app.config`.

# Functions are the only behavior unit

All behavior is written as functions.

```
fn findById(id: UserId, out user: User): Status {
    ...
}
```

# Custom type

Use `type` for all data definitions.

Records:

```
type User = {
    id: UserId,
    email: Email,
    passwordHash: Bytes,
}
```

Sum types:

```
type LoginState =
    | LoggedOut
    | LoggedIn(UserId)
    | Locked(Time)
```

Aliases:

```
type Status = s32
type UserId = u64
```

# Memory model

Safe code is enable to use of reference.

Unsafe function allows explicit memory managment and raw pointers.

Safe public functions may use unsafe internally if they uphold the invariants.

# Minimal keyword set

Keyword set:
``` text
module
export
import
type
fn
const
var
return
if
else
for
continue
break
unsafe
```
