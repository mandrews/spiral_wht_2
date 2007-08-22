type simple_expr =
  | Temporary of int
  | Variable  of string
  | Constant  of int
  | Times     of simple_expr * simple_expr
  | Plus      of simple_expr * simple_expr

type variable   = simple_expr
type stride     = simple_expr
type register   = simple_expr

type code =
  | Load      of register * variable * stride
  | Store     of register * variable * stride
  | Add       of register * register * register 
  | Sub       of register * register * register 
  | Decl      of variable

val wht : int -> code list
val wht_to_string : string -> code list -> string

val make_tmp : unit -> int

val expr_to_string : simple_expr -> string

val wht_type   : string
val wht_header : string -> string
val wht_declarations : string -> string
val wht_footer : string
