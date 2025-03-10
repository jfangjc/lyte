- arch/
    - evaluator.c
        - Covert the code to the equivlant asm code
- compiler/
    - scanner.c
        - Return the next word with the corresponding token type.
    - parser.c
        - Construct the Abstract Syntax Tree.
    - optimiser.c
        - Merge some statements together to reduce the amount of redundent code generated.
    - token.h


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

for (int n : ns) {}
loo (int i : 10) {}
