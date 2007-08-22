open Printf

type wht =
  | W of int
  | I of int
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

type mask = int list

type code =
  | Add   of register * register * register
  | Sub   of register * register * register
  | Load  of register * variable * offset
  | Store of register * variable * offset
  | Shuf  of register * register * register * mask

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
  | Tensor (a,b) -> (wht_size a) * (wht_size b)
  | Product (a,b) -> (wht_size a)

let rec expr_to_string x =
  match x with
  | Times (a,b)  -> (expr_to_string a) ^ "*" ^ (expr_to_string b)
  | Plus  (a,b)  -> (expr_to_string a) ^ "+" ^ (expr_to_string b)
  | Constant a   -> sprintf "%d" a 
  | Variable a   -> a

let rec mask_to_string x =
  match x with
  | [] -> ""
  | x :: [] -> sprintf "%d" x
  | x :: xs -> (sprintf "%d, " x) ^ (mask_to_string xs)

let rec wht_to_string wht =
  match wht with
  | W n -> sprintf "W(%d)" n
  | I n -> sprintf "I(%d)" n
  | Tensor  (x,y) -> (wht_to_string x) ^ " X " ^ (wht_to_string y) 
  | Product (x,y) -> "(" ^ (wht_to_string x) ^ ")(" ^ (wht_to_string y) ^ ")"

let rec code_to_string code =
  let _code_to_string x =
    match x with
    | Load  (r,v,o)   -> sprintf "t%d = %s[%s]\n" r v (expr_to_string o)
    | Add   (r,a,b)   -> sprintf "t%d = t%d + t%d;\n" r a b
    | Sub   (r,a,b)   -> sprintf "t%d = t%d - t%d;\n" r a b
    | Store (r,v,o)   -> sprintf "%s[%s] = t%d\n" v (expr_to_string o) r
    | Shuf  (r,a,b,m) -> sprintf "shuf(t%d,t%d,t%d,%s);\n" r a b (mask_to_string m)
    (*| other -> sprintf "/* undefined */\n" *)
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

let sse2_w_4 input = 
  assert ((Array.length input) = 2);
  let t1 = make_tmp () 
  and t2 = make_tmp ()
  and t3 = make_tmp () 
  and t4 = make_tmp ()
  and t5 = make_tmp () 
  and t6 = make_tmp ()
  in {
    output = (Array.of_list [t3;t4]);
    block  =
      [ Add  (t1,(Array.get input 0),(Array.get input 1));
        Sub  (t2,(Array.get input 0),(Array.get input 1));
        Shuf (t3, t1, t2, [0;0]);
        Shuf (t4, t1, t2, [1;1]);
        Add  (t5, t3, t4);
        Sub  (t6, t3, t4);
        Shuf (t3, t5, t6, [0;0]);
        Shuf (t4, t5, t6, [1;1]);
      ]
    }

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
  let mn = (Array.length input) in
  let rec _permute p =
    let x = [(stride input p m mn)] in
    if p = 0 then
      x
    else
      List.append (_permute (p-1)) x
  in _permute (m-1)

(* Partition array into blocks of size k *)
let partition input k =
  let mn = (Array.length input) in
  let rec _partition p =
    let x = [ (Array.sub input p k) ] in
    if p =  (mn - k) then
      x
    else
      List.append x (_partition (p+k))
  in (_partition 0)

let i_tensor_a m a input apply =
  let n = wht_size a in
  let ims = (List.map (fun p -> apply a p) (partition input (n/m))) in
    List.fold_left (vertical_merge) empty_chunk (List.rev ims)

let a_tensor_i m a input apply =
  let ims = (List.map (fun p -> apply a p) (permute input m)) in
    List.fold_left (vertical_merge) empty_chunk ims

let sse2_w_n_tensor_i_m input = 
  let whts = List.map (fun p -> scalar_w_2 p) (permute input 2) in
    List.fold_left (vertical_merge) empty_chunk whts

let rec wht_to_code_rules wht input apply =
  (printf "wht_to_code_rules: %s\n" (wht_to_string wht));
  match wht with
  | Tensor (I m, a) ->  i_tensor_a m a input apply
  | Tensor (a, I m) ->  a_tensor_i m a input apply
  | Product (a,b) -> 
    begin
      let x = (apply b input) in
      let y = (apply a x.output) in
      (horizontal_merge y x)
    end
  | other -> failwith "No encoding" 

let rec scalar_kernels wht input =
  match wht with
  | W 2 -> scalar_w_2 input
  | W 4 -> scalar_w_4 input
  | other -> wht_to_code_rules wht input scalar_kernels

let rec sse2_kernels wht input =
  match wht with
  | W 4 -> sse2_w_4 input 
  | Tensor (W n, I m) -> sse2_w_n_tensor_i_m input
  | other -> wht_to_code_rules wht input sse2_kernels

let wht_to_code wht size specific_rules =
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
  let y = (specific_rules wht x.output) in
  let z = (stores y.output size) in
  (horizontal_merge z (horizontal_merge y x))
  ;;

(*
let n = 8 in
let wht = derive (W n) in
  begin
    print_string (wht_to_string wht); printf "\n";
    let code = (wht_to_code wht n scalar_kernels) in 
      print_string (code_to_string code.block)
  end
*)

let n = 8 
and v = 2 in
let wht = derive (W n) in
  begin
    print_string (wht_to_string wht); printf "\n";
    let code = (wht_to_code wht (n/v) sse2_kernels) in 
      print_string (code_to_string code.block)
  end
