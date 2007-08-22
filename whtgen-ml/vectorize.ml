open Wht
open Utils

open Printf

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


type register_pair = Wht.register * Wht.register

let rec convert wht =
  match wht with
  | [] -> []
  | Load  (r,v,s) :: xs -> VLoad  (r,v,[s],1) :: (convert xs)
  | Store (r,v,s) :: xs -> VStore (r,v,[s],1) :: (convert xs)
  | Add   (r,a,b) :: xs -> VAdd   (r,a,b,1)   :: (convert xs)
  | Sub   (r,a,b) :: xs -> VSub   (r,a,b,1)   :: (convert xs)
  | Decl  (v)     :: xs -> VDecl  (v,1)       :: (convert xs)

let rec expand_strides var strides =
  let es = expr_to_string in
  match strides with
  | [] -> ""
  | x :: [] -> (sprintf "%s[%s]"   (es var) (es x)) 
  | x :: xs -> (expand_strides var [x]) ^ "," ^ (expand_strides var xs)

let vector_code_to_string x =
  let es = expr_to_string in
  match x with
  | VLoad   (r,v,s,1) ->
    sprintf "  wht_load(%s, %s);\n"       (es r) (expand_strides v s)
  | VStore  (r,v,s,1) ->
    sprintf "  wht_store(%s, %s);\n"      (es r) (expand_strides v s)
  | VLoad   (r,v,s,n) ->
    sprintf "  wht_load%d(%s, %s);\n"   n (es r) (expand_strides v s)
  | VStore  (r,v,s,n) ->
    sprintf "  wht_store%d(%s, %s);\n"  n (es r) (expand_strides v s)
  | VAdd    (r,a,b,n) ->
    sprintf "  wht_add%d(%s,%s,%s);\n"  n (es r) (es a) (es b)
  | VSub    (r,a,b,n) ->
    sprintf "  wht_sub%d(%s,%s,%s);\n"  n (es r) (es a) (es b)
  | VAddSub (r,a,b,n) ->
    sprintf "  wht_addsub%d(%s,%s,%s);\n"  n (es r) (es a) (es b)
  | VSubAdd (r,a,b,n) ->
    sprintf "  wht_subadd%d(%s,%s,%s);\n"  n (es r) (es a) (es b)
  | VDecl   (Temporary t,n) ->
    sprintf "  %s%d t%d;\n" wht_type n t
  | VDecl   (Variable  v,_) ->
    sprintf "  %s *%s;\n"   wht_type v
  | VDecl   (_,_) -> failwith "unsupported declaration"

let add_vector_decls wht =
  let rec _add_vector_decls w =
    match w with
    | (VLoad (r,(Variable "x"),_,n) | VStore (r,(Variable "x"),_,n)) :: xs -> 
      VDecl (r,n) :: _add_vector_decls xs
    | (VLoad (r,v,_,n) | VStore (r,v,_,n)) :: xs -> 
      VDecl (r,n) :: VDecl (v,1) :: _add_vector_decls xs
    | (VAdd  (r,a,b,n) | VSub (r,a,b,n) | VAddSub (r,a,b,n) | VSubAdd (r,a,b,n)) :: xs -> 
      VDecl (r,n) :: VDecl (a,n) :: VDecl (b,n) :: _add_vector_decls xs
    | x :: xs -> _add_vector_decls xs
    | [] -> []
  in (unique (_add_vector_decls wht)) @ wht


let vector_wht_to_string name wht =
  let rec vector_code_list_to_string wht =
    match wht with 
    | x :: xs -> (vector_code_to_string x) ^ (vector_code_list_to_string xs)
    | [] -> ""
  in  (wht_header name) ^ 
      (vector_code_list_to_string (add_vector_decls wht)) ^ 
      (wht_footer)

