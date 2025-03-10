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

# Types
lyte    | C equivalent | Description
--------|--------------|-------------------------------
    si8 |       int8_t |    signed 8-bit integer
    ui8 |      uint8_t |  unsigned 8-bit integer
   si16 |      int16_t |   signed 16-bit integer
   ui16 |     uint16_t | unsigned 16-bit integer
   si32 |      int32_t |   signed 32-bit integer
   ui32 |     uint32_t | unsigned 32-bit integer
   si64 |      int64_t |   signed 64-bit integer
   ui64 |     uint64_t | unsigned 64-bit integer
