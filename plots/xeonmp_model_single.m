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

%plot_model(n_tables{1}, 0.80, 0, 0, 18.00, 0.8 , 400, 'seq1_a');
%plot_model(n_tables{2}, 0.80, 0, 0, 18.00, 0.8,  400, 'seq2_a');
%plot_model(n_tables{2}, 1.00, 1, 0, 18.00, 0.8,  500, 'seq2_b');
%
%plot_model(n_tables{3}, 0.25, 0, 0,  5.5, 0.8, 400, 'seq3_a');
%plot_model(n_tables{3}, 0.25, 1, 0,  5.5, 0.8, 800, 'seq3_b');

%plot_model(n_tables{4}, 0.25, 0, 0, 1.8, 0.5, 400, 'vec1_a');
%plot_model(n_tables{4}, 0.25, 1, 0, 1.8, 0.5, 500, 'vec1_b');

%plot_model(n_tables{5}, 0.75, 0, 0, 4.5, 0.8, 400, 'vec2_a');
%plot_model(n_tables{5}, 0.75, 1, 0, 4.5, 0.8, 400, 'vec2_b');
