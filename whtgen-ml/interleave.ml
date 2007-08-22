open Wht
open Utils

open Printf

let make_base =
  let base_count = ref 0
    in fun () -> begin
      base_count := !base_count + 1;
      !base_count
    end

let duplicate wht =
  let m = make_tmp()
  and b = make_base () in
  (* This procedure only maintains consistency when called from duplicate *)
  let new_var x =
    match x with 
    | Variable a -> Plus (Variable a, Times (Constant b, Variable "D"))
    | Plus (Variable a, Times (Constant i, Variable "D")) ->
      Plus (Variable a, Times (Constant (b+i), Variable "D")) 
    | other -> other
  in
  let relabel x m =
    (* Increment counter but do not use value *)
    let _ = make_tmp() in (); 
    match x with
    | Load      (Temporary reg,var,expr) -> 
      Load      (Temporary (reg+m),(new_var var),expr)  
    | Store     (Temporary reg,var,expr) -> 
      Store     (Temporary (reg+m),(new_var var),expr)            
    | Add       (Temporary reg0, Temporary reg1, Temporary reg2) ->
      Add       (Temporary (reg0+m), Temporary (reg1+m), Temporary (reg2+m)) 
    | Sub       (Temporary reg0, Temporary reg1, Temporary reg2) ->
      Sub       (Temporary (reg0+m), Temporary (reg1+m), Temporary (reg2+m)) 
    | other -> other
  in List.map (fun x -> relabel x m) wht

let rec interleave a b =
  assert (List.length a == List.length b);
  if List.length a == 0 then
    []
  else
    let x  = List.hd a
    and xs = List.tl a 
    and y  = List.hd b
    and ys = List.tl b in
        [x;y] @ (interleave xs ys)

let factor_common wht =
  let loads = ref ([] : code list) in

  let _factor_var v =
    match v with
    | Plus (Variable v, (Times (Constant i, Variable "D") as factor)) ->
      let xi = (sprintf "%s%d" v i) in
        loads := Load (Variable xi, (Variable v), factor) :: ! loads;
        (Variable xi)
    | other -> other
  in

  let rec _factor_common w =
    match w with
    | Store (r, v, s) :: xs ->
      Store (r, (_factor_var v), s) :: _factor_common xs
    | Load  (r, v, s) :: xs ->
      Load  (r, (_factor_var v), s) :: _factor_common xs
    | x :: xs -> x :: (_factor_common xs)
    | [] -> []
  in 
  
  (unique !loads) @ (_factor_common wht)

let interleave_by wht factor =
  let rec _interleave_by wht factor =
    if factor == 0 then
      wht
    else
      _interleave_by (interleave wht (duplicate wht)) (factor - 1)
  in  factor_common (_interleave_by wht factor)

