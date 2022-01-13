#include "declare.h"
typedef struct l_node L_NODE;
typedef struct fin_node FIN_NODE;
typedef struct flt_node FLT_NODE;
typedef struct dinff DINFF;
typedef struct t_node T_NODE;
typedef struct scan_chain SCAN_CHAIN;
typedef struct FF_Pat_Flt_List FF_PAT_FLT_LIST;

typedef int CHGATA;           /* int when debugging, char otherwise */
typedef unsigned int BITGATA; /* sort of memorry used unit of bit */

//2進数でパターンごとにキャプチャ毎に故障を記録するため構造体_2015710_王
//typedef struct lfsr LFSR;
struct l_node
{
  int line, type, innum, outnum, OBTimes, addgateflag;
  //toggle gate insertion
  int cp_flag;
  int toggle_cap[MAXCAP];//toggle number for capture power computing
  float toggle_rate[MAXCAP];//toggle rate for power computing
  /*unsigned int gdval1_cap[MAXCAP];
  unsigned int ftval1_cap[MAXCAP];
  unsigned int gdval1_br_cap[MAXCAP];
  unsigned int ftval1_br_cap[MAXCAP];
  */
  //
  L_NODE *next, *prev;
  FIN_NODE *finlst, *foutlst;
  unsigned int gdval0, gdval1, gdval_slow, ftval0, ftval1, ftval_slow;
  unsigned int tff_org_gval, tff_trans_gval,tff_trans_fval;

  #if TRANSITIONFAULT
  unsigned int la_val;
  //unsigned int tran_gdval[MAXCAP];
  //unsigned int tran_ftval[MAXCAP];
  #endif

  char *Name;
  int sel_flag[20]; //FF選択法
};

struct fin_node
{
  L_NODE *node;
  FIN_NODE *next;
};

struct flt_node
{
  int saval, typeflag, num, line;
  L_NODE *back, *forwd;
  FLT_NODE *next, *prev;
  DINFF *dfflst;
  unsigned int detect[MAXLEN / 32 + 1], activ[MAXLEN / 32 + 1];
  int dtime, TranDetTimes;
};
struct dinff
{
  L_NODE *node;
  unsigned int val;
  DINFF *next;
};

struct t_node
{
  unsigned int gdval1, ftval1;
  int DetFaultsNum;
  //int DetFaults[20000];
};

struct scan_chain
{
  int length;
  int top;
  int lastval;
};

#if FAULTOB
struct FF_Pat_Flt_List
{ //パターン毎、FF毎に検出した故障情報を保存する構造体
  //int Total_Det_Faults;
  //int OB_Det_Faults;
  //int Last_Det_Faults;
  //unsigned int Cap_Faults[MAXCAP][MAXFLT/32+1];//パターン毎、FF毎、キャプチャ毎に検出故障
  // unsigned int Last_Cap_Faults[MAXFLT/32+1];//パターン毎、FF毎に最終のキャプチャで検出した故障
  // unsigned int OB_Cap_Faults[MAXFLT/32+1];//パターン毎、FF毎に中間キャプチャで検出した故障
  //unsigned int Total_Faults[MAXFLT/32+1];//パターン毎、FF毎に検出した故障の統計
  //unsigned int **Cap_Faults;//パターン毎、FF毎、キャプチャ毎に検出故障
  unsigned int *Last_Cap_Faults; //パターン毎、FF毎に最終のキャプチャで検出した故障
  unsigned int *OB_Cap_Faults;   //パターン毎、FF毎に中間キャプチャで検出した故障
  unsigned int *Total_Faults;    //パターン毎、FF毎に検出した故障の統計
};
FF_PAT_FLT_LIST **Pat_FF_Faults;
#endif

L_NODE gnode, inode;
FIN_NODE pinode, ponode, ffnode;
FLT_NODE fltlst, dropflt;
L_NODE add_gate[32], add_pi[32];
FIN_NODE add_fin[32][32], add_fout[32], pi_fout[32];
T_NODE **ff_observe;
T_NODE **po_observe;
SCAN_CHAIN *schain;


/* net list */
typedef struct element
{
  int type, nfi, nfo, line;
  int fil;
  int fol;
  /* char name[30]; */
} ELEMENT;

/*Instance Variable Defination*/
int MODE_TOOL, TPG_MODE, TAP_NUM, cap_freq, SKIP_CAPTURE, SKIP_CYCLE, FF_SEL_METHOD, CP_CTR_MODE, CP_TYPE, clocktime;
int LBISTMODE, ALPMODE, INTERVAL_CYCLE, TG_FILE, SOControlflag, SoControlMode, FFSelMode, CHAINLENGTH, length,group_tpi;
int numout, slist, numgate, n_tpi;
int lpnt, inpnum, ffnum, sum_flt, remain_flt, sum_Tran_flt, chainnum, tgl_gt_cnt;
int **tgl_tpi;
int flt_cap[MAXCAP];

float Tgl_rate;
float ff_rate;
#if POWEREVA
long double toggle_scn_max, toggle_cap_max, toggle_scn, toggle_scn_in, toggle_scn_out, toggle_cap[MAXCAP];
double toggle_shift_perpat, toggle_cap_perpat[MAXCAP];
double WSA[MAXCAP], MaxWSA;
float ShiftPeak[CHAINNUM];
#endif

char *ff_select;
float OBSERVE_RATE;
int *flt_det_num;
int **flt_det_flag;
//int flt_det_flag[MAXFLT][11];
int FF_FILE;
int toggle_gates[MAXGATE];
FILE *fout_in;
FILE *fout_flt_in;
