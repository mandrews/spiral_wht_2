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

%plot_model(n_tables{1}, 1.2, 0, 0, 30, 2.0, 400, 'seq1_a');
%plot_model(n_tables{2}, 1.2, 0, 0, 30, 2.0, 400, 'seq2_a');
%plot_model(n_tables{3}, 0.35, 0, 0, 10, 1.0, 250, 'seq3_a');
plot_model(n_tables{4}, 0.25, 1, 0, 4, 1.0, 250, 'seq4_a');
