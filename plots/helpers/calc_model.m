function [x,y,e,r,z,w] = calc_model_papi(in, alpha, beta, omega, flag)
  global RDTSC TOT_INS IC_ALL L1_DCM L2_DCM;
  global SPLIT_ALPHA SPLIT_BETA_1 SPLIT_BETA_2 SPLIT_BETA_3
  global SPLITIL_ALPHA SPLITIL_BETA_1 SPLITIL_BETA_2 SPLITIL_BETA_3
  global MOVM IMUL;

  [z,w] = calc_p_ins_and_omega(in);

  n = beta(1) .* in(:,L1_DCM) + beta(2) .* in(:,L2_DCM) ;

  if (flag > 0)
    n = n + (z*w');
  end

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

  i = alpha * (m * omega);

  x = i + n;

  e = mean(x - y);

  xy = filter_outliers([x y]);

  x = xy(:,1) - e;
  y = xy(:,2);

  r = corr(x,y)

