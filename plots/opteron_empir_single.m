clear;
path(path,'helpers');
path(path,'plots');
path(path,'format');

single_format;
opteron_arch;

tables = { 
  load('../data/opteron_L1_sample_500_seq1_table.txt'),
  load('../data/opteron_L1_sample_500_seq2_table.txt'),
  load('../data/opteron_L1_sample_500_seq3_table.txt'),
  load('../data/opteron_L1_sample_500_vec1_table.txt'),
  load('../data/opteron_L1_sample_500_vec2_table.txt')
};

n_tables = normalize(tables, L1_A, L1_B);

all = [ n_tables{1}; n_tables{2}; n_tables{3}; n_tables{4}; n_tables{5} ];

m = [
  all(:,SPLIT_ALPHA) ...
  all(:,SPLIT_BETA_1) ...
  all(:,SPLIT_BETA_2) ...
  all(:,SPLIT_BETA_3)  ...
  all(:,SPLITIL_ALPHA) ...
  all(:,SPLITIL_BETA_1) ...
  all(:,SPLITIL_BETA_2) ...
  all(:,SPLITIL_BETA_3)  ];
k = lsqnonneg(m, all(:,TOT_INS) - all(:,IC_ALL));

[x1,y1,b1,e1,r1] = calc_empir(n_tables{1}, k);
plot_scatter(x1, y1,'suffix', 'empir_seq1');

[x2,y2,b2,e2] = calc_empir(n_tables{2}, k);
plot_scatter(x2, y2, 'suffix', 'empir_seq2');

[x3,y3,b3,e3] = calc_empir(n_tables{3},k);
plot_scatter(x3, y3, 'suffix', 'empir_seq3');

[x4,y4,b4,e4] = calc_empir(n_tables{4},k);
plot_scatter(x4, y4, 'suffix', 'empir_vec1');

[x5,y5,b5,e5] = calc_empir(n_tables{5},k);
plot_scatter(x5, y5, 'suffix', 'empir_vec2');
