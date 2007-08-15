open Wht

type vector_size    = int
type vector_stride  = simple_expr list

type vector_code =
  | VLoad      of register * variable * vector_stride * vector_size
  | VStore     of register * variable * vector_stride * vector_size
  | VAdd       of register * register * register * vector_size
  | VSub       of register * register * register * vector_size
  | VAddSub    of register * register * register * vector_size
  | VSubAdd    of register * register * register * vector_size
  | VDecl      of variable * vector_size

val vectorize_by: code list -> int -> vector_code list
val vector_wht_to_string : string -> vector_code list -> string
