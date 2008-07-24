function [] = plot_empir(x,y,varargin)
  global PREFIX;

  p = inputParser;
  p.addParamValue('suffix', 'tmp');
  p.parse(varargin{:});

  h = figure;
  hold on;

  xlabel('Predicted', 'FontSize', 16);
  ylabel('Actual','FontSize', 16);

  scatter(x, y, 'b.');

  m = round(max([max(x) max(y)])) ;
  line([0 m], [0 m]);
  axis([0 m 0 m]);
  axis square;

  output  = [ PREFIX, '_scatter_', p.Results.suffix ];
  saveas(h,['./eps/', output, '.eps'], 'epsc');
  saveas(h,['./png/', output, '.png'], 'png');

  h = figure;
  hold on;

  xlabel('Error', 'FontSize', 16);
  ylabel('Counts','FontSize', 16);

  q = (y - x)./y;
  hist(q, 50);

  axis square;
  xlim([-0.8 0.8]);

  output  = [ PREFIX, '_error_', p.Results.suffix ];
  saveas(h,['./eps/', output, '.eps'], 'epsc');
  saveas(h,['./png/', output, '.png'], 'png');

