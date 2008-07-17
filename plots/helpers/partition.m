function [y] = partition(x, i, a, b)
  if (a == b)
    y = x(find(x(:,i) == a),:);
  else
    y = x(intersect(find(x(:,i) >= a),find(x(:,i) < b)),:);
  end
