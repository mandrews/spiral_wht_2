function [x,y,e,a,r] = calc_model_1(in, alpha, beta)
  global RDTSC IC_OVR L1_DCM L2_DCM;
  global CMC1 CMC2;

  [x,w] = calc_p_ins_and_omega(in);

  n = (x*w') + beta(1) .* in(:,CMC1) + beta(2) .* in(:,CMC2) ;

  y = in(:,RDTSC) ;
  i = alpha * in(:,IC_OVR);

  x = i + n;

  epsilon = mean(x - y);

  xy = filter_outliers([x y]);

  x = xy(:,1) - epsilon;
  y = xy(:,2);

  e = (y - x) ./ x;

  r = corr(x,y);

