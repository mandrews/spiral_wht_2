open Printf

type wht =
  | W of int
  | I of int
  | L of int * int
  | Tensor  of wht * wht
  | Product of wht * wht

type register = int
type variable = string

type expr =
  | Variable  of variable
  | Constant  of int
  | Times     of expr * expr
  | Plus      of expr * expr

type offset = expr

type code =
  | Add   of register * register * register
  | Sub   of register * register * register
  | Load  of register * variable * offset
  | Store of register * variable * offset

type chunk = 
  {
    output : register array;
    block  : code     list
  }

let empty_chunk =
  {
    output = Array.of_list [];
    block  = []
  }

let rec range a b s =
  if a >= b then []
  else a :: range (a+s) b s
  ;;

let make_tmp =
  let tmp_count = ref 0
    in fun () -> begin
      tmp_count := !tmp_count + 1;
      !tmp_count
    end

let rec derive w =
  match w with
  | W 2 -> W 2
  | W 4 -> W 4
  | W n -> (Product (Tensor (W 2, I (n/2)), Tensor (I 2, derive (W (n/2)))))
  | other -> other

let rec wht_size w =
  match w with
  | W n -> n
  | I k -> k
  | L (nm,m) -> nm
  | Tensor (a,b) -> (wht_size a) * (wht_size b)
  | Product (a,b) -> (wht_size a)

let rec derive_v w =
  let v = 2 in
  match w with
  (* Specific Base cases *)
  | Tensor (I m, W 4) as base -> base
  (* Base cases *)
  | Tensor (W n, I m) as base when m = v -> base
  | L (nm,m) as base when (m = 2 && nm = 2*v) -> base
  | L (nm,m) as base when (m = v && nm = 2*v) -> base
  (* Recursive cases *)
  | Tensor (a, I m) when m mod v = 0 && m <> 2 ->
    derive_v (Tensor (Tensor (a, I v), I (m/v)))
  (*
  | I mn when (mn mod v = 0) -> 
    derive_v (Tensor (I v, I (mn/v)))
  | I mn when (mn mod v = 0) -> 
    derive_v (Tensor (I (mn/v), I v))
  *)
  (*
  | Tensor (I m,a) ->
    let n = (wht_size a) in
    (Product (Product (L (m*n,m), Tensor (a, I m)), L (m*n,m)))
  *)
  | Tensor (a,I m) ->
    let n = (wht_size a) in
    derive_v (Product (Product (L (m*n,m), Tensor (I m,a)), L (m*n,n)))
  | W n -> derive_v (Product (Tensor (W 2, I (n/2)), Tensor (I 2, W (n/2))))
  (* Stop cases *)
  (*
  | Tensor  (a,b) -> Tensor (derive_v a, derive_v b)
  *)
  | Product (a,b) -> Product (derive_v a, derive_v b)
  | other -> other

let rec expr_to_string x =
  match x with
  | Times (a,b)  -> (expr_to_string a) ^ "*" ^ (expr_to_string b)
  | Plus  (a,b)  -> (expr_to_string a) ^ "+" ^ (expr_to_string b)
  | Constant a   -> sprintf "%d" a 
  | Variable a   -> a

let rec wht_to_string wht =
  match wht with
  | W n -> sprintf "W(%d)" n
  | I n -> sprintf "I(%d)" n
  | L (nm,m) -> sprintf "L(%d,%d)" nm m
  | Tensor  (x,y) -> (wht_to_string x) ^ " X " ^ (wht_to_string y) 
  | Product (x,y) -> "(" ^ (wht_to_string x) ^ ")(" ^ (wht_to_string y) ^ ")"

let rec code_to_string code =
  let _code_to_string x =
    match x with
    | Load  (r,v,o) -> (sprintf "t%d = %s[%s]\n" r v (expr_to_string o))
    | Add   (r,a,b) -> (sprintf "t%d = t%d + t%d;\n" r a b)
    | Sub   (r,a,b) -> (sprintf "t%d = t%d - t%d;\n" r a b)
    | Store (r,v,o) -> (sprintf "%s[%s] = t%d\n" v (expr_to_string o) r)
  in
  match code with
  | x :: xs -> (_code_to_string x) ^ (code_to_string xs)
  | [] -> ""

let scalar_w_2 input =
  assert ((Array.length input) = 2);
  let t1 = make_tmp () 
  and t2 = make_tmp ()
  in {
    output = (Array.of_list [t1;t2;]);
    block  =
      [ Add (t1,(Array.get input 0),(Array.get input 1));
        Sub (t2,(Array.get input 0),(Array.get input 1));
      ]
    }

