program             ->  ( func_decl )*

// declarations
const_decl          -> "let" identifier ":=" initialiser ";"
func_decl           -> "fn" identifier para_list ":" type compound_stmt
var_decl            -> "var" identifier ":" type (":=" initialiser)? ";"
addr_decl           -> "addr" identifier ":" type (":=" initialiser)? ";"

para_list           -> "(" (para_decl ( "," para_decl )*)? ")"
para_decl           -> declarator ":" type

initialiser         -> expr
                    | "{" expr ( "," expr )* "}"

// primitive types
type                -> i8 | i16 | i32 | i64 | ui8 | ui16 | ui32 | ui64

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

assign_expr     -> ( identifier ":=" )* expr
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
