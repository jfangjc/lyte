- compiler/
    - scanner.c
        - Return the next word with the corresponding token type.
    - parser.c
        - Construct the Abstract Syntax Tree.
    - optimiser.c
        - Merge some statements together to reduce the amount of redundent code generated.
    - common.h
    - emitter.c
    - gen.c
    - ht.c
    - error.c


# Scanner
Read in the whole file
-> only get the next token when needed
-> use advance (peek) to not need to copy the string

# Function
- Function consist 4 parts:
    - Function type
    - Function name
    - Function arguments
    - Function contents

`fn for (n : s8): s8 {}`
`fn loo (i : u16): u16 {}`

# Variables
- Variables are immutable by default using `let`.
- Mutable variables are declared using `let mut`.
    - `let x : i32 = 10;` (Immutable)
    - `let mut y : i32 = 20;` (Mutable)

# Macros
- Macros are defined using the C-style `define` directive.
- They are processed before compilation.
    - `define MAX(a, b) ((a) > (b) ? (a) : (b))`
