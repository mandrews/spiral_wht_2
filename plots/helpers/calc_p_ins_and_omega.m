function [x,w] = calc_p_ins_and_omega(in)
  global PREC LAT THP NUM;
  if PREC == 'single'
    single_format;

    ops = [
      UNPCKHPS ...
      UNPCKLPS ...
      SHUFPS ...
      ADDSS ...
      SUBSS ...
      ADDPS ...
      SUBPS ...
      MOVSSM ...
      MOVAPSM ...
      MOVHPSM ...
      MOVLPSM ...
      MOVAPS ...
      ADD ...
      SUB ...
      MOV ...
      IMUL ...
      XOR ...
      SHL ...
      MOVM ...
      MOVHLPS ...
      MOVLHPS ...
    ];

    x = [];
    l = [];
    t = [];
    n = [];

    for op = ops
      x = [ x in(:,op) ];
      l = [ l LAT{op}  ];
      t = [ t THP{op}  ];
      n = [ n NUM{op}  ];
    end
  end

  w =  (l + ((n - 1) .* t)) ./ n;

