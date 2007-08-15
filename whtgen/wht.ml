open Printf
open Utils

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


let wht_type = "wht_value"

let make_tmp =
  let tmp_count = ref 0
    in fun () -> begin
      tmp_count := !tmp_count + 1;
      !tmp_count
    end

let rec range a b =
  if a > b then []
  else a :: range (a + 1) b 
  ;;

let wht size =
  let hash = Hashtbl.create size in

  let rec loads n =
    if n == 0 then []
    else 
      let t = make_tmp() 
      and m = size - n in
        (Hashtbl.add hash m t);
         Load  ((Temporary t),(Variable "x"),Times (Constant m, Variable "S")) :: loads (n - 1)

  and stores n =
    if n == 0 then []
    else 
      let m = size - n in
      let t = (Hashtbl.find hash m) in
         Store ((Temporary t),(Variable "x"),Times (Constant m, Variable "S")) :: stores (n - 1)

  and unroll_wht size offset stride =
    if size == 2 then
      let t1 = (Hashtbl.find hash offset)
      and t2 = (Hashtbl.find hash (offset+stride))
      and t3 = make_tmp()
      and t4 = make_tmp() in
        (Hashtbl.add hash offset t3);
        (Hashtbl.add hash (offset+stride) t4);
        [ 
          Add ((Temporary t3), (Temporary t1), (Temporary t2));
          Sub ((Temporary t4), (Temporary t1), (Temporary t2)) 
        ]
    else
      let m = size/2 in
        let a = (unroll_wht m offset stride) @ (unroll_wht m (offset+m) stride) 
        and b = List.map (fun i -> unroll_wht 2 i m) (range offset (offset+m-1)) in
          a @ (List.flatten b)
  in 
  let header = (loads size) in 
  let body   = (unroll_wht size 0 1) in
  let footer = (stores size) in
    header @ body @ footer
  ;;

let rec simplify_expr x =
  match x with
  | Times (a,(Constant 1))  -> simplify_expr a
  | Times ((Constant 1),b)  -> simplify_expr b
  | Times (a,(Constant 0))  -> Constant 0
  | Times ((Constant 0),b)  -> Constant 0
  | Plus  (a,(Constant 0))  -> simplify_expr a
  | Plus  ((Constant 0),b)  -> simplify_expr b
  | Constant  a   -> Constant  a
  | Variable  a   -> Variable  a
  | Temporary a   -> Temporary a
  | Times (a,b) -> Times (simplify_expr a, simplify_expr b)
  | Plus  (a,b) -> Plus  (simplify_expr a, simplify_expr b)

let rec expr_to_string x =
  match (simplify_expr x) with 
  | Times (a,b)  -> (expr_to_string a) ^ "*" ^ (expr_to_string b)
  | Plus  (a,b)  -> (expr_to_string a) ^ "+" ^ (expr_to_string b)
  | Constant a   -> sprintf "%d" a 
  | Temporary a  -> sprintf "t%d" a
  | Variable a   -> a

let code_to_string x =
  let es = expr_to_string in
  match x with
  | Load  (reg,var,expr)   -> 
    sprintf "  wht_load(%s, %s[%s]);\n"  (es reg) (es var) (es expr)
  | Store (reg,var,expr)   -> 
    sprintf "  wht_store(%s[%s], %s);\n" (es var) (es expr) (es reg)
  | Add   (reg0,reg1,reg2) -> 
    sprintf "  wht_add(%s,%s,%s);\n" (es reg0) (es reg1) (es reg2) 
  | Sub   (reg0,reg1,reg2) -> 
    sprintf "  wht_sub(%s,%s,%s);\n" (es reg0) (es reg1) (es reg2) 
  | Decl  (Temporary t) ->
    sprintf "  %s t%d;\n"  wht_type t
  | Decl  (Variable  v) ->
    sprintf "  %s *%s;\n"  wht_type v
  | Decl  (_) -> failwith "unsupported declaration"

let add_decls wht =
  let rec _add_decls w =
    match w with
    | (Load (r,(Variable "x"),_) | Store (r,(Variable "x"),_)) :: xs -> 
      Decl r :: _add_decls xs
    | (Load (r,v,_) | Store (r,v,_)) :: xs -> 
      Decl r :: Decl v :: _add_decls xs
    | (Add  (r,a,b) | Sub (r,a,b)) :: xs -> 
      Decl r :: Decl a :: Decl b :: _add_decls xs
    | x :: xs -> _add_decls xs
    | [] -> []
  in (unique (_add_decls wht)) @ wht

let wht_header name =
  "#include \"spiral_wht.h\"\n\n" ^
  "void apply_" ^ name ^ "(Wht *W, long S, long D, wht_value *x)\n{\n"

let wht_declarations wht_type =
  let max = make_tmp () in
  let rec declare_tmp n =
    match n with
    | 0 -> ""
    | i -> declare_tmp (i-1) ^ (sprintf "  %s t%d;\n" wht_type i) 
  in declare_tmp (max - 1)

let wht_footer = "}\n"

let wht_to_string name wht =
  let rec code_list_to_string w =
    match w with 
    | x :: xs -> (code_to_string x) ^ (code_list_to_string xs)
    | [] -> ""
  in  (wht_header name) ^ 
      (code_list_to_string (add_decls wht)) ^ 
      (wht_footer)
 ;;

