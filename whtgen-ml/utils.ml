let rec unique = function 
  | [] -> []
  | x :: xs -> [x] @ (unique (List.filter (fun y->y<>x) xs))

