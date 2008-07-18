function [y] = filter_outliers(x)
% FILTER_OUTLIERS Filter outliers outside the 'inner fence'.
% Y = FILTER_OUTLIERS(X) filters the data in matrix X by removing all elements
% outside 3/2 IQR(X) +/- 1st/3rd quartiles.

% From http://en.wikipedia.org/wiki/Outlier

  n = length(x);

  s  = repmat((3.0 / 2.0 )*iqr(x),n,1);
  q1 = repmat(prctile(x, 25), n, 1);
  q2 = repmat(prctile(x, 75), n, 1);

  o1 = (x + s) < q1;
  o2 = (x - s) > q2;

  y = [];

  f = any([o1 o2], 2);

  for i=[1:n]
    if f(i) == 0
      y = [y; x(i,:)];
    end
  end

  size(y)

