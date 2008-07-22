% NOTE: MOVSS R,R is an instruction but GCC did not use them on this code

global LAT THP NUM;

global BETA;

global PREFIX;

global L1_A L1_B;

LAT = {};
THP = {};
NUM = {};

LAT{ADDSS} = 4.00;
LAT{SUBSS} = 4.06;
LAT{MOVSSM} = (3.52 + 3.53) ./ 2; % Combine load and store
LAT{ADDPS} = 4.01;
LAT{SUBPS} = 4.02;
LAT{UNPCKHPS} = 3.02;
LAT{UNPCKLPS} = 3.01;
LAT{SHUFPS} = 2.51; 
LAT{MOVAPS} = 2.01;
LAT{MOVAPSM} = (4.21 + 4.18) ./ 2; % Combine load and store
LAT{MOVHPSM} = (3.53 + 4.00) ./ 2;
LAT{MOVLPSM} = (3.55 + 3.52) ./ 2;
LAT{MOVHLPS} = 0.52;
LAT{MOVLHPS} = 0.52;
LAT{ADD} = 1.02;
LAT{SUB} = 1.01;
LAT{MOV} = 1.02;
LAT{IMUL} = 4.02;
LAT{XOR} = 1.02;
LAT{SHL} = 0.36;
LAT{MOVM} = ( 2.15 + 2.66) ./ 2; % Combine load and store

THP{ADDSS} = 1.00;
THP{SUBSS} = 1.00;
THP{MOVSSM} = (1.00 + 1.34) ./ 2; % Combine load and store
THP{ADDPS} = 2.01;
THP{SUBPS} = 2.00;
THP{UNPCKHPS} = 2.00;
THP{UNPCKLPS} = 2.00;
THP{SHUFPS}  = 2.50; 
THP{MOVAPS} = 1.00;
THP{MOVAPSM} = 2.0;
THP{MOVHPSM} = 1.0;
THP{MOVLPSM} = 1.0;
THP{MOVHLPS} = 0.5;
THP{MOVLHPS} = 0.5;
THP{ADD} = 1.0;
THP{SUB} = 1.0;
THP{MOV} = 1.0;
THP{IMUL} = 4.0;
THP{XOR} = 1.0;
THP{SHL} = 0.34;
THP{MOVM} = 0.51;

% Presentation states:
% 12 stage integer pipeline
% 17 stage fp pipeline
% http://www.dynamore.de/download/eu03/papers/K-I/LS-DYNA_ULM_K-I-43.pdf

NUM{ADDSS} = 17.0;
NUM{SUBSS} = 17.0;
NUM{MOVSSM} = 17.0; 
NUM{ADDPS} = 17.0;
NUM{SUBPS} = 17.0;
NUM{UNPCKHPS} = 17.0;
NUM{UNPCKLPS} = 17.0;
NUM{SHUFPS}  = 17.0; 
NUM{MOVAPS} = 17.0;
NUM{MOVAPSM} = 17.0;
NUM{MOVHPSM} = 17.0;
NUM{MOVLPSM} = 17.0;
NUM{MOVHLPS} = 17.0;
NUM{MOVLHPS} = 17.0;
NUM{ADD} = 12.0;
NUM{SUB} = 12.0;
NUM{MOV} = 12.0;
NUM{IMUL} = 12.0;
NUM{XOR} = 12.0;
NUM{SHL} = 12.0;
NUM{MOVM} = 12.0; 

% This article states that L2 latency is 8.9 ns * 1.8 GHz = 16.02 cycles
% http://www.anandtech.com/printarticle.aspx?i=1816

% Manual states L1 latency is 3.0
% http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/23932.pdf
BETA = [ 3.0 16.0 ]; 

L1_A = 11;  % Real number is 13 but this was too narrow a range
L1_B = 17;

PREFIX = 'opteron';

