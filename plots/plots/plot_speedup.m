function [skew] = plot_model(before, after, ax, flag, SUFFIX)
  global PREFIX RDTSC VECTOR_SIZE;

  x = before(:,RDTSC);
  y = after(:,RDTSC);
  z = x ./ y;

  h = figure;
  hold on;

  zx = filter_outliers([z x]);

  scatter(zx(:,2), zx(:,1), 'b.');

  axis(ax);

  if (flag == 1)
    plot([0 max(x) ], [VECTOR_SIZE VECTOR_SIZE], 'r-');
  end

  xlabel('Runtime', 'FontSize', 16);
  ylabel('Speedup','FontSize', 16);
  
  output  = [ PREFIX, '_speedup_scatter_', SUFFIX ];
  saveas(h,['./eps/', output, '.eps'], 'epsc');
  saveas(h,['./png/', output, '.png'], 'png');

