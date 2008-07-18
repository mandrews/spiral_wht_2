% TODO Plot using cache miss counts and papi interpolated instruction count

function [x,y,e,r] = calc_model_papi(in, alpha, beta)
  global RDTSC TOT_INS IC_ALL L1_DCM L2_DCM;
  %global IC_PAPI CMC1 CMC2;

  [x,w] = calc_p_ins_and_omega(in);

  n = (x*w') + beta(1) .* in(:,L1_DCM) + beta(2) .* in(:,L2_DCM) ;
  %n = (x*w') + beta(1) .* in(:,CMC1) + beta(2) .* in(:,CMC2) ;

  y = in(:,RDTSC) ;
  i = alpha * (in(:,TOT_INS) - in(:,IC_ALL));
  %i = alpha * (in(:,IC_PAPI) - in(:,IC_ALL));

  x = i + n;

  e = mean(x - y);

  xy = filter_outliers([x y]);

  x = xy(:,1) - e;
  y = xy(:,2);

  r = corr(x,y);

