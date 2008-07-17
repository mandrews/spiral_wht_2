global PREC;
global RDTSC TOT_INS L1_DCM L2_DCM;
global VECTOR_SIZE;

PREC = 'single';
VECTOR_SIZE = 4;

ID = 1;
RDTSC = 2;
TOT_INS = 3;
L1_DCM = 4;
L2_DCM = 5;
ADDSS = 6;
SUBSS = 7;
ADDPS = 8;
SUBPS = 9;
SHUFPS = 10;
UNPCKHPS = 11;
UNPCKLPS = 12;
MOVAPS = 13;
MOVHLPS = 14;
MOVLHPS = 15;
SHL = 16;
XOR = 17;
MOV = 18;
PUSH = 19;
POP = 20;
NOP = 21;
RETQ = 22;
ADD = 23;
SUB = 24;
IMUL = 25;
MOVSSM = 26;
MOVAPSM = 27;
MOVHPSM = 28;
MOVLPSM = 29;
LEAM = 30;
MOVM = 31;
% NOTE: These are reversed and should be fixed in script/common.sh
IC_ALL = 32;
IC_PAPI = 33;
CMC1 = 34;
CMC2 = 35;
LEFT = 36;
RIGHT = 37;
BASE = 38;

% NOTE: This should be moved to a xeonmp.m definition file

ADDSS_LAT = 3.00;
SUBSS_LAT = 3.00;
MOVSS_LAT = 1.01;
MOVSSM_LAT = (2.54 + 2.53) ./ 2; % Combine load and store
ADDPS_LAT = 3.01;
SUBPS_LAT = 3.01;
UNPCKHPS_LAT = 2.00;
UNPCKLPS_LAT = 2.01;
SHUFPS_LAT = 4.00; 
MOVAPS_LAT = 1.01;
MOVAPSM_LAT = (2.54 + 2.53) ./ 2; % Combine load and store
MOVHPSM_LAT = (4.00 + 4.00) ./ 2;
MOVLPSM_LAT = (3.00 + 3.01) ./ 2;

ADDSS_THP = 1.00;
SUBSS_THP = 1.00;
MOVSS_THP = 0.51;
MOVSSM_THP = (2.54 + 2.53) ./ 2; % Combine load and store
ADDPS_THP = 1.00;
SUBPS_THP = 1.00;
UNPCKHPS_THP = 1.01;
UNPCKLPS_THP = 1.01;
SHUFPS_THP  = 1.01; 
MOVAPS_THP = 0.51;
MOVAPSM_THP = 1.0;
MOVHPSM_THP = (1.00 + 1.01) ./ 2;
MOVLPSM_THP = (1.00 + 1.01) ./ 2;

% NOTE: This should be moved to a xeonmp_icc_10_15.m definition file

ADDSS_NUM = 14.0;
SUBSS_NUM = 14.0;
MOVSS_NUM = 0.0;
MOVSSM_NUM = 14.0; 
ADDPS_NUM = 14.0;
SUBPS_NUM = 14.0;
UNPCKHPS_NUM = 14.0;
UNPCKLPS_NUM = 14.0;
SHUFPS_NUM  = 14.0; 
MOVAPS_NUM = 14.0;
MOVAPSM_NUM = 14.0;
MOVHPSM_NUM = 14.0;
MOVLPSM_NUM = 14.0;