let vectorize wht = 
  let stack = Stack.create () in

  let rec stores wht = 
    match wht with
    | VStore (_,_,_,_) as x :: xs -> x :: stores(xs)
    | x :: xs -> stores(xs)
    | [] -> []
  in

  let rec nonstores wht = 
    match wht with
    | VStore (_,_,_,_) :: xs -> nonstores(xs)
    | x :: xs -> nonstores(xs) @ [x] (* Notice that nonstores are created in reverse *)
    | [] -> []
  in

  let set_dependancy a1 a2 b1 b2 =
    if a1 = a2 && b1 = b2 then
      begin
        (Stack.push (b1,a1) stack);
        (a2,a2)
      end
    else
      begin
        (Stack.push (a1,a2) stack);
        (Stack.push (b1,b2) stack);
        (a2,b2)
      end
  in

  (* 
   * TODO Need to add toplevel rules for VAddSub and VSubAdd before 
   * vectors of size > 4 will work
   *)
  let rec pair_ops wht x1 x2 =
    let cond r1 r2 = ((x1 = r1) && (x2 = r2)) in
    let paired = ref false in
    match wht with 
    | VAdd (r1,a1,b1,n1) as x :: xs  ->
      begin
        let rec search_for_pair xs =
          match xs with
          | VAdd (r2,a2,b2,n2) :: ys when (cond r1 r2) ->
          (
            paired := true;
            let c1, c2 = (set_dependancy a1 a2 b1 b2) in
            (* Stop searching for paired instruction *)
            VAdd (r2,c1,c2,(2*n2)) :: ys
          )
          | VSub (r2,a2,b2,n2) :: ys when (cond r1 r2) ->
          (
            paired := true;
            let c1, c2 = (set_dependancy a1 a2 b1 b2) in
            (* Stop searching for paired instruction *)
            VSubAdd (r2,c1,c2,(2*n2)) :: ys
          )
          | y :: ys -> y :: (search_for_pair ys)
          | [] -> [] 
        in 
        let zs = (search_for_pair xs) in
        if !paired then
          zs
        else
          x :: (pair_ops xs x1 x2)
      end 
    | VSub (r1,a1,b1,n1) as x :: xs  ->
      begin
        let rec search_for_pair xs =
          match xs with
          | VSub (r2,a2,b2,n2) :: ys when (cond r1 r2) ->
          (
            paired := true;
            let c1, c2 = (set_dependancy a1 a2 b1 b2) in
            (* Stop searching for paired instruction *)
            VSub (r2,c1,c2,(2*n2)) :: ys
          )
          | VAdd (r2,a2,b2,n2) :: ys when (cond r1 r2) ->
          (
            paired := true;
            let c1, c2 = (set_dependancy a1 a2 b1 b2) in
            (* Stop searching for paired instruction *)
            VAddSub (r2,c1,c1,(2*n2)) :: ys
          )
          | y :: ys -> y :: (search_for_pair ys)
          | [] -> [] 
        in 
        let zs = (search_for_pair xs) in
        if !paired then
          zs
        else
          x :: (pair_ops xs x1 x2)
      end 
    | VLoad (r1,v1,s1,n1) as x :: xs ->
      begin
        let rec search_for_pair xs =
          match xs with
          | VLoad (r2,v2,s2,n2) :: ys when (cond r1 r2) ->
          (
            paired := true;
            (* Stop searching for paired instruction *)
            VLoad (r2,v2,s1@s2,(2*n2)) :: ys
          )
          | y :: ys -> y :: (search_for_pair ys)
          | [] -> [] 
        in 
        let zs = (search_for_pair xs) in
        if !paired then
          zs
        else
          x :: (pair_ops xs x1 x2)
      end
    | x :: xs -> x :: (pair_ops xs x1 x2)
    | [] -> []
  in

  let rec store_fuse wht_stores =
    let paired = ref false in
    match wht_stores with
    | VStore (r1,v1,s1,n1) as x :: xs ->
      begin
        let rec search_for_pair xs =
          match xs with
            | VStore (r2,v2,s2,n2) :: ys when ((n1 = n2) && (v1 = v2)) ->
            (
              paired := true;
              (Stack.push (r2,r1) stack);
            (* Stop searching for paired instruction *)
              VStore (r1,v1,(s1 @ s2),(2*n1)) :: ys
            )
            | y :: ys ->  y :: (search_for_pair ys)
            | [] -> [] 
        in
        let zs = (search_for_pair xs) in
        if !paired then
          (store_fuse zs)
        else
          x :: (store_fuse xs)
      end
    | x :: xs -> x :: (store_fuse xs)
    | [] -> []
  in
  let fused_stores = (store_fuse  (stores wht))
  and paired_ops = ref (nonstores wht) in
  while not (Stack.is_empty stack) do
    let (a,b) = (Stack.pop stack) in
    paired_ops := (pair_ops !paired_ops a b);
  done;
  (List.rev !paired_ops) @ fused_stores


let rec vectorize_by wht factor =
  let rec _vectorize_by _wht factor =
    if factor == 1 then
      _wht
    else
      _vectorize_by (vectorize _wht) (factor - 1)
  in _vectorize_by (convert wht) factor

