global PREC;

global RDTSC TOT_INS L1_DCM L2_DCM;

global ADDSS SUBSS ADDPS SUBPS SHUFPS UNPCKHPS UNPCKLPS MOVAPS MOVHLPS MOVLHPS SHL XOR MOV PUSH POP NOP RETQ ADD SUB IMUL MOVSSM MOVAPSM MOVHPSM MOVLPSM LEAM MOVM;

global IC_ALL IC_PAPI CMC1 CMC2 LEFT RIGHT BASE IC_OVR;

global SPLIT_ALPHA SPLIT_BETA_1 SPLIT_BETA_2 SPLIT_BETA_3 SPLITIL_ALPHA SPLITIL_BETA_1 SPLITIL_BETA_2 SPLITIL_BETA_3 SPLITV_ALPHA SPLITV_BETA_1 SPLITV_BETA_2 SPLITV_BETA_3;

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

IC_PAPI = 32;
IC_ALL = 33;
CMC1 = 34;
CMC2 = 35;
LEFT = 36;
RIGHT = 37;
BASE = 38;

SPLIT_ALPHA = 39;
SPLIT_BETA_1 = 40;
SPLIT_BETA_2 = 41;
SPLIT_BETA_3 = 42;
SPLITIL_ALPHA = 43;
SPLITIL_BETA_1 = 44;
SPLITIL_BETA_2 = 45;
SPLITIL_BETA_3 = 46;
SPLITV_ALPHA = 47;
SPLITV_BETA_1 = 48;
SPLITV_BETA_2 = 49;
SPLITV_BETA_3 = 50;

IC_OVR = 51; % Loop overhead as opposed to codelet instructions