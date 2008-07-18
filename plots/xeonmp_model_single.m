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

%plot_model(n_tables{1}, 1.0, 0, 18, 0.5, 450, 'seq1');
%plot_model(n_tables{2}, 1.0, 0, 18, 0.5,  500, 'seq2');
%plot_model(n_tables{3}, 0.55, 0, 5.5, 0.5, 700, 'seq3');
%plot_model(n_tables{4}, 0.70, 0, 1.8, 0.5, 400, 'vec1');
%plot_model(n_tables{5}, 0.95, 0, 4.5, 0.5, 400, 'vec2');

