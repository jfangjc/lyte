<h1 align="center">Lyte</h1>
<h3 align="center">Yet another programming language that no one use</h3>

# What do I want to achieve?
- Support passing function address as a type
- Most 'unsafe' programming language
- Functional programming lanaguage
- Minimum optimization (opt in optimization)
- High performance
- Mid-level language
- Static typed language
- As less keywords as possible
- Allow inline assembly to be written inside a function with type asm
- Don't allow non-terminating function

# Basic architecture
Scanner -> Linked list of tokens -> Parser -> AST -> Evalutaor -> CASM (Common assembly) -> Translator -> Machine specific assembly

WYWIWYG (What you write is what you get)