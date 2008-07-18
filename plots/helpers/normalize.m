function [out] = normalize(in, start, stop)
  global PREC;
  if PREC == 'single'
    single_format;
    for i = [ 1:length(in) ]
      t{i}  = partition(in{i}, 1, start, stop);

      base = t{i}(:,ID).*(2.^t{i}(:,ID));

      for j = [ 2:size(t{i},2) ]
        t{i}(:,j) = t{i}(:,j) ./ base;
      end
    end
  end

  out = t;

