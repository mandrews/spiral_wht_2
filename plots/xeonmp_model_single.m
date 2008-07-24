clear;
path(path,'helpers');
path(path,'plots');
path(path,'format');

single_format;
xeonmp_arch;

tables = { 
  load('../data/xeonmp_L1_sample_500_seq1_table.txt'),
  load('../data/xeonmp_L1_sample_500_seq2_table.txt'),
  load('../data/xeonmp_L1_sample_500_seq3_table.txt'),
  load('../data/xeonmp_L1_sample_500_vec1_table.txt'),
  load('../data/xeonmp_L1_sample_500_vec2_table.txt')
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

[x1,y1,e1,r1,z1,w1] = calc_model(n_tables{1}, 1.25, BETA, k, 1);
plot_scatter(x1, y1, 'suffix', 'model_seq1_a');

[x2,y2,e2,r2,z2,w2] = calc_model(n_tables{2}, 1.25, BETA, k, 1);
plot_scatter(x2, y2, 'suffix', 'model_seq2_a');

[x3,y3,e3,r3,z3,w3] = calc_model(n_tables{3}, 0.50, BETA, k, 1);
plot_scatter(x3, y3, 'suffix', 'model_seq3_a');

[x4a,y4a,e4a,r4a,z4a,w4a] = calc_model(n_tables{4}, 0.60, BETA, k, 1);
plot_scatter(x4a, y4a, 'suffix', 'model_vec1_a');

[x4b,y4b,e4b,r4b,z4b,w4b] = calc_model(n_tables{4}, 0.55, BETA, k, 0);
plot_scatter(x4b, y4b, 'suffix', 'model_vec1_b');

[x5a,y5a,e5a,r5a,z5a,w5a] = calc_model(n_tables{5}, 0.35, BETA, k, 1);
plot_scatter(x5a, y5a, 'suffix', 'model_vec2_a');

[x5b,y5b,e5b,r5b,z5b,w5b] = calc_model(n_tables{5}, 0.35, BETA, k, 0);
plot_scatter(x5b, y5b, 'suffix', 'model_vec2_b');

