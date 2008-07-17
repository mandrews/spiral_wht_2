clear;
path(path,'helpers');

ID        = 1;
TOT_CYC   = 2;
TOT_INS   = 3;
L1_DCM    = 4;
L2_DCM    = 5;
UNPCKHPD  = 6;
UNPCKLPD  = 7;
ADDSUBSD  = 8;
ADDSUBSDM = 9;
ADDSUBPD  = 10;
ADDSUBPDM = 11;  
MOVSD     = 12;
MOVSDM    = 13;
MOVAPD    = 14;
MOVAPDM   = 15;
MOVHPDM   = 16;
MOVLPDM   = 17;
CMC1      = 18;
LEFT      = 19;
RIGHT     = 20;
BASE      = 21;
MODEL     = 22;
EMPIR     = 23;

a = { 
  load('kodiak/model_p_2/ix_double.txt'),
  load('kodiak/model_p_2/ix_sse2_i_2_rule.txt') 
};

for i = [ 1:2]
  b{i}  = partition(a{i},ID, 12,19);
  d{i}  = b{i};

  d{i}(:,MODEL) = zeros(length(d{i}(:,ID)),1);
  d{i}(:,EMPIR) = zeros(length(d{i}(:,ID)),1);

  d{i}(:,ADDSUBSD) = d{i}(:,ADDSUBSD) + d{i}(:,ADDSUBSDM);
  d{i}(:,ADDSUBPD) = d{i}(:,ADDSUBPD) + d{i}(:,ADDSUBPDM);
  d{i}(:,UNPCKHPD) = d{i}(:,UNPCKHPD) + d{i}(:,UNPCKLPD);
  d{i}(:,MOVHPDM)  = d{i}(:,MOVHPDM)  + d{i}(:,MOVLPDM);

%  tm  = d{i}(:,MOVSDM) + d{i}(:,MOVAPDM) + d{i}(:,MOVHPDM) ;
%  tm1 = d{i}(:,MOVSDM) ./ tm;
%  tm2 = d{i}(:,MOVAPDM) ./ tm;
%  tm3 = d{i}(:,MOVHPDM) ./ tm;
  tm1 = 1;
  tm2 = 1;
  tm3 = 1;

  d{i}(:,MOVSDM) = d{i}(:,MOVSDM) - (d{i}(:,L1_DCM) .* tm1);
  d{i}(:,MOVAPDM) = d{i}(:,MOVAPDM) - (d{i}(:,L1_DCM) .* tm2);
  d{i}(:,MOVHPDM) = d{i}(:,MOVHPDM) - (d{i}(:,L1_DCM) .* tm3);

  d{i}(:,TOT_INS) = d{i}(:,TOT_INS) - ...
    ( d{i}(:,UNPCKHPD) + ...
      d{i}(:,ADDSUBSD) + ...
      d{i}(:,ADDSUBPDM) + ...
      d{i}(:,ADDSUBPD) + ...
      d{i}(:,MOVSD) + ...
      d{i}(:,MOVSDM) + ...
      d{i}(:,MOVAPD) + ...
      d{i}(:,MOVAPDM) + ...
      d{i}(:,MOVHPDM) );

  ni = d{i}(:,ID).*(2.^d{i}(:,ID));

  for j = [ TOT_CYC:EMPIR]
    d{i}(:,j) = d{i}(:,j) ./ ni;
  end

end

V = 2.0;

% 
% Predicting cycles for sequential
%

x1 = [  d{1}(:,ADDSUBSD) ...
        d{1}(:,ADDSUBPD) ...
        d{1}(:,MOVAPD)  ...
        d{1}(:,MOVSDM) ...
        d{1}(:,MOVAPDM)  ...
        d{1}(:,MOVHPDM)  ...
        d{1}(:,UNPCKHPD) ...
    ];

% 
% Accounting for cycles after vectorization
%

x2 = [  d{2}(:,ADDSUBSD) ...
        d{2}(:,ADDSUBPD) ...
        d{2}(:,MOVAPD)  ...
        d{2}(:,MOVSDM) ...
        d{2}(:,MOVAPDM)  ...
        d{2}(:,MOVHPDM)  ...
        d{2}(:,UNPCKHPD) ...
    ];

w1 = [  0.939 ...
        0.939 ...
        1.902 ...
        0.953 ...
        0.953 ...
        0.912 ...
        1.000 ];
l1 = [  3.0 ...
        3.0 ...
        1.0 ...
        2.53  ...
        2.53  ...
        3.50 ...
        1.000 ];
t1 = [  1.0 ...
        1.0 ...
        0.5 ...
        1.0  ...
        1.0  ...
        1.0 ...
        1.000 ];

n1 = [ 2 2 1 4 4 2 2 ];
w1 =  n1 ./ (l1 + ((n1 - 1) .* t1)) ;

w2 = w1;


