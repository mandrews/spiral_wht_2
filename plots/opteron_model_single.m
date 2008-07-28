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

[x1,y1,e1,r1,z1,w1] = calc_model(n_tables{1}, k);
plot_scatter(x1, y1, 'suffix', 'model_seq1');

[x2,y2,e2,r2,z2,w2] = calc_model(n_tables{2}, k);
plot_scatter(x2, y2, 'suffix', 'model_seq2');

[x3,y3,e3,r3,z3,w3] = calc_model(n_tables{3}, k);
plot_scatter(x3, y3, 'suffix', 'model_seq3');

[x4,y4,e4,r4,z4,w4] = calc_model(n_tables{4}, k);
plot_scatter(x4, y4, 'suffix', 'model_vec1');

[x5,y5,e5,r5,z5,w5] = calc_model(n_tables{5}, k);
plot_scatter(x5, y5, 'suffix', 'model_vec2');


