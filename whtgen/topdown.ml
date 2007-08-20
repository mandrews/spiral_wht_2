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

let kernel_2 input =
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

let kernel_4 input =
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

let kernel n  =
  match n with
  | 2 -> kernel_2
  | 4 -> kernel_4 
  | n -> failwith "No kernel for this size."

let stride input i k n =
  Array.map (fun x -> (Array.get input x)) (Array.of_list (range i n k))

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

let i_tensor_w code input n k =
  let m = n*k in
  let rec _i_tensor_w code input n k =
    if k = 1 then
      (code (Array.sub input 0 n))
    else
      let y = (code (Array.sub input (m/k) n))
      and x = (_i_tensor_w code input n (k-1)) in
      (vertical_merge y x)
  in (_i_tensor_w code input n k)

let w_tensor_i code input n k =
  let m = n*k in
  let rec _w_tensor_i code input i =
    if i = 0 then
      (code (stride input 0 k m)) 
    else
      let y = (code (stride input i k m)) 
      and x = (_w_tensor_i code input (i-1)) in
      (vertical_merge y x)
  in (_w_tensor_i code input (n+1))

let wht_to_code wht n =
  let rec loads i =
    let t = make_tmp() 
    and m = n - i in
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
    match wht with
    | W n -> ((kernel n) input)
    | Tensor (I k, W n) ->  (i_tensor_w (kernel n) input n k)
    | Tensor (W n, I k) ->  (w_tensor_i (kernel n) input n k)
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

  let x = (loads n) in
  let y = (encode wht x.output) in
  let z = (stores y.output n) in
  (horizontal_merge z (horizontal_merge y x))
  ;;

let n = 16 in
let wht = derive (W n) in
  begin
    print_string (wht_to_string wht); printf "\n";
    let code = (wht_to_code wht n) in 
      print_string (code_to_string code.block)
  end
