#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <sys/sysinfo.h>

/* signal value */
#define X 2
#define U 3
#define U0 4
#define U1 5

#define SEQEN 1
#define NAME 0

/* maximum limits */
#define MAXGATE 100000
#define MAXFLT 100000  /* maximum number of flist */
#define MAXLEN 1000000 /* maximum number of test length */
#define MAXCAP 12      /* maximum number of cycle capture */
//#define CHAINLENGTH 100	/* scan chain length */
#define CHAINNUM 20
#define LFSR_BIT 16 /* # of LFSR bit */

#define MAKE_REC 0 /* when 1, output fault coverage to 'fault coverage.log' */
#define DEBUG 0
//キャプチャ回数,flsrの状態などが表示される
#define DEBUG1 0
#define DEBUG2 0
#define DEBUG_NODE 0
#define FALSE 0
#define TRUE 1
#define PRN_FLT_INT 5
#define SAMPLE 0
#define ALL_F 0xffffffff
#define ITI 0x1
//#define TAP_NUM 5

/*expect toggle rate*/
#define TIMESCHEDULE 0 //1: yes 0:no
#define SPACECHANGE 0
#define EXPECTTOGGLERATE 0.20 //expected toggle rate
#define checktiming 0         // observe timing
#define lev0 0.50
#define lev1 0.18
#define lev2 0.08 // given toggle rate level
#define MASK 0xaaaa
/*expect toggle rate*/
#define EXTERNSEED 0
#define RESEED 0
#define RANDOMSEED 0
#define SEEDMEMORY 0
#define SEEDINTER 10

#define PRNT_DET_FLT 0 /* 1 when printing detected faults */
#define PRNT_PIVAL 0   /* 1 when printing PI values */
#define DALLOC (DINFF *)malloc(sizeof(DINFF))

#define INITIAL_ST 1   /* 0 when all 0, 1 when use LFSR */
#define PRNT_FF 0      /* 1 when printing FFs */
#define OUTPUT_FLIST 0 /* 1 when output fault list */

#define PO_OBSERVE 0    /* 1 when observe POs */
#define FF_OBSERVE 1     /* 1 when observe FFs while multi-capture マルチサイクルの時0*/
#define SELECT_STATION 1 /* 1 when select FF station 0: Full observation*/
// 0なら全観測・1なら部分観測
//#define OBSERVE_RATE 1 /* MUST 0 <, < 1 */

#define POWEREVA 0 //power evaluation トグル率の表示
#define WSA_REC 0  //WSA evaluation 検出率の表示
#define FF_CAP 0   //=1: evaluate the toggle rate at each FF

//#define INTERVAL_CYCLE 4 //=1:toggle at every cycle, =2:toggle at every 2 cycles, =3: toggle at every 3 cycles
#define PEAK 0 //Shift Peak power computing
//#define SWITCH_GATE 1
//#define LEFTSET 3
//#define STUCT_TGL_GT 1 //Control by external input toggle gate data
//#define tgl_rate 0.1
//#define TGL_GT_NUM 1
//#define GATE_GP_START 0.2
//#define GATE_GP_END	0.4

/*Faultsimulation control*/
#define FAULTSIMULATION 1 /*for observing scanout: 0: stop fault simulation*/
#define TRANSITIONFAULT 1 // operate transition faults detection
#define L_CK 1
#define FAULTDROP 1       // 1: fault droping simulation 0: Full fault sim
#define FCOVPERPAT 1
#define FAULTOB 0   //Observation Fault Detection Infomation
#define BITRECORD 0 //2進数でパターンごと、FFごと、キャプチャごとの故障情報を記録する＿＿2015710_王
#define NORMAL_REC 0
#define PAT_FCOV 0
#define FLT_PER_PAT 0 //パターン毎の故障検出情報を取るための変数
//全観測のとき1に部分観測のとき0にする
/*if SELECT_STATION=1, FAULTOB !=1 */
#define Transcycle 1
#define FIXVALUE 0
#define SEL_OB_FF_FLT 0

#define FLT_OUTPUT 0
#define FLTLIST_PAT 0

#define SCTEST 1 //SCAN Test
#define MULTITEST 2 // Multi-cycle test
#define MULTI_OP 3 // multi-cycle test with Seq-OB
#define MULTI_CP 4 // Multi-cycle test with CPI

#define FCP_TOG 2 //FF CPI by toggling
#define FCP_RAN 3 //FF CPI by Random load
#define LCP_TOG 1 // Logic CPI by toggling
#define LCP_RAN 4 //Logic CPI by Random load

#define CP_TDT 1 // transition driven toggle CP
#define CP_INV 2 // inversion CP by EX_OR
#define CP_JST 3 // Just Toggle CP

#if FLTLIST_PAT
FILE *FLST_OUT;
#endif


int flt_list[1000000][2];
int test_val[10][30];
