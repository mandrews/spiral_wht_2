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

plot_speedup(n_tables{1}, n_tables{3}, [0 30 0 8], 0, 'seq1');
plot_speedup(n_tables{3}, n_tables{4}, [0 10 1 5], 1, 'vec1');
plot_speedup(n_tables{2}, n_tables{5}, [0 25 1 5], 1, 'vec2');