let scalar_w_4 input =
  assert ((Array.length input) = 4);
  let t1 = make_tmp () 
  and t2 = make_tmp ()
  and t3 = make_tmp ()
  and t4 = make_tmp ()
  and t5 = make_tmp ()
  and t6 = make_tmp ()
  and t7 = make_tmp ()
  and t8 = make_tmp ()
  in {
    output = (Array.of_list [t5;t6;t7;t8]);
    block  =
      [ Add (t1,(Array.get input 0),(Array.get input 1));
        Sub (t2,(Array.get input 0),(Array.get input 1));
        Add (t3,(Array.get input 2),(Array.get input 3));
        Sub (t4,(Array.get input 2),(Array.get input 3));
        Add (t5,t1,t3);
        Sub (t7,t1,t3);
        Add (t6,t2,t4);
        Sub (t8,t2,t4);
      ]
    }

let sse2_w_2 input = empty_chunk
let sse2_w_4 input = empty_chunk
let sse2_l_2 input = empty_chunk
let sse2_l_4 input = empty_chunk

let scalar_kernels t n  =
  match (t,n) with
  | ('W',2) -> scalar_w_2
  | ('W',4) -> scalar_w_4 
  | ('W',n) -> failwith "No WHT kernel for this size."
  | other -> failwith "No kernel for this type."

let sse2_kernels t n =
  match (t,n) with
  | ('W',4) -> sse2_w_2
  | ('W',4) -> sse2_w_4
  | ('L',2) -> sse2_l_2
  | ('L',4) -> sse2_l_4
  | other -> failwith "No kernel for this type."

let vertical_merge y x =
  {
    output = (Array.append x.output y.output);
    block  = x.block @ y.block
  }

let horizontal_merge y x =
  {
    output = y.output;
    block  = x.block @ y.block
  }

let stride input i k n =
  Array.map (fun x -> (Array.get input x)) (Array.of_list (range i n k))

(* Permute array input by m *)
let permute input m =
  let nm = (Array.length input) in
  let rec _permute p =
    let x = [(stride input p m nm)] in
    if p = 0 then
      x
    else
      List.append (_permute (p-1)) x
  in _permute (m-1)

(* Partition array into blocks of size k *)
let partition input k =
  let nm = (Array.length input) in
  let rec _partition p =
    let x = [ (Array.sub input p k) ] in
    if p =  (nm - k) then
      x
    else
      List.append x (_partition (p+k))
  in (_partition 0)

let wht_to_code wht size klookup =
  let rec loads i =
    let t = make_tmp() 
    and m = size - i in
    let x = {
      output  = (Array.of_list [ t ]);
      block   = [ Load  (t,"x",Times (Constant m, Variable "S")) ]
    } in
    if i == 1 then 
      x
    else 
      (vertical_merge (loads (i-1)) x)
  in 

  let rec encode wht input =
    let m = (Array.length input) in
    match wht with
    | W n -> (klookup 'W' n) input
    | L (mn,m) -> (klookup 'L' m) input
    | Tensor (I k, x) ->  
      let iks = (List.map (fun p -> encode x p) (partition input (m/k))) in
        List.fold_left (vertical_merge) empty_chunk (List.rev iks)
    | Tensor (x, I k) ->  
      let iks = (List.map (fun p -> encode x p) (permute input k)) in
        List.fold_left (vertical_merge) empty_chunk iks
    | Product (a,b) -> 
      begin
        let x = (encode b input) in
        let y = (encode a x.output) in
        (horizontal_merge y x)
      end
    | other -> failwith "No encoding" 
  in 

  let rec stores input i =
    let t = (Array.get input (i-1)) in
    let x = {
      output  = ((Array.of_list []) : register array);
      block   = [ Store (t,"x",Times (Constant (i-1), Variable "S")) ]
    } in
    if i == 1 then 
      x
    else 
      (vertical_merge x (stores input (i-1)))
  in

  let x = (loads size) in
  let y = (encode wht x.output) in
  let z = (stores y.output size) in
  (horizontal_merge z (horizontal_merge y x))
  ;;

(*
let n = 16 in
let wht = derive (W n) in
  begin
    print_string (wht_to_string wht); printf "\n";
    let code = (wht_to_code wht n scalar_kernels) in 
      print_string (code_to_string code.block)
  end
*)

let n = 8 
and v = 2 in
let wht = derive_v (W n) in
  begin
    print_string (wht_to_string wht); printf "\n";
    let code = (wht_to_code wht (n/v) sse2_kernels) in 
      print_string (code_to_string code.block)
  end

