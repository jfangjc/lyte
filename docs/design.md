# Lyte Language Specification

## 1. Abstract
Lyte is a statically typed, ahead-of-time (AOT) compiled systems programming language. It is designed to provide the absolute, uncompromised hardware control of C, paired with a strictly enforced modular architecture. Lyte utilizes a unique 3-Tier Architecture (`fn` / `module` / `interface`) to enforce Dependency Inversion and strictly prohibit state-bleeding across file boundaries, while leaving memory management and error handling entirely in the hands of the developer.

## 2. Core Paradigms & OOP Integration
Lyte implements **Data-Oriented, Composition-First OOP** without the overhead or fragility of traditional class hierarchies.
- **No Classes:** State and behavior are strictly decoupled.
- **Interface Inheritance:** An `interface` may extend another `interface` to build complex contracts.
- **Composition over Inheritance:** Complex data structures are built exclusively by embedding structs within structs.
- **Explicit Control:** There is no garbage collector and no implicit borrow checker. Developers have total authority over the memory lifecycle.

## 3. Type System & Memory Model
Lyte expects the developer to manage memory allocations and lifetimes explicitly. 

### 3.1 Primitive Types
- **Integers:** `s8`, `s16`, `s32`, `s64`, `u8`, `u16`, `u32`, `u64`, `ssize`, `usize`
- **Floating Point:** `f32`, `f64`, `f128`
- The types below may not exist
    - **Booleans:** `bool` (true/false)
    - **Strings:** `string`

### 3.2 Pointers and Memory Control
Variables are declared using `let` (mutable bindings) or `const` (compile-time constants). Lyte relies on traditional C-style raw pointers for referencing memory.
- **`T` (Value Type):** A stack-allocated value.
- **`*T` (Raw Pointer):** A pointer to a type `T`, which can reside on the stack or the heap.
- **Address-of (`&`):** Retrieves the memory address of a variable.
- **Dereference (`*`):** Accesses or modifies the underlying value at a given memory address.

### 3.3 Dynamic Types (Fat Pointers)
When an `interface` is used as a type (e.g., `*Logger`), the compiler dynamically generates a **Fat Pointer**. This struct consists of two components:
1.  A pointer to the backing data (`struct`).
2.  A pointer to the Virtual Method Table (VTable) of the implementing `module`, enabling dynamic dispatch at runtime.

## 4. The 3-Tier Architecture
Every Lyte file enforces a strict separation of concerns to guarantee true modularity.

### Tier 1: Private Internals (`fn`, `struct`)
- Data structures (`struct`) and helper functions (`fn`) are strictly private. They cannot be prefixed with `export`.
- They are uniquely bound to the file they are defined in, or shared exclusively with associated `parent`.

### Tier 2: The Exposed Component (`module`)
- The `module` block binds private internal data to public behavior.
- A module must explicitly declare what it implements using the `implements` keyword.
- Modules expose instantiation logic via `static fn` factories.

### Tier 3: The Abstract Blueprint (`interface`)
- Interfaces declare function signatures without implementation logic.
- They act as the primary, safe cross-boundary communication type between isolated files.

## 5. Modularity & The File System
Lyte utilizes a flat, acyclic dependency graph, entirely eliminating the need for C-style header files or complex namespace nesting.

### 5.1 Collections
Files optionally belong to a logical group declared via `collection <Name>;` at the top of the file. This acts as an organizational tag for the package manager and linker.

### 5.2 Strict Imports
Dependencies are imported explicitly: `import { Target } from "path/to/file";`. The compiler strictly enforces an Acyclic Dependency Graph. Circular imports yield an immediate, unrecoverable compilation error.

### 5.3 Scope Attachments
To prevent monolithic file bloat without compromising Tier 1 encapsulation, Lyte allows a single logical scope to be distributed across multiple physical files. This is achieved through explicit structural attachment rather than traditional module imports.
- **The Parent File:** Declares `attach "filename.lyt";`. This instructs the compiler to mechanically merge the target file directly into the parent's Tier 1 private scope.
- **The Attached File:** Must explicitly declare its structural owner at the top of the file using `parent "parent_file.lyt";`.
- **Visibility & Constraints:** Attached files share the exact same private scope (private `fn` and `struct` declarations) as their parent file. Because they are strictly an extension of the parent's AST, attached files are entirely invisible to the rest of the compilation unit and **cannot** be targeted by an `import` statement.
    
## 6. Control Flow & Error Handling
Lyte values a minimalist, highly predictable Abstract Syntax Tree (AST). 

### 6.1 Looping
To reduce compiler complexity and enforce standard coding patterns, Lyte features exactly one loop construct: the `while` loop. Constructs such as `for`, `do-while`, and `loop` do not exist in the language grammar.

### 6.2 Error Handling
Lyte does not possess built-in exception handling (`try/catch`) or compiler-enforced error types (`Result<T, E>`). Error handling is entirely user-defined.

## 7. Formal Syntax Grammar (EBNF)

```ebnf
<Program>          ::= <CollectionDecl>? <ParentDecl>? <ImportDecl>* <AttachDecl>* <Statement>*

<CollectionDecl>   ::= "collection" <Identifier> ";"
<ParentDecl>       ::= "parent" <StringLiteral> ";"
<AttachDecl>       ::= "attach" <StringLiteral> ";"
<ImportDecl>       ::= "import" "{" <IdentifierList> "}" "from" <StringLiteral> ";"

<Statement>        ::= <VarDecl> | <StructDecl> | <InterfaceDecl> | <ModuleDecl> | <PrivateFnDecl> 
                     | <WhileLoop> | <IfStatement> | <ReturnStmt> | <ExpressionStmt>

<VarDecl>          ::= ("let" | "const") <Identifier> ":" <Type> ("=" <Expression>)? ";"

/* Memory & Pointers */
<Type>             ::= "i8" | "i16" | "i32" | "i64" | "u8" | "u16" | "u32" | "u64" 
                     | "f32" | "f64" | "bool" | "string" | "void" | <Identifier> | "*" <Type>
<Expression>       ::= <AddressOfExpr> | <DerefExpr> | <BinaryExpr> | <FunctionCall> | <Literal> | <Identifier>
<AddressOfExpr>    ::= "&" <Identifier>
<DerefExpr>        ::= "*" <Identifier>

/* Minimalist Control Flow */
<WhileLoop>        ::= "while" "(" <Expression> ")" <Block>
<IfStatement>      ::= "if" "(" <Expression> ")" <Block> ("else" <Block>)?
<ReturnStmt>       ::= "return" <Expression>? ";"

<StructDecl>       ::= "struct" <Identifier> "{" <StructFields> "}"
<StructFields>     ::= (<Identifier> ":" <Type> ",")+ | <Empty>

<InterfaceDecl>    ::= "export"? "interface" <Identifier> ("extends" <IdentifierList>)? "{" <Signatures> "}"
<Signatures>       ::= (<Identifier> "(" <ParamList> ")" ":" <Type> ";")*

<ModuleDecl>       ::= "export" "module" <Identifier> ("implements" <IdentifierList>)? "{" <ModuleMethods> "}"
<ModuleMethods>    ::= ("static"? "fn" <Identifier> "(" <ParamList> ")" ":" <Type> <Block>)*

<PrivateFnDecl>    ::= "fn" <Identifier> "(" <ParamList> ")" ":" <Type> <Block>

<ParamList>        ::= (<Param> ("," <Param>)*)?
<Param>            ::= <Identifier> ":" <Type>

<Block>            ::= "{" <Statement>* "}"
```
