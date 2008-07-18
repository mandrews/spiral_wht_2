function [skew] = plot_model(in, alpha, a, b, c, d, SUFFIX)
  global BETA PREFIX;
  k = 1;
  [x,y,e,r] = calc_model(in, alpha, BETA);

  skew = skewness((y-x)./y);

  h = figure;
  hold on;

  xlabel('Predicted', 'FontSize', 16);
  ylabel('Actual','FontSize', 16);

  scatter(x, y, 'b.');

  if (b ~= 0)
    z = line([a b], [a b]);
    axis([a b a b]);
  end

  output  = [ PREFIX, 'model_scatter', SUFFIX ];
  saveas(h,['./eps/', output, '.eps'], 'epsc');
  saveas(h,['./png/', output, '.png'], 'png');

  h = figure;
  hold on;

  xlabel('Error', 'FontSize', 16);
  ylabel('Counts','FontSize', 16);

  hist( (y - x)./y, 50);
  if (c ~= 0 && d ~= 0)
    axis([-c c 0 d]);
  end

  %kstest((y - x)./y, [])

  output  = [ PREFIX, 'model_error', SUFFIX ];
  saveas(h,['./eps/', output, '.eps'], 'epsc');
  saveas(h,['./png/', output, '.png'], 'png');

