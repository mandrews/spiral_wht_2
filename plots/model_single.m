% TODO
% _ Derive Counts
% _ Generalize
% _ Check In

clear;
path(path,'helpers');
path(path,'format');

single_format;

tables = { 
  load('../data/opteron_L1_sample_500_seq1_table.txt'),
  load('../data/opteron_L1_sample_500_seq2_table.txt'),
  load('../data/opteron_L1_sample_500_vec1_table.txt'),
  load('../data/opteron_L1_sample_500_vec2_table.txt')
};
%load('../data/xeonmp_L1_sample_500_seq3_table.txt'),

L1_A = 13;
L1_B = 20;

p = normalize(tables, L1_A, L1_B);

k = 2;

beta = [ 3.0 14.0 ]; % Got these from the manual
[x,y,e,w,a,r] = calc_model_1(p{k}, beta);

h = figure;
hold on;

xlabel('Actual', 'FontSize', 16);
ylabel('Predicted','FontSize', 16);

scatter(x, y, 'b.');

%z = line([0 12], [0 12]);
%axis([0 12 0 12]);

%output  = 'ix_sse2_model_seq';
%saveas(h,['./eps/', output, '.eps'], 'epsc');
%saveas(h,['./png/', output, '.png'], 'png');


h = figure;
hold on;

xlabel('Error', 'FontSize', 16);
ylabel('Counts','FontSize', 16);

hist( e, 50);

