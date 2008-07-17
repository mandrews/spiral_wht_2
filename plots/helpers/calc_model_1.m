function [x,y,e,w,a,p] = calc_model_1(in, beta)
  global RDTSC TOT_INS L1_DCM L2_DCM;

  [x,w] = calc_p_ins_and_omega(in);

  n = (x*w') + beta(1) .* in(:,L1_DCM) + beta(2) .* in(:,L2_DCM) ;

  y = in(:,RDTSC) ;
  i = in(:,TOT_INS);

  %a = regress(y , [i n]);
  a = [ 0.9 1.0 ]';
  x = [i n]*a;

  xy = filter_outliers([x y]);
  x = xy(:,1);
  y = xy(:,2);

  e = (y - x) ./ x;

  p = corr(x,y);