z1 = [ d{1}(:,TOT_INS) (x1*w1') (d{1}(:,L1_DCM)) d{1}(:,L2_DCM) ];
z2 = [ d{2}(:,TOT_INS) (x2*w2') (d{2}(:,L1_DCM)) d{2}(:,L2_DCM) ];

b1 = [ 0.45 1.0  4.2 7.0 ]';
b2 = [ 0.50 1.0  4.0 7.0 ]';

d{1}(:,MODEL) = z1*b1 ;
d{2}(:,MODEL) = z2*b2;

%k1 = princomp([ x1; x2]);
%k2 = k1;
k1 = 1;
k2 = 1;

e1 = [ d{1}(:,TOT_INS) (x1*k1) (d{1}(:,L1_DCM)) (d{1}(:,L2_DCM)) ];
e2 = [ d{2}(:,TOT_INS) (x2*k1) (d{2}(:,L1_DCM)) (d{2}(:,L2_DCM)) ];

%r1 = regress([ d{1}(:,TOT_CYC); d{1}(:,TOT_CYC) ], [e1; e2]);
%r2 = r1;
%r1 = lsqnonneg([e1;e2], [ d{1}(:,TOT_CYC); d{1}(:,TOT_CYC) ]);
%r2 = r1;
%d{1}(:,EMPIR) = e1*r1;
%d{2}(:,EMPIR) = e2*r2;

r1 = robustfit([e1;e2], [d{1}(:,TOT_CYC);d{2}(:,TOT_CYC)], 'logistic');
r1(4) = r1(3);
r2 = r1;
%r2 = robustfit(e2, d{2}(:,TOT_CYC), 'logistic');
d{1}(:,EMPIR) = e1*r1(2:length(r1)) + r1(1);
d{2}(:,EMPIR) = e2*r2(2:length(r2)) + r2(1);

corr(d{1}(:,TOT_CYC), d{1}(:,EMPIR))
corr(d{2}(:,TOT_CYC), d{2}(:,EMPIR))

%
% Scatter Seq
%
h = figure;
hold on;

xlabel('Actual', 'FontSize', 16);
ylabel('Predicted','FontSize', 16);

xy = filter_outliers([ d{1}(:,TOT_CYC) d{1}(:,MODEL) ] );

corr(xy(:,1), xy(:,2))

scatter(xy(:,1), xy(:,2), 'b.');

z = line([0 12], [0 12]);
axis([0 12 0 12]);

output  = 'ix_sse2_model_seq';
saveas(h,['./eps/', output, '.eps'], 'epsc');
saveas(h,['./png/', output, '.png'], 'png');

%
% Scatter Vec
%
h = figure;
hold on;

xlabel('Actual', 'FontSize', 16);
ylabel('Predicted','FontSize', 16);

xy = filter_outliers([ d{2}(:,TOT_CYC) d{2}(:,MODEL) ] );

corr(xy(:,1), xy(:,2))

scatter(xy(:,1), xy(:,2), 'b.');

z = line([0 12], [0 12]);
axis([0 12 0 12]);

output  = 'ix_sse2_model_vec';
saveas(h,['./eps/', output, '.eps'], 'epsc');
saveas(h,['./png/', output, '.png'], 'png');

%
% Scatter Speedup
%
%h = figure;
%hold on;
%
%xlabel('Actual', 'FontSize', 16);
%ylabel('Predicted','FontSize', 16);
%
%xy = filter_outliers([ 
%  (d{1}(:,TOT_CYC) ./ d{2}(:,TOT_CYC)) ...
%  (d{1}(:,MODEL) ./ d{2}(:,MODEL)) ]);
%
%corr(xy(:,1), xy(:,2))
%
%scatter(xy(:,1), xy(:,2), 'b.');
%
%axis([1 2 1 2]);
%z = line([1 2], [1 2]);
%
%output  = 'ix_sse2_speedup_model';
%saveas(h,['./eps/', output, '.eps'], 'epsc');
%saveas(h,['./png/', output, '.png'], 'png');
%
%
% Error Seq
%
h = figure;
hold on;

xlabel('Error', 'FontSize', 16);
ylabel('Counts','FontSize', 16);

xy = filter_outliers([ d{1}(:,TOT_CYC) d{1}(:,MODEL) ] );

corr(xy(:,1), xy(:,2))

hist( (xy(:,1) - xy(:,2)) ./ xy(:,1), 50)

axis([-0.8 0.8  0 120 ]);

output  = 'ix_sse2_model_seq_err';
saveas(h,['./eps/', output, '.eps'], 'epsc');
saveas(h,['./png/', output, '.png'], 'png');

%
% Error Vec
%
h = figure;
hold on;

xlabel('Error', 'FontSize', 16);
ylabel('Counts','FontSize', 16);

xy = filter_outliers([ d{2}(:,TOT_CYC) d{2}(:,MODEL) ] );

corr(xy(:,1), xy(:,2))

hist( (xy(:,1) - xy(:,2)) ./ xy(:,1), 50)

axis([-0.8 0.8  0 120 ]);

output  = 'ix_sse2_model_vec_err';
saveas(h,['./eps/', output, '.eps'], 'epsc');
saveas(h,['./png/', output, '.png'], 'png');

%
% Error Speedup
%
%h = figure;
%hold on;
%
%xlabel('Error', 'FontSize', 16);
%ylabel('Counts','FontSize', 16);
%
%xy = filter_outliers([ 
%  (d{1}(:,TOT_CYC) ./ d{2}(:,TOT_CYC)) ...
%  (d{1}(:,MODEL) ./ d{2}(:,MODEL)) ]);
%
%hist( (xy(:,1) - xy(:,2)) ./ xy(:,1), 50)
%
%axis([-0.8 0.8  0 120 ]);
%
%output  = 'ix_sse2_speedup_model_err';
%saveas(h,['./eps/', output, '.eps'], 'epsc');
%saveas(h,['./png/', output, '.png'], 'png');
