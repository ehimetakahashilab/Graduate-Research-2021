#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"
#include "string.h"
#include "math.h"
#define FLT_PRN 0 // output the detected fault list

main(argc, argv) int argc;
char *argv[13];
{
  struct tms cputime;
  int totime, ia, ib, ic, id;
  int remain_flt;
  time_t time_1, time_2;
  char path_last[256], path_middle[256], name[256], path_toggle[256];
  FILE *fout;

  Instance_Get(argc, argv); //shellからツールモード、回路テーブルパス、キャプチャ回数などのargvを読み込む
  make_line_list(argv);//回路テーブル読み込む、回路リスト作成、故障リスト作成、
  sort_node();//回路のノードリストソーティング、故障リストソーティング
  err_check(fltlst.next, 0);//故障リストが正しく作られているかをチェックする

  //flt_det_flag[][]:異なる中間観測ＦＦ選定手法で選んだＦＦを観測する場合の故障検出数の情報を保存するための配列
  //flt_det_flag[a][b]: a:故障番号、b:観測ＦＦ選定手法、b=10: 全観測の場合の結果
  flt_det_flag = (int **)calloc((sum_flt + 2), sizeof(int *));
  flt_det_num = (int *)calloc(FF_FILE+2, sizeof(int));

  if (flt_det_flag == NULL)
    printf("memory error @flt_det_flag in flt_info \n"), exit(1);
if (flt_det_num == NULL)
      printf("memory error @flt_det_num in flt_info \n"), exit(1);
  for (ia = 0; ia <= sum_flt + 1; ia++)	{
    if(MODE_TOOL==1 || MODE_TOOL==2)   {
        flt_det_flag[ia] = (int *)calloc(2, sizeof(int));
      }
    else{
      flt_det_flag[ia] = (int *)calloc(FF_FILE+2, sizeof(int));
    }
    if (flt_det_flag[ia] == NULL)  printf("memory error @flt_det_flag \n"), exit(1);
  }

#if FLT_OUTPUT
//output the fault list to file
  printf("\nOutput Fault List\n");
  flt_out(fltlst.next, argv);
  exit(1);
#endif

#if DEBUG_NODE
  prn_fltlst(fltlst.next); //故障リストの情報を出力する
//  prn_fltlst_format(fltlst.next);//故障リストの情報を出力する
  prn_node(gnode.next);//ゲートnodeリストの情報を出力する
  prn_subnode(pinode.next);//外部入力PIノード
  prn_subnode(ffnode.next);//FFノード
  prn_subnode(ponode.next);//外部出力POノード
  #if TRANSITIONFAULT
  printf(" === Sum of Transition Faults %d (ko) ===\n", sum_flt);
  #else
  printf(" === Sum of Stuck-at Faults %d (ko) ===\n", sum_flt);
  #endif
#endif

 //fltlist_print(fltlst.next);exit(1);
  times(&cputime);
  totime = cputime.tms_utime;//シミュレーション時間計測
  time_1=time((time_t *)0);

  faultsim(argv); //論理・故障シミュレーション
//  fltlist_print(fltlst.next);
  Out_Put(argv);//シミュレーション結果出力
  times(&cputime);
  totime = cputime.tms_utime - totime;

  #if FLT_PRN
  prn_flt(flt_det_flag,argv);
  #endif

for (ia = 0; ia <= sum_flt + 1; ia++)
  free(flt_det_flag[ia]);
  free(flt_det_flag);
  free(flt_det_num);
  printf(" ***  cputime = %.3f[sec] *** \n\n", (double)totime / 100);
  time_2=time((time_t *)0);
 printf(" CPU-time %.2f(sec)\n",difftime(time_2,time_1));
}


count_flt(flttag)
    FLT_NODE *flttag;
{
  int sumflt = 0, sumTDflt = 0;
  int ia, ib;
  int cnt=0;
/*#if TRANSITIONFAULT
  for (; flttag != NULL; flttag = flttag->next){
    if (!flttag->TranDetTimes)
      sumTDflt++;
  }
  return sumTDflt;
#else*/
#if SELECT_STATION
  for (ia = 0; ia <=FF_FILE+1; ia++){
    flt_det_num[ia] = 0;
  }
#endif

  for (; flttag != NULL; flttag = flttag->next)
  {cnt++;
    if (!flttag->dtime){
      sumflt++;
    }
  }
  printf("cnt=%d, sumflt=%d\n",cnt,sumflt);
  if ( MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP){
    for (ia = 0; ia <= sum_flt; ia++) {
      for (ib = 0; ib <= FF_FILE; ib++){
        if (flt_det_flag[ia][ib]) flt_det_num[ib]++;
      }
      if (flt_det_flag[ia][FF_FILE+1])  flt_det_num[FF_FILE+1]++;
    }
  }
  return sumflt;
//#endif
}

