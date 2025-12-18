```
program             ->  ( macro_decl | func_decl )*

// declarations
const_decl          -> "let" identifier ( ":" type )? "=" initialiser ";"
var_decl            -> "let" "mut" identifier ( ":" type )? "=" initialiser ";"
func_decl           -> "fn" identifier para_list ":" type compound_stmt
macro_decl          -> "define" identifier "(" (identifier ("," identifier)*)? ")" .*
addr_decl           -> "addr" identifier ":" type (":=" initialiser)? ";"

para_list           -> "(" (para_decl ( "," para_decl )*)? ")"
para_decl           -> identifier ":" ( "mut" )? type

initialiser         -> expr
                    | "{" expr ( "," expr )* "}"

// primitive types
type                -> s8 | s16 | s32 | s64 | u8 | u16 | u32 | u64

// identifiers
identifier          -> ID

// statements
compound_stmt       -> "{" (var_decl | addr_decl | stmt)* "}"

stmt                -> if_stmt
                    |  for_stmt
                    |  break_stmt
                    |  continue_stmt
                    |  return_stmt
                    |  expr_stmt

if_stmt             -> if "(" boolean_expr ")" compound_stmt ( else if "(" boolean_expr ")" compound_stmt )+ ( else compound_stmt )?
for_stmt            -> for "(" boolean_expr ")" compound_stmt
break_stmt          -> break ";"
continue_stmt       -> continue ";"
return_stmt         -> return expr? ";"

expr_stmt           -> expr? ";"

// expressions
expr                -> assign_expr
                    | call_expr
                    | var_decl
                    | addr_decl

assign_expr     -> ( identifier "=" )* expr
                    | identifier += expr
                    | identifier -= expr
                    | identifier *= expr
                    | identifier /= expr

boolean_expr        -> equality_expr
                    | !boolean_expr
                    | "(" boolean_expr ")"
                    |  boolean_expr "||" boolean_expr
                    |  boolean_expr "&&" boolean_expr

equality_expr        -> arith_expr "==" arith_expr
                    |  arith_expr "!=" arith_expr
                    |  arith_expr "<" arith_expr
                    |  arith_expr "<=" arith_expr
                    |  arith_expr ">" arith_expr
                    |  arith_expr ">=" arith_expr

arith_expr          -> unary_expr
                    |  unary_expr "+" unary_expr
                    |  unary_expr "-" unary_expr
                    |  unary_expr "*" unary_expr
                    |  unary_expr "/" unary_expr

unary_expr          -> identifier
                    | +unary_expr
                    | -unary_expr
                    | numbers

call_expr           -> identifier "(" ( identifier "," )* identifier? ")"
```
