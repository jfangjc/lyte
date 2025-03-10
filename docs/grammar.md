program             ->  ( func_decl )*

// declarations
func_decl           -> "fn" identifier para_list ":" type compound_stmt
var_decl            -> "var" init_declarator ";"
init_declarator     -> declarator  ":" "->" type ( "=" initialiser )? 
declarator          -> identifier 
                    |  identifier "\[" INTLITERAL? "\]"

initialiser         -> expr 
                    |  "{" expr ( "," expr )* "}"

// primitive types
type                -> i8 | i16 | i32 | i64 | ui8 | ui16 | ui32 | ui64

// identifiers
identifier          -> ID 

// statements 
compound_stmt       -> "{" (var_decl | stmt)* "}" 

stmt                -> if_stmt 
                    |  loop_stmt
                    |  for_stmt
                    |  while_stmt 
                    |  break_stmt
                    |  continue_stmt
                    |  return_stmt
                    |  expr_stmt

if_stmt             -> if "(" expr ")" compound_stmt ( else compound_stmt )?
for_stmt            -> for "(" expr? ";" expr? ";" expr? ")" compound_stmt
while_stmt          -> while "(" expr ")" compound_stmt
break_stmt          -> break ";"
continue_stmt       -> continue ";"
return_stmt         -> return expr? ";"
expr_stmt           -> expr? ";"


// expressions 
expr                -> assignment_expr
assignment_expr     -> ( cond_or_expr "=" )* cond_or_expr
cond_or_expr        -> cond_and_expr 
                    |  cond_or_expr "||" cond_and_expr
cond_and_expr       -> equality_expr 
                    |  cond_and_expr "&&" equality_expr
equality_expr       -> rel_expr
                    |  equality_expr "==" rel_expr
                    |  equality_expr "!=" rel_expr
rel_expr            -> additive_expr
                    |  rel_expr "<" additive_expr
                    |  rel_expr "<=" additive_expr
                    |  rel_expr ">" additive_expr
                    |  rel_expr ">=" additive_expr
additive_expr       -> multiplicative_expr
                    |  additive_expr "+" multiplicative_expr
                    |  additive_expr "-" multiplicative_expr
multiplicative_expr -> unary_expr
                    |  multiplicative_expr "*" unary_expr
                    |  multiplicative_expr "/" unary_expr
unary_expr          -> "+" unary_expr
                    |  "-" unary_expr
                    |  "!" unary_expr
                    |  primary_expr

primary_expr        -> identifier arg_list?
                    | identifier "\[" expr "\]"
                    | "(" expr ")"

// parameters
para_list           -> "(" proper_para_list? ")"
proper_para_list    -> para_decl ( "," para_decl )*
para_decl           -> type declarator
arg_list            -> "(" proper_arg_list? ")"
proper_arg_list     -> arg ( "," arg )*
arg                 -> expr
