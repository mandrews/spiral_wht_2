function [x,y] = calc_empir(in, alpha, beta)
  global RDTSC TOT_INS L1_DCM L2_DCM IC_ALL;

  [q,w] = calc_p_ins_and_omega(in);

  w0 = [ alpha w beta ]';

  y = in(:,RDTSC) ;

  z = [ ...
    (in(:,TOT_INS) - in(:,IC_ALL)) ...
    q ...
    in(:,L1_DCM) ...
    in(:,L2_DCM) ...
  ];

  step = 1e-3;
  tol  = 1e-3;

  wi = w0;
  p1 = abs(mean((y - (z * w0)) ./ y))
%  cd = (y - (z*w0)) ./ y;
%  p1 = (mean([(mean(cd) + std(cd)) (mean(cd) - std(cd))]))^2
  p2 = Inf;

  while (p2 > tol)
    wj = wi;
    for i=[1:length(wi)]
      wj(i,:) = wj(i,:) + step;
       pi = abs(mean((y - (z * wj)) ./ y));
%      cd = (y - (z*wj)) ./ y;
%      pi = abs(mean([(mean(cd) + std(cd)) (mean(cd) - std(cd))]))^2;
      if (pi < p2)
        wi = wj;
        p2 = pi;
      end

      wj(i,:) = wj(i,:) - 2*step;
      pi = abs(mean((y - (z * wj)) ./ y));
%      cd = (y - (z*wj)) ./ y;
%      pi = abs(mean([(mean(cd) + std(cd)) (mean(cd) - std(cd))]))^2;
      if (pi < p2)
        wi = wj;
        p2 = pi;
      end

    end
  end

  w0
  wi
  p2

  x = z*wi;

  epsilon = mean(y - x);

  xy = filter_outliers([x y]);

  x = xy(:,1) - epsilon;
  y = xy(:,2);

  k = kstest((y-x)./y)

  p = corr(x,y);
