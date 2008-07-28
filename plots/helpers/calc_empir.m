function [x,y,b,e,r] = calc_empir(in, k, varargin)
  global RDTSC TOT_INS IC_ALL L1_DCM L2_DCM;
  global SPLIT_ALPHA SPLIT_BETA_1 SPLIT_BETA_2 SPLIT_BETA_3
  global SPLITIL_ALPHA SPLITIL_BETA_1 SPLITIL_BETA_2 SPLITIL_BETA_3

  [z,w] = calc_p_ins_and_omega(in);

  y = in(:,RDTSC) ;

  m = [
    in(:,SPLIT_ALPHA) ...
    in(:,SPLIT_BETA_1) ...
    in(:,SPLIT_BETA_2) ...
    in(:,SPLIT_BETA_3)  ...
    in(:,SPLITIL_ALPHA) ...
    in(:,SPLITIL_BETA_1) ...
    in(:,SPLITIL_BETA_2) ...
    in(:,SPLITIL_BETA_3)  ];

  for i=1:length(m)
    m(i,:) = m(i,:) .* k';
  end

  x = [ z m ...
    in(:,L1_DCM) in(:,L2_DCM) ...
  ];

%  kp = princomp(x);
%  x = x * kp;

  [b,xxx,r] = lsqnonneg(x,y);
  e = mean(r);

  x = x*b + e;

  xy = filter_outliers([x y]);

  x = xy(:,1);
  y = xy(:,2);

  r = corr(x,y);