void prn_flt(flt_table,argv)
int **flt_table;
char *argv[1];
{
  FILE *flist;
  char file_name[100];
  sprintf(file_name,"flt_det_list_%s.csv",argv[1]);
  flist = fopen(file_name, "a");
  if(flist == NULL) {
    printf("cannot open the detected fault list \n");
    exit(1);
  }
  printf("\noutput the fault detection table \n");
  printf("#fault, nodft,");
  int ia,ib;
  fprintf(flist, "%d\n",sum_flt );
  if ( MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP){
    for(ia=0;ia<FF_FILE;ia++) printf("op%d,",ia+1);
      printf("full_op");
  }
  printf("\n");
  for(ia=1;ia<=sum_flt;ia++){
    printf("%d,",ia);
    fprintf(flist, "%d,",ia);
    if ( MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP){
      for(ib=0;ib<=FF_FILE+1;ib++)  {
        printf("%d,",flt_det_flag[ia][ib]);
        fprintf(flist, "%d,",flt_det_flag[ia][ib]);
      }
      printf("\n");
      fprintf(flist, "\n");
    }
    else{
      printf("%d,\n",flt_det_flag[ia][0]);
      fprintf(flist, "%d,\n",flt_det_flag[ia][0]);
    }
  }
  fclose(flist);
}

fltlist_print(flttag)
    FLT_NODE *flttag;
{
  int sumflt = 0, sumTDflt = 0;
  int ia, ib;
  printf("\noutput the fault detection table \n");
  printf("#fault, nodft,");
  if ( MODE_TOOL == 3 || MODE_TOOL == 4){
    for(ia=0;ia<FF_FILE;ia++)
    printf("op%d,",ia+1);
    printf("full_op");
  }
  printf("\n");


for(; flttag != NULL; flttag = flttag->next){
  printf("%d,", flttag->num);
//#if TRANSITIONFAULT
//  if (!flttag->TranDetTimes) printf("0,");
//  else printf("1,");

//#else
  if ( !flttag->dtime ) printf("0,");
  else printf("1,");

  if ( MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP){
    for (ia = 0; ia <= sum_flt; ia++){
      for (ib = 0; ib < FF_FILE; ib++)
      {
        if (flt_det_flag[ia][ib]) printf("1,");
        else printf("0,");
      }
    if (flt_det_flag[ia][10]) printf("1,");//全観測
    else printf("0,");
    }
  }
  printf("\n");
//#endif
  }
}

//縮退故障リストをチェックする、全体の縮退故障数を返し
saf_list_check(flttag)
    FLT_NODE *flttag;
{
  int count = 0;
  for (; flttag != NULL; flttag = flttag->next){
    count++;
  }
  return count;
}
//故障リストが正しく作られているかをチェックする
err_check(flttag, time)
    FLT_NODE *flttag;
int time;
{
  while (flttag != NULL)
  {
    if (flttag->dfflst != NULL)
    {
      printf(" error fault %d  time %d\n", flttag->back->line, time);
      exit(1);
    }
    flttag = flttag->next;
  }
}
#if FLT_OUTPUT
//個々ＦＦで検出した故障リストを作る
fault_det_cap(fflist, totalflts, argv)
int fflist[ffnum][40000];//FF毎に検出した故障情報を保存する配列
int totalflts;//故障総数
char *argv[10];
{
  int ia, ib, ic, id, flag;
  float FF_TCov[ffnum], FF_Only_Flt[ffnum];
  FILE *FF_flt_list;
  for (ia = 0; ia < ffnum; ia++)
  {
    FF_Only_Flt[ia] = 0.0;
    FF_TCov[ia] = (float)fflist[ia][0] / totalflts * 100;
    for (ib = 1; ib <= fflist[ia][0]; ib++)
    {
      flag = 0;
      for (ic = 0; ic < ffnum; ic++)
      {
        if (ia == ic)
          continue;
        for (id = 1; id <= fflist[ic][0]; id++)
        {
          if (fflist[ia][ib] == fflist[ic][id])
          {
            flag = 1;
            break;
          }
        }
        if (flag == 1)
          break;
      }
    }

    if (flag == 0)
      FF_Only_Flt[ia]++;
  }
  FF_flt_list = fopen("TCov_of_FFs.txt", "a");
  fprintf(FF_flt_list, "%s  %s  %s\n", argv[1], argv[3], argv[4]);
  for (ia = 0; ia < ffnum; ia++)
    fprintf(FF_flt_list, "%4.3f ", FF_TCov[ia]);
  fprintf(FF_flt_list, "\n");
  for (ia = 0; ia < ffnum; ia++)
    fprintf(FF_flt_list, "%d ", FF_Only_Flt[ia]);
  fprintf(FF_flt_list, "\n");

  fclose(FF_flt_list);
}

flt_out(flttag, argv)
    FLT_NODE *flttag;
char *argv[11];
{
  int sumflt = 0, sumTDflt = 0;
  FILE *flstout;
  char outfile[30];
  sprintf(outfile, "./FLT_LIST/%s", argv[1]);

  if (NULL == (flstout = fopen(outfile, "a")))
    printf(" error fault list file does not exit!\n"), exit(0);

  fprintf(flstout, "%d \n", sum_flt);
  int ie = 0;

  for (; flttag != NULL; flttag = flttag->next)
  {
    flt_list[ie][0] = flttag->line;
    flt_list[ie][1] = flttag->saval;
    if (flttag->forwd != NULL)
      fprintf(flstout, "%d\n", flttag->num);
    else
      fprintf(flstout, "%d\n", flttag->num);
    ie++;
  }
  fclose(flstout);
}
#endif
