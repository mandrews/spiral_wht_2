open Wht
open Interleave
open Vectorize

open Printf

let usage = "Usage: " ^ Sys.argv.(0) ^ " -n size"

let size        = ref None
let interleave  = ref None
let vectorize   = ref None
let aligned     = ref false

let opts = [
  "-n", Arg.Int  (fun x -> size       := Some x),
  " size of WHT to generate.";
  "-l", Arg.Int  (fun x -> interleave := Some x),
  " number of loops to interleave by.";
  "-v", Arg.Int  (fun x -> vectorize  := Some x),
  " size of vector codes to use.";
  "-a", Arg.Bool (fun x -> aligned    := true),
  " aligned or strided memory access.";
]

let standard_arg_parse_fail _ = failwith "too many arguments"

let parse opts usage =
  Arg.parse
    opts
    standard_arg_parse_fail
    usage

let check_size s =
  match s with 
  | (Some s) when (s > 0) -> (1 lsl s)
  | other -> failwith "invalid size."

let generate n l v a =
  match (l,v) with
  | (Some l, Some v) -> 
    let name = sprintf "il_%d_v_%d_small%d" l v n in
      (vector_wht_to_string name (vectorize_by (interleave_by (wht n) l) v))
  | (None  , Some v) -> 
    let name = sprintf "v_%d_small%d" v n in
      (vector_wht_to_string name (vectorize_by (wht n) v))
  | (Some l, None  ) -> 
    let name = sprintf "il_%d_small%d" l n in
      wht_to_string name (interleave_by (wht n) l)
  | (None  , None  ) -> 
    let name = sprintf "small%d" n in
      wht_to_string name (wht n)

let main () =
  begin
    parse opts usage;
    print_string (generate (check_size !size) !interleave !vectorize !aligned)
  end

let _ = main ()

