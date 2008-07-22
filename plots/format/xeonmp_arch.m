% NOTE: MOVSS R,R is an instruction but ICC did not use them on this code

global LAT THP NUM;

global BETA;

global PREFIX;

global L1_A L1_B;

LAT = {};
THP = {};
NUM = {};

LAT{ADDSS} = 3.00;
LAT{SUBSS} = 3.00;
LAT{MOVSSM} = (2.54 + 2.53) ./ 2; % Combine load and store
LAT{ADDPS} = 3.01;
LAT{SUBPS} = 3.01;
LAT{UNPCKHPS} = 2.00;
LAT{UNPCKLPS} = 2.01;
LAT{SHUFPS} = 4.00; 
LAT{MOVAPS} = 1.01;
LAT{MOVAPSM} = (2.54 + 2.53) ./ 2; % Combine load and store
LAT{MOVHPSM} = (4.00 + 4.00) ./ 2;
LAT{MOVLPSM} = (3.00 + 3.01) ./ 2;
LAT{MOVHLPS} = 1.0;
LAT{MOVLHPS} = 1.0;
LAT{ADD} = 1.0;
LAT{SUB} = 1.0;
LAT{MOV} = 1.0;
LAT{IMUL} = 5.0;
LAT{XOR} = 1.0;
LAT{SHL} = 0.51;
LAT{MOVM} = 2.53; % Combine load and store

THP{ADDSS} = 1.00;
THP{SUBSS} = 1.00;
THP{MOVSSM} = (1.00 + 1.00) ./ 2; % Combine load and store
THP{ADDPS} = 1.00;
THP{SUBPS} = 1.00;
THP{UNPCKHPS} = 1.01;
THP{UNPCKLPS} = 1.01;
THP{SHUFPS}  = 1.01; 
THP{MOVAPS} = 0.51;
THP{MOVAPSM} = 1.0;
THP{MOVHPSM} = (1.00 + 1.01) ./ 2;
THP{MOVLPSM} = (1.00 + 1.01) ./ 2;
THP{MOVHLPS} = 1.0;
THP{MOVLHPS} = 1.0;
THP{ADD} = 1.0;
THP{SUB} = 1.0;
THP{MOV} = 1.0;
THP{IMUL} = 5.0;
THP{XOR} = 1.0;
THP{SHL} = 0.50;
THP{MOVM} = (1.01 + 1.00) ./ 2; % Combine load and store

% "14 stage efficient pipeline" in manual

NUM{ADDSS} = 14.0;
NUM{SUBSS} = 14.0;
NUM{MOVSSM} = 14.0; 
NUM{ADDPS} = 14.0;
NUM{SUBPS} = 14.0;
NUM{UNPCKHPS} = 14.0;
NUM{UNPCKLPS} = 14.0;
NUM{SHUFPS}  = 14.0; 
NUM{MOVAPS} = 14.0;
NUM{MOVAPSM} = 14.0;
NUM{MOVHPSM} = 14.0;
NUM{MOVLPSM} = 14.0;
NUM{MOVHLPS} = 14.0;
NUM{MOVLHPS} = 14.0;
NUM{ADD} = 14.0;
NUM{SUB} = 14.0;
NUM{MOV} = 14.0;
NUM{IMUL} = 14.0;
NUM{XOR} = 14.0;
NUM{SHL} = 14.0;
NUM{MOVM} = 14.0; 

BETA = [ 3.0 14.0 ]; % Got these from the manual

L1_A = 13;
L1_B = 20;

PREFIX = 'xeonmp';

