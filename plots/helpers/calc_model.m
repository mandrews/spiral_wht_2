function [x,y,e,r,z,w] = calc_model_papi(in, omega)
  global RDTSC BETA L1_DCM L2_DCM;
  global SPLIT_ALPHA SPLIT_BETA_1 SPLIT_BETA_2 SPLIT_BETA_3
  global SPLITIL_ALPHA SPLITIL_BETA_1 SPLITIL_BETA_2 SPLITIL_BETA_3

  [z,w] = calc_p_ins_and_omega(in);

  n = (z*w') + BETA(1) .* in(:,L1_DCM) + BETA(2) .* in(:,L2_DCM) ;

  m = [ ...
        in(:,SPLIT_ALPHA)  ...
        in(:,SPLIT_BETA_1) ...
        in(:,SPLIT_BETA_2) ...
        in(:,SPLIT_BETA_3) ...
        in(:,SPLITIL_ALPHA)  ...
        in(:,SPLITIL_BETA_1) ...
        in(:,SPLITIL_BETA_2) ...
        in(:,SPLITIL_BETA_3) ...
      ];

  y = in(:,RDTSC) ;

  i = (m * omega);

  x = i + n;

  e = mean(x - y);

  xy = filter_outliers([x y]);

  x = xy(:,1) - e;
  y = xy(:,2);

  r = corr(x,y)

