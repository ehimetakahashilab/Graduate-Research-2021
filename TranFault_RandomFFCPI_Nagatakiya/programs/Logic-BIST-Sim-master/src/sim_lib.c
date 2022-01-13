#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"
#include <math.h>

int flt_det_cap[MAXCAP];

pi_valset(pivalset) int pivalset[];
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  unsigned int val;
  int line, cnt, ni;

  finnode = pinode.next;
  for (ni = 1; finnode != NULL; finnode = finnode->next, ni++)
  {
    fnode = finnode->node;
    line = fnode->line;

    val = pivalset[ni];

    if (val == 1)
    {
      fnode->gdval1 = ALL_F;
      // fnode->gdval0 = ALL_F;
    }
    else if (val == 0)
    {
      fnode->gdval1 = 0;
      //fnode->gdval0 = 0;
    }
    else if (val == X)
    {
      printf("error: Not support X value\n");
      exit(1);
    }
    else
      printf(" error 3982\n"), exit(1);
    fnode->ftval1 = fnode->gdval1;
    //  fnode->ftval0 = fnode->gdval0 ;
    //printf("%x %d %d\n", fnode->ftval1, fnode->line, ni);
  }
#if PRNT_PIVAL
  finnode = pinode.next;
  printf(" INPUT VECTOR ");
  for (cnt = 1; finnode != NULL; finnode = finnode->next, cnt++)
  {
    fnode = finnode->node;
    if (fnode->gdval1 == 0)
      printf("0");
    else
      printf("1");
    if (cnt % 30 == 0)
      printf("\n");
    else if (cnt % 10 == 0)
      printf(" ");
    else if (cnt % 5 == 0)
      printf(" ");
  }
  printf("\n");
#endif
}


update_nextstate_ff_inv_cp(capture) int capture;
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  int ia;
  unsigned int tmp[ffnum];
  unsigned int val1;

  finnode = ffnode.next;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    tmp[ia] = fnode->finlst->node->gdval1;
  }
  finnode = ffnode.next;

  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    if (fnode->gdval1 != tmp[ia])
    {
      fnode->gdval1 = tmp[ia];
#if POWEREVA
      toggle_cap[capture]++;
      toggle_cap_perpat[capture]++;
#if FF_CAP
      fnode->toggle_cap[capture]++;
#endif

#endif
    }
    else  {
      if (capture < SKIP_CYCLE  || capture >= cap_freq) continue;
      if (fnode->cp_flag != 1) continue;

          //if (capture >= SKIP_CYCLE - 1)
      if (capture % INTERVAL_CYCLE == 0){
                //if(fnode->gdval0==fnode->gdval1)
                fnode->gdval1 = ~fnode->gdval1;
                //fnode->gdval1= ~fnode->gdval1;

            }

    }

#if TRANSITIONFAULT
    if (capture == L_CK)
      fnode->gdval_slow = fnode->gdval1;
#endif
  }

#if DEBUG
  //for(ia=1;ia<=capture;ia++)
  printf("toggle_cap %d = %.0f %.0f\n", capture, toggle_cap[capture], toggle_cap_perpat[capture]);
#endif
}

update_nextstate(capture) int capture;
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  int ia;
  unsigned int tmp[ffnum];

  finnode = ffnode.next;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    tmp[ia] = fnode->finlst->node->gdval1;
  //  if(fnode->line==202 ||fnode->line==201) printf("ff-%d= %x,%x\n",fnode->line,fnode->gdval1,fnode->finlst->node->gdval1);
  }

  finnode = ffnode.next;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    if (fnode->gdval1 != tmp[ia])
    {// printf("%d,%d \n",ia, fnode->line);
      fnode->gdval1 = tmp[ia];
#if POWEREVA
      toggle_cap[capture]++;
      toggle_cap_perpat[capture]++;
#if FF_CAP
      fnode->toggle_cap[capture]++;
#endif

#endif
    }
#if TRANSITIONFAULT
    if (capture == L_CK)
      fnode->gdval_slow = fnode->gdval1;
#endif
//if(fnode->line==202 ||fnode->line==201) printf("ff-%d= %x,%x\n",fnode->line,fnode->gdval1,fnode->finlst->node->gdval1);

  }
//  exit(1);
}

tpi_ff_state_load_rand(capture) int capture;
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  int ia, tpi_cnt;
  //FF_TPI_by LFSR
  finnode = ffnode.next;
  tpi_cnt = 0;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    if (fnode->cp_flag == 1)
    {
      //  printf(" %d: %x %x\n", fnode->line, fnode->gdval0, fnode->gdval1);
      if (tgl_tpi[tpi_cnt][capture] == 1)
        fnode->gdval1 = ALL_F;
      else if (tgl_tpi[tpi_cnt][capture] == 0)
        fnode->gdval1 = 0;
      else if (tgl_tpi[tpi_cnt][capture] == X)
        printf("error: Not support X value\n"), exit(1);
      tpi_cnt++; //printf("herer?%d %d \n",tpi_cnt,capture);
    }
    //printf("out%d %d \n",tpi_cnt,capture);
  }

  //printf("out%d %d \n",tpi_cnt,capture);
}

tpi_ff_state_load_rand_ft(capture) int capture;
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  int ia, tpi_cnt;
  //FF_TPI_by LFSR
  finnode = ffnode.next;
  tpi_cnt = 0;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    if (fnode->cp_flag != 1)
      continue;
    if (tgl_tpi[tpi_cnt][capture] == 1)
      fnode->ftval1 = ALL_F;
    else if (tgl_tpi[tpi_cnt][capture] == 0)
      fnode->ftval1 = 0;
    else if (tgl_tpi[tpi_cnt][capture] == X)
      printf("error: Not support X value\n"), exit(1);
    tpi_cnt++;
  }
}

tpi_ff_state_load_tog(capture) int capture;
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  int ia, tpi_cnt;
  //FF_TPI_by LFSR
  finnode = ffnode.next;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    if (fnode->cp_flag == 1)
    {
      //  printf(" %d: %x %x\n", fnode->line, fnode->gdval0, fnode->gdval1);
        fnode->gdval1 = ~fnode->gdval1;
    }
    //printf("out%d %d \n",tpi_cnt,capture);
  }

}

tpi_ff_state_load_tog_ft(capture) int capture;
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  int ia, tpi_cnt;
  //FF_TPI_by LFSR
  finnode = ffnode.next;
  tpi_cnt = 0;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    if (fnode->cp_flag != 1)
      continue;
      fnode->ftval1 = ~fnode->ftval1;
  }
}

update_nextstate_ft_ff_inv_cp(capture) int capture;
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  int ia;
  unsigned int tmp[ffnum], tgl_val;
  int count = 0;
  finnode = ffnode.next;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    tmp[ia] = fnode->finlst->node->ftval1;
  }
  finnode = ffnode.next;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    tgl_val = fnode->ftval1 ^ tmp[ia];
    fnode->ftval1 = tmp[ia];
    if (capture < SKIP_CYCLE  || capture == cap_freq) continue;
    if (capture % INTERVAL_CYCLE ) continue;
    if (fnode->cp_flag != 1) continue;
      //if(fnode->gdval0==fnode->gdval1)
    fnode->ftval1 = ~(tmp[ia] ^ tgl_val);
    //  printf(" %d: %x %x\n",fnode->line,fnode->gdval1,fnode->ftval1);
    }
}

update_nextstate_ft(capture) int capture;
{
  FIN_NODE *finnode;
  L_NODE *fnode;
  int ia;
  unsigned int tmp[ffnum];
  finnode = ffnode.next;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++){
    fnode = finnode->node;
    tmp[ia] = fnode->finlst->node->ftval1;
    //fnode->ftval1 = tmp[ia];
  //  fnode->ftval1 = fnode->finlst->node->ftval1;
  }
  finnode = ffnode.next;
  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    fnode->ftval1 = tmp[ia];
    }

}

initial_state(int ff_state[])
{
  L_NODE *fnode;
  FIN_NODE *finnode;
  int inval1;
#if INITIAL_ST == 0
  /** assume all 0 value **/
#if DEBUG_NODE
  printf(" Assume initial all 0 state\n");
#endif
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    fnode->gdval1 = 0;
  }
#endif
#if INITIAL_ST == 1
  /** assume LFSR value **/
#if DEBUG_NODE
  printf(" Assume initial RANDOM state\n");
#endif

  finnode = ffnode.next;
  int ia = 0;
  for (; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    if (ff_state[ia] == 1)
    {
      fnode->gdval1 = ALL_F;
    }
    else
    {
      fnode->gdval1 = 0;
    }
    //printf("%x %d\n", fnode->gdval1, fnode->line);
  }
#endif
}

initial_state_ft(int ff_state[])
{
  L_NODE *fnode;
  FIN_NODE *finnode;
  int inval1;
#if INITIAL_ST == 0
  /** assume all 0 value **/
#if DEBUG_NODE
  printf(" Assume initial all 0 state\n");
#endif
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    fnode->ftval1 = 0;
  }
#endif
#if INITIAL_ST == 1
  /** assume LFSR value **/
#if DEBUG_NODE
  printf(" Assume initial RANDOM state\n");
#endif

  finnode = ffnode.next;
  int ia = 0;
  for (; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    if (ff_state[ia] == 1)
    {
      fnode->ftval1 = ALL_F;
    }
    else
    {
      fnode->ftval1 = 0;
    }
  }
#endif
}

prn_allvalue(node_head)
    L_NODE *node_head;
{
  L_NODE *fnode;
  unsigned int val1;
  int numg;

  printf(" === LINES VALUES ===\n");
  fnode = node_head;
  numg = 0;
  for (; fnode != NULL; fnode = fnode->next)
  {
    numg++;

    val1 = fnode->gdval1;
    printf(" L[%d]= ", fnode->line);
    if (val1 == 0)
      printf("0");
    else if (val1 == ALL_F)
      printf("1");
    else
      printf(" error -28329-\n"), exit(1);
    /*     if(numg%5==0) */
    printf("\n");
  }
  printf("\t\t (good)\n");
}

prn_out_value(po_observe, ia)
    T_NODE po_observe[numout][cap_freq];
int ia;
{
  unsigned int val1;
  int ib;

  printf(" OUTPUT ");
  for (ib = 0; ib < numout; ib++)
  {
    val1 = po_observe[ib][ia - 1].gdval1;

    if (val1 == 0)
      printf("0");
    else if (val1 == ALL_F)
      printf("1");
    else
      printf("E");
    //printf(" error -28329-\n"),exit(1);
  }
  printf("\n");
}

prn_state(finnode)
    FIN_NODE *finnode;
{
  L_NODE *fnode;
  unsigned int val1;
  int numg = 0, cnt;
  //#if DEBUG1 || PRNT_FF
  printf(" FF ");
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    val1 = fnode->gdval1;
    if (val1 == 0)
      printf("0");
    else if (val1 == ALL_F)
      printf("1");
    else
      printf(" error -28329-\n"), exit(1);
  }
//#endif
#if DEBUG1
  printf(" STATE ");
  for (cnt = 1; finnode != NULL; finnode = finnode->next, cnt++)
  {
    fnode = finnode->node;
    if (fnode->gdval1 == 0)
      printf("0");
    else
      printf("1");
    if (cnt % 30 == 0)
      printf("\n\t");
    else if (cnt % 5 == 0)
      printf(" ");
  }
//exit(1);
#endif
  printf("\n");
}

prn_state_ao(fp, time)
    FILE *fp;
int time;
{
  L_NODE *fnode;
  unsigned int val1;
  int numg = 0, cnt;
  FIN_NODE *finnode;

  finnode = pinode.next;
  //printf("%d ", fnode1->line);
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    //fprintf(fp, "%d ", fnode->line);
    if (fnode->gdval1 == ALL_F)
    {
      test_val[time][fnode->line] = 1;
      // fnode->gdval0 = ALL_F;
    }
    else if (fnode->gdval1 == 0)
    {
      test_val[time][fnode->line] = 0;
    }
  }
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    //fprintf(fp, "%d ", fnode->line);
    if (fnode->gdval1 == ALL_F)
    {
      test_val[time][fnode->line] = 1;
    }
    else if (fnode->gdval1 == 0)
    {
      test_val[time][fnode->line] = 0;
    }
  }
  fnode = gnode.next;
  for (; fnode != NULL; fnode = fnode->next)
  {
    //fprintf(fp, "%d ", fnode->line);
    if (fnode->gdval1 == ALL_F)
    {
      test_val[time][fnode->line] = 1;
    }
    else if (fnode->gdval1 == 0)
    {
      test_val[time][fnode->line] = 0;
    }
  }
}

prn_state_ao3(fp, time)
    FILE *fp;
int time;
{
  L_NODE *fnode;
  unsigned int val1;
  int numg = 0, cnt;
  FIN_NODE *finnode;

  finnode = pinode.next;
  //printf("%d ", fnode1->line);
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    //fprintf(fp, "%d ", fnode->line);
    if (fnode->gdval1 == ALL_F)
    {
      fprintf(fp, "1,");
      // fnode->gdval0 = ALL_F;
    }
    else if (fnode->gdval1 == 0)
    {
      fprintf(fp, "0,");
    }
  }
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    //fprintf(fp, "%d ", fnode->line);
    if (fnode->gdval1 == ALL_F)
    {
      fprintf(fp, "1,");
    }
    else if (fnode->gdval1 == 0)
    {
      fprintf(fp, "0,");
    }
  }
  fprintf(fp, "\n");
}

prn_state_ao_ft(fp, time)
    FILE *fp;
int time;
{
  L_NODE *fnode;
  unsigned int val1;
  int numg = 0, cnt;
  FIN_NODE *finnode;

  finnode = pinode.next;
  //printf("%d ", fnode1->line);
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    //fprintf(fp, "%d ", fnode->line);
    if (fnode->ftval1 == ALL_F)
    {
      fprintf(fp, "%d  1,", test_val[time][fnode->line]);
      // fnode->gdval0 = ALL_F;
    }
    else if (fnode->ftval1 == 0)
    {
      fprintf(fp, "%d  0,", test_val[time][fnode->line]);
    }
  }
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    //fprintf(fp, "%d ", fnode->line);
    if (fnode->ftval1 == ALL_F)
    {
      fprintf(fp, "%d  1,", test_val[time][fnode->line]);
    }
    else if (fnode->ftval1 == 0)
    {
      fprintf(fp, "%d  0,", test_val[time][fnode->line]);
    }
    else
    {
      fprintf(fp, "%d  %x,", test_val[time][fnode->line], fnode->ftval1);
    }
  }
  fnode = gnode.next;
  for (; fnode != NULL; fnode = fnode->next)
  {
    //fprintf(fp, "%d ", fnode->line);
    if (fnode->line == 9 || fnode->line == 11 || fnode->line == 13 || fnode->line == 17 || fnode->line == 18 || fnode->line == 12 || fnode->line == 14 || fnode->line == 16 || fnode->line == 10 || fnode->line == 15 || fnode->line == 5 || fnode->line == 19)
    {
      if (fnode->ftval1 == ALL_F)
      {
        fprintf(fp, "%d  1,", test_val[time][fnode->line]);
      }
      else if (fnode->ftval1 == 0)
      {
        fprintf(fp, "%d  0,", test_val[time][fnode->line]);
      }
      else
      {
        fprintf(fp, "%d  %x,", test_val[time][fnode->line], fnode->ftval1);
      }
    }
  }
  fprintf(fp, "\n");
}

prn_state_ao2(fp)
    FILE *fp;
{
  L_NODE *fnode;
  unsigned int val1;
  int numg = 0, cnt;
  FIN_NODE *finnode;

  fprintf(fp, "gate,");
  finnode = pinode.next;
  //printf("%d ", fnode1->line);
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    fprintf(fp, "%d,", fnode->line);
  }
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    fprintf(fp, "%d,", fnode->line);
  }
  fnode = gnode.next;
  for (; fnode != NULL; fnode = fnode->next)
  {
    fprintf(fp, "%d,", fnode->line);
  }
  fprintf(fp, "\n");
}

prn_state_flt_ao(finnode)
    FIN_NODE *finnode;
{
  L_NODE *fnode;
  unsigned int val1;
  int numg = 0, cnt;
  FIN_NODE *finnode1;

  finnode1 = pinode.next;
  //printf("%d ", fnode1->line);
  for (; finnode1 != NULL; finnode1 = finnode1->next)
  {
    fnode = finnode1->node;
    if (fnode->gdval1 == ALL_F)
    {
      fprintf(fout_flt_in, "1,");
      // fnode->gdval0 = ALL_F;
    }
    else if (fnode->gdval1 == 0)
    {
      fprintf(fout_flt_in, "0,");
      //fnode->gdval0 = 0;
    }
  }
  fprintf(fout_flt_in, ",");
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    val1 = fnode->gdval1;
    //printf("%d ", fnode->line);
    if (val1 == 0)
      fprintf(fout_flt_in, "0,");
    else if (val1 == ALL_F)
      fprintf(fout_flt_in, "1,");
    else
      printf(" error -28329-\n"), exit(1);
  }
  fprintf(fout_flt_in, "\n");
}

scan_out(finnode)
    FIN_NODE *finnode;
{
  L_NODE *fnode;
  int ia, ib, ic, togglecount, togglecount1, tmpl;
  int ff_state[ffnum], tmp[ffnum];
  unsigned int val1;

  for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
  {
    fnode = finnode->node;
    val1 = fnode->gdval1;
    // if(initial_ff_state[2][ia]!=val1) printf("exit!\n");

    if (val1 == 0)
    {
      ff_state[ia] = 0;
      tmp[ia] = 0;
    }
    else if (val1 == ALL_F)
    {
      ff_state[ia] = 1;
      tmp[ia] = 1;
    }
    else
      printf("E:%d\n", val1);
  }

  for (ia = 0; ia < chainnum; ia++)
  {
    if (schain[ia].length > 0)
    {
      schain[ia].lastval = ff_state[schain[ia].top];
    }
  }
  for (ia = 1; ia <= (ffnum - 1) / chainnum; ia++)
  {
    for (ib = 0; ib < chainnum; ib++)
    {
      if (ia < schain[ib].length)
      {
#if POWEREVA
        if (ff_state[schain[ib].top + ia] != schain[ib].lastval)
        {
          toggle_scn += schain[ib].length - ia;
          toggle_scn_out += schain[ib].length - ia;
          toggle_shift_perpat += schain[ib].length - ia;
        }
#endif
        schain[ib].lastval = ff_state[schain[ib].top + ia];
      }
    }
  }

  //exit(1);
}

initialize_detect(flt_node, length)
    FLT_NODE *flt_node;
int length;
{
  int ni;
  while (flt_node != NULL)
  {
    flt_node->dtime = 0;
    flt_node->TranDetTimes = 0;
    for (ni = 0; ni <= length / 32; ni++)
      flt_node->detect[ni] = flt_node->activ[ni] = 0;
    flt_node = flt_node->next;
  }
}

flt_info(fgnode)
    FLT_NODE *fgnode;
{
    int ia = 0;
  flt_det_flag = (int **)malloc((sum_flt + 2) * sizeof(int *));
  if (flt_det_flag == NULL)
    printf("memory error @flt_det_flag in flt_info \n"), exit(1);

  for (ia = 0; ia <= sum_flt + 1; ia++)
  {
    flt_det_flag[ia] = (int *)malloc(11 * sizeof(int));
    if (flt_det_flag[ia] == NULL)
      printf("memory error @flt_det_flag \n"), exit(1);
  }


  for (; fgnode != NULL; fgnode = fgnode->next)
  {
    for (ia = 0; ia <= 10; ia++)
    {
      flt_det_flag[fgnode->num][ia] = 0;
      flt_det_flag[0][ia] = 0;
    }
  }
}

prn_detect(fgnode, length)
    FLT_NODE *fgnode;
int length;
{
  int ni;

  for (; fgnode != NULL; fgnode = fgnode->next)
  {
    if (!fgnode->dtime)
      continue;
    if (fgnode->num == 7765)
    {
      printf(" Fault %d", fgnode->back->line);
      if (fgnode->forwd != NULL)
        printf(" >> %d", fgnode->forwd->line);
      else
        printf(" >> 0");
      printf(" num %d   sa %d |", fgnode->num, fgnode->saval);
      printf(" %d times", fgnode->dtime);
      printf("\n \t Detected ");

      for (ni = 1; ni <= length; ni++)
      {
        if (fgnode->detect[ni / 32] & ITI << (ni % 32))
          printf(" %d", ni);
      }
      printf("\n \t Propagate ");

      for (ni = 1; ni <= length; ni++)
      {
        if (fgnode->activ[ni / 32] & ITI << (ni % 32))
          printf(" %d", ni);
      }
      printf("\n");
    }
  }
}

onetimesim_cp_jst(capture) int capture;
{ //just toggle cp
  L_NODE *fnode;
  FIN_NODE *finnode;
  int tpi_cnt;
  unsigned int val1;
#if WSA_REC
  unsigned int tmp_val;
  double MaxTemp = 0.0, temp = 0.0;
#endif
  unsigned int tgl_val,  new_val1;
  //printf("\nhere?\n ");
  fnode = gnode.next;
  tpi_cnt = 0;
  for (; fnode != NULL; fnode = fnode->next)
  {

    finnode = fnode->finlst;
    new_val1 = finnode->node->gdval1;
#if WSA_REC
    tmp_val = fnode->gdval1; //printf(" %d: %x \n",fnode->line,fnode->gdval1);
#endif

    finnode = finnode->next;
    //printf("%d %d\n", fnode->line, fnode->type);
    //printf("target2 :Line %d val1 %x %x \n", fnode->line, fnode->gdval1, new_val1);
    for (; finnode != NULL; finnode = finnode->next)
    {
      //printf("input   :Line %d %x %X \n", finnode->node->line, finnode->node->gdval1, new_val1);
      val1 = finnode->node->gdval1;
      // printf("--%d %d\n", finnode->node->line, finnode->node->line);
      switch (fnode->type)
      {
      case AND:
      case NAND:
        new_val1 &= val1;
        break;
      case OR:
      case NOR:
        new_val1 |= val1;
        break;
      default:
        printf(" error type %d -284-\n", fnode->type);
        exit(1);
      }
    }
    //printf("output1 :Line %d val %x newval %x \n", fnode->line, fnode->gdval1, new_val1);
    if (fnode->type == NAND || fnode->type == NOR || fnode->type == NOT)
    {
      //printf("反転\n");
      fnode->gdval1 = ~new_val1;
      // if(capture==cap_freq-Transcycle) fnode->gdval0 = fnode->gdval1;
    }
    else
    {
      fnode->gdval1 = new_val1;
    }
    //printf("output1 :Line %d val %x newval %x \n\n", fnode->line, fnode->gdval1, new_val1);
#if WSA_REC
    if (capture == 1)
    {
      MaxWSA += (1 + fnode->outnum);
      MaxTemp += (1 + fnode->outnum);
    }
    if (fnode->gdval1 != tmp_val)
    {
      WSA[capture] += (1 + fnode->outnum);
      //temp+=fnode->outnum;
      //Wsa_for_Peak[cap_cycle]+=fnode->outnum;
    }
#endif
    if(capture == 1 ) {
      fnode->tff_org_gval=fnode->gdval1;
      fnode->tff_trans_gval=fnode->tff_org_gval;
    }
    if (capture < SKIP_CYCLE || fnode->cp_flag != 1 || capture % INTERVAL_CYCLE) continue;
    if (CP_CTR_MODE == LCP_TOG){
            //tgl_val = tmp2_val ^ fnode->gdval1;
            fnode->gdval1 = ~fnode->tff_trans_gval;
            fnode->tff_trans_gval=fnode->gdval1;
            //printf(" %d: %x %x\n",fnode->line,tmp2_val,fnode->gdval1);
    }
    else if (CP_CTR_MODE == LCP_RAN){
      if (tgl_tpi[tpi_cnt][capture] == 1)
        fnode->gdval1 = ALL_F;
      else if (tgl_tpi[tpi_cnt][capture] == 0)
        fnode->gdval1 = 0;
      else if (tgl_tpi[tpi_cnt][capture] == X)
        printf("error: Not support X value\n"), exit(1);
      tpi_cnt++;
    }
#if DEBUG_NODE
    //if(capture>cap_freq-Transcycle)
    printf(" Line %d val1 %x %x -298-\n", fnode->line, fnode->gdval0, fnode->gdval1);
#endif
  }
  //printf("\nhere?\n ");
}

onetimesim_cp_inversion(capture) int capture;
{//inversion control point
  L_NODE *fnode;
  FIN_NODE *finnode;
  int tpi_cnt;
  unsigned int val1;
#if WSA_REC
  unsigned int tmp_val;
  double MaxTemp = 0.0, temp = 0.0;
#endif
  unsigned int tgl_val,  new_val1;
  //printf("\nhere?\n ");
  fnode = gnode.next;
  tpi_cnt = 0;
  for (; fnode != NULL; fnode = fnode->next)
  {

    finnode = fnode->finlst;
    new_val1 = finnode->node->gdval1;
#if WSA_REC
    tmp_val = fnode->gdval1; //printf(" %d: %x \n",fnode->line,fnode->gdval1);
#endif

    finnode = finnode->next;
    //printf("%d %d\n", fnode->line, fnode->type);
    //printf("target2 :Line %d val1 %x %x \n", fnode->line, fnode->gdval1, new_val1);
    for (; finnode != NULL; finnode = finnode->next)
    {
      //printf("input   :Line %d %x %X \n", finnode->node->line, finnode->node->gdval1, new_val1);
      val1 = finnode->node->gdval1;
      // printf("--%d %d\n", finnode->node->line, finnode->node->line);
      switch (fnode->type)
      {
      case AND:
      case NAND:
        new_val1 &= val1;
        break;
      case OR:
      case NOR:
        new_val1 |= val1;
        break;
      default:
        printf(" error type %d -284-\n", fnode->type);
        exit(1);
      }
    }
    //printf("output1 :Line %d val %x newval %x \n", fnode->line, fnode->gdval1, new_val1);
    if (fnode->type == NAND || fnode->type == NOR || fnode->type == NOT)
    {
      //printf("反転\n");
      fnode->gdval1 = ~new_val1;
      // if(capture==cap_freq-Transcycle) fnode->gdval0 = fnode->gdval1;
    }
    else
    {
      fnode->gdval1 = new_val1;
    }
    //printf("output1 :Line %d val %x newval %x \n\n", fnode->line, fnode->gdval1, new_val1);
#if WSA_REC
    if (capture == 1)
    {
      MaxWSA += (1 + fnode->outnum);
      MaxTemp += (1 + fnode->outnum);
    }
    if (fnode->gdval1 != tmp_val)
    {
      WSA[capture] += (1 + fnode->outnum);
      //temp+=fnode->outnum;
      //Wsa_for_Peak[cap_cycle]+=fnode->outnum;
    }
#endif

    if (capture < SKIP_CYCLE || fnode->cp_flag != 1 || capture % INTERVAL_CYCLE) continue;
    if (CP_CTR_MODE == LCP_TOG){
            //tgl_val = tmp2_val ^ fnode->gdval1;
            fnode->gdval1 = ~fnode->gdval1;
            //printf(" %d: %x %x\n",fnode->line,tmp2_val,fnode->gdval1);
    }
    else if (CP_CTR_MODE == LCP_RAN){
      if (tgl_tpi[tpi_cnt][capture] == 1)
        fnode->gdval1 = ALL_F;
      else if (tgl_tpi[tpi_cnt][capture] == 0)
        fnode->gdval1 = 0;
      else if (tgl_tpi[tpi_cnt][capture] == X)
        printf("error: Not support X value\n"), exit(1);
      tpi_cnt++;
    }
#if DEBUG_NODE
    //if(capture>cap_freq-Transcycle)
    printf(" Line %d val1 %x %x -298-\n", fnode->line, fnode->gdval0, fnode->gdval1);
#endif
  }
  //printf("\nhere?\n ");
}

onetimesim_cp_tdt(capture) int capture;
{//transition driven toggle cotrol point
  L_NODE *fnode;
  FIN_NODE *finnode;
  int tpi_cnt;
  unsigned int val1;
#if WSA_REC
  unsigned int tmp_val;
  double MaxTemp = 0.0, temp = 0.0;
#endif
  unsigned int tgl_val, tmp2_val, new_val1;
  //printf("\nhere?\n ");
  fnode = gnode.next;
  tpi_cnt = 0;
  for (; fnode != NULL; fnode = fnode->next)
  {

    finnode = fnode->finlst;
    new_val1 = finnode->node->gdval1;
    tmp2_val = fnode->gdval1;
#if WSA_REC
    tmp_val = fnode->gdval1; //printf(" %d: %x \n",fnode->line,fnode->gdval1);
#endif

    finnode = finnode->next;
    //printf("%d %d\n", fnode->line, fnode->type);
    //printf("target2 :Line %d val1 %x %x \n", fnode->line, fnode->gdval1, new_val1);
    for (; finnode != NULL; finnode = finnode->next)
    {
      //printf("input   :Line %d %x %X \n", finnode->node->line, finnode->node->gdval1, new_val1);
      val1 = finnode->node->gdval1;
      // printf("--%d %d\n", finnode->node->line, finnode->node->line);
      switch (fnode->type)
      {
      case AND:
      case NAND:
        new_val1 &= val1;
        break;
      case OR:
      case NOR:
        new_val1 |= val1;
        break;
      default:
        printf(" error type %d -284-\n", fnode->type);
        exit(1);
      }
    }
    //printf("output1 :Line %d val %x newval %x \n", fnode->line, fnode->gdval1, new_val1);
    if (fnode->type == NAND || fnode->type == NOR || fnode->type == NOT)
    {
      //printf("反転\n");
      fnode->gdval1 = ~new_val1;
      // if(capture==cap_freq-Transcycle) fnode->gdval0 = fnode->gdval1;
    }
    else
    {
      fnode->gdval1 = new_val1;
    }
    //printf("output1 :Line %d val %x newval %x \n\n", fnode->line, fnode->gdval1, new_val1);
#if WSA_REC
    if (capture == 1)
    {
      MaxWSA += (1 + fnode->outnum);
      MaxTemp += (1 + fnode->outnum);
    }
    if (fnode->gdval1 != tmp_val)
    {
      WSA[capture] += (1 + fnode->outnum);
    }
#endif

    if (capture < SKIP_CYCLE || fnode->cp_flag != 1 || capture % INTERVAL_CYCLE) continue;
    if (CP_CTR_MODE == LCP_TOG){
            tgl_val = tmp2_val ^ fnode->gdval1;
            fnode->gdval1 = ~(fnode->gdval1 ^ tgl_val);
    }
    else if (CP_CTR_MODE == LCP_RAN){
      if (tgl_tpi[tpi_cnt][capture] == 1)
        fnode->gdval1 = ALL_F;
      else if (tgl_tpi[tpi_cnt][capture] == 0)
        fnode->gdval1 = 0;
      else if (tgl_tpi[tpi_cnt][capture] == X)
        printf("error: Not support X value\n"), exit(1);
      tpi_cnt++;
    }
#if DEBUG_NODE
    //if(capture>cap_freq-Transcycle)
    printf(" Line %d val1 %x %x -298-\n", fnode->line, fnode->gdval0, fnode->gdval1);
#endif
  }
}


onetimesim(capture) int capture;
{
  L_NODE *fnode;
  FIN_NODE *finnode;
  int tpi_cnt;
  unsigned int val1;
#if WSA_REC
  unsigned int tmp_val;
  double MaxTemp = 0.0, temp = 0.0;
#endif
  unsigned int tgl_val,  new_val1;
  //printf("\nhere?\n ");
  fnode = gnode.next;
  tpi_cnt = 0;
  for (; fnode != NULL; fnode = fnode->next)
  {

    finnode = fnode->finlst;
    new_val1 = finnode->node->gdval1;
#if WSA_REC
    tmp_val = fnode->gdval1; //printf(" %d: %x \n",fnode->line,fnode->gdval1);
#endif
    //printf("target  :Line %d %x %x \n", fnode->line, fnode->gdval1, new_val1);
    //printf("input   :Line %d %x %x \n", finnode->node->line, finnode->node->gdval1, new_val1);
    finnode = finnode->next;
    //printf("%d %d\n", fnode->line, fnode->type);
    //printf("target2 :Line %d val1 %x %x \n", fnode->line, fnode->gdval1, new_val1);
    for (; finnode != NULL; finnode = finnode->next)
    {
      //printf("input   :Line %d %x %X \n", finnode->node->line, finnode->node->gdval1, new_val1);
      val1 = finnode->node->gdval1;
      // printf("--%d %d\n", finnode->node->line, finnode->node->line);
      switch (fnode->type)
      {
      case AND:
      case NAND:
        new_val1 &= val1;
        break;
      case OR:
      case NOR:
        new_val1 |= val1;
        break;
      default:
        printf(" error type %d -284-\n", fnode->type);
        exit(1);
      }
    }
    //printf("output1 :Line %d val %x newval %x \n", fnode->line, fnode->gdval1, new_val1);
    if (fnode->type == NAND || fnode->type == NOR || fnode->type == NOT)
    {
      //printf("反転\n");
      fnode->gdval1 = ~new_val1;
      // if(capture==cap_freq-Transcycle) fnode->gdval0 = fnode->gdval1;
    }

    else
    {
      fnode->gdval1 = new_val1;
    }
    //printf("output1 :Line %d val %x newval %x \n\n", fnode->line, fnode->gdval1, new_val1);
#if WSA_REC
    if (capture == 1)
    {
      MaxWSA += (1 + fnode->outnum);
      MaxTemp += (1 + fnode->outnum);
    }
    if (fnode->gdval1 != tmp_val)
    {
      WSA[capture] += (1 + fnode->outnum);
      //temp+=fnode->outnum;
      //Wsa_for_Peak[cap_cycle]+=fnode->outnum;
    }
#endif

  }
}

ftvalsim_cp_jst(capture) int capture;
{//just toggle cpi
  L_NODE *fnode;
  FIN_NODE *finnode;
  unsigned int val1;
  int ia;
  int count1 = 0;
  unsigned int tgl_val, new_val1;
  int tpi_cnt = 0;
  fnode = gnode.next;
  for (; fnode != NULL; fnode = fnode->next)
  {
    count1++;
    finnode = fnode->finlst;
    new_val1 = finnode->node->ftval1;
    finnode = finnode->next;

//if(CP_CTR_MODE==1&&fnode->cp_flag==1){
// tgl_val= fnode->ftval1; printf(" %d: %x %x\n",fnode->line,fnode->gdval1,fnode->ftval1);
//}
#if DEBUG1
    if (fnode->line == 38)
      printf(" Faulty val In-line %x\n", new_val1);
#endif
    for (; finnode != NULL; finnode = finnode->next)
    {
      val1 = finnode->node->ftval1;
      switch (fnode->type)
      {
      case AND:
      case NAND:
        new_val1 &= val1;
        break;
      case OR:
      case NOR:
        new_val1 |= val1;
        break;
      default:
        printf(" error type %d -284-\n", fnode->type);
        exit(1);
      }
    }
    if (fnode->type == NAND || fnode->type == NOR || fnode->type == NOT)
    {
      fnode->ftval1 = ~new_val1;
    }
    else
    {
      fnode->ftval1 = new_val1;
    }


    if(capture == 1 ) fnode->tff_trans_fval=fnode->tff_org_gval;
  //  printf(" %d: %x \n",fnode->line, fnode->tff_trans_fval);
    if (capture < SKIP_CYCLE  || !fnode->cp_flag  || capture % INTERVAL_CYCLE) continue;
    if (CP_CTR_MODE == LCP_TOG){
            //tgl_val = tmp2_val ^ fnode->gdval1;
            fnode->ftval1 = ~fnode->tff_trans_fval;
            fnode->tff_trans_fval=fnode->ftval1;
    //  if(fnode->line == 3931)
        //   printf("%d %d: %x %x %x\n", capture,fnode->line, fnode->ftval1,fnode->tff_trans_fval,fnode->tff_org_gval);
          }
    else if (CP_CTR_MODE == LCP_RAN){
      if (tgl_tpi[tpi_cnt][capture] == 1)
        fnode->ftval1 = ALL_F;
      else if (tgl_tpi[tpi_cnt][capture] == 0)
        fnode->ftval1 = 0;
      else if (tgl_tpi[tpi_cnt][capture] == X)
        printf("error: Not support X value\n"), exit(1);
      tpi_cnt++;
    }

    /*if(CP_CTR_MODE==1&& fnode->cp_flag==1){
if(tgl_val==fnode->ftval1)
  fnode->ftval1= ~fnode->ftval1;printf(" %d: %x %x\n",fnode->line,fnode->gdval1,fnode->ftval1);
}*/

#if DEBUG_NODE
    if (fnode->line == 380)
      printf(" Line %d gdval %x ftval %x %d\n", fnode->line, fnode->gdval_slow, fnode->ftval1, count1);
#endif
  }
  //printf("%d ",count1);
  //exit(1);
}

ftvalsim_cp_inversion(capture) int capture;
{//inversion cpi
  L_NODE *fnode;
  FIN_NODE *finnode;
  unsigned int val1;
  int ia;
  int count1 = 0;
  unsigned int tgl_val, new_val1;
  int tpi_cnt = 0;
  fnode = gnode.next;
  for (; fnode != NULL; fnode = fnode->next)
  {
    count1++;
    finnode = fnode->finlst;
    new_val1 = finnode->node->ftval1;
    finnode = finnode->next;

//if(CP_CTR_MODE==1&&fnode->cp_flag==1){
// tgl_val= fnode->ftval1; printf(" %d: %x %x\n",fnode->line,fnode->gdval1,fnode->ftval1);
//}
#if DEBUG1
    if (fnode->line == 38)
      printf(" Faulty val In-line %x\n", new_val1);
#endif
    for (; finnode != NULL; finnode = finnode->next)
    {
      val1 = finnode->node->ftval1;
      switch (fnode->type)
      {
      case AND:
      case NAND:
        new_val1 &= val1;
        break;
      case OR:
      case NOR:
        new_val1 |= val1;
        break;
      default:
        printf(" error type %d -284-\n", fnode->type);
        exit(1);
      }
    }
    if (fnode->type == NAND || fnode->type == NOR || fnode->type == NOT)
    {
      fnode->ftval1 = ~new_val1;
    }
    else
    {
      fnode->ftval1 = new_val1;
    }

    if (capture < SKIP_CYCLE  || fnode->cp_flag != 1 || capture % INTERVAL_CYCLE) continue;
    if (CP_CTR_MODE == LCP_TOG){
            fnode->ftval1 = ~fnode->ftval1;
            //printf(" %d: %x %x\n",fnode->line, fnode->ftval1,fnode->gdval1);
    }
    else if (CP_CTR_MODE == LCP_RAN){
      if (tgl_tpi[tpi_cnt][capture] == 1)
        fnode->ftval1 = ALL_F;
      else if (tgl_tpi[tpi_cnt][capture] == 0)
        fnode->ftval1 = 0;
      else if (tgl_tpi[tpi_cnt][capture] == X)
        printf("error: Not support X value\n"), exit(1);
      tpi_cnt++;
    }

    /*if(CP_CTR_MODE==1&& fnode->cp_flag==1){
if(tgl_val==fnode->ftval1)
  fnode->ftval1= ~fnode->ftval1;printf(" %d: %x %x\n",fnode->line,fnode->gdval1,fnode->ftval1);
}*/

#if DEBUG_NODE
    if (fnode->line == 380)
      printf(" Line %d gdval %x ftval %x %d\n", fnode->line, fnode->gdval_slow, fnode->ftval1, count1);
#endif
  }
  //printf("%d ",count1);
  //exit(1);
}


ftvalsim_cp_tdt(capture) int capture;
{//transition driven toggle cpi
  L_NODE *fnode;
  FIN_NODE *finnode;
  unsigned int val1;
  int ia;
  int count1 = 0;
  unsigned int tgl_val, tmp_val, new_val1;
  int tpi_cnt = 0;
  fnode = gnode.next;
  for (; fnode != NULL; fnode = fnode->next)
  {
    count1++;
    finnode = fnode->finlst;
    new_val1 = finnode->node->ftval1;
    finnode = finnode->next;
    tmp_val = fnode->ftval1;
//if(CP_CTR_MODE==1&&fnode->cp_flag==1){
// tgl_val= fnode->ftval1; printf(" %d: %x %x\n",fnode->line,fnode->gdval1,fnode->ftval1);
//}
#if DEBUG1
    if (fnode->line == 38)
      printf(" Faulty val In-line %x\n", new_val1);
#endif
    for (; finnode != NULL; finnode = finnode->next)
    {
      val1 = finnode->node->ftval1;
      switch (fnode->type)
      {
      case AND:
      case NAND:
        new_val1 &= val1;
        break;
      case OR:
      case NOR:
        new_val1 |= val1;
        break;
      default:
        printf(" error type %d -284-\n", fnode->type);
        exit(1);
      }
    }
    if (fnode->type == NAND || fnode->type == NOR || fnode->type == NOT)
    {
      fnode->ftval1 = ~new_val1;
    }
    else
    {
      fnode->ftval1 = new_val1;
    }

    if (capture < SKIP_CYCLE  || fnode->cp_flag != 1 || capture % INTERVAL_CYCLE) continue;
    if (CP_CTR_MODE == LCP_TOG){
            tgl_val = tmp_val ^ fnode->ftval1;
            fnode->ftval1 = ~(fnode->ftval1 ^ tgl_val);
          //  printf(" Flt: %d: %x %x\n",fnode->line,fnode->ftval1,fnode->gdval1);
    }
    else if (CP_CTR_MODE == LCP_RAN){
      if (tgl_tpi[tpi_cnt][capture] == 1)
        fnode->ftval1 = ALL_F;
      else if (tgl_tpi[tpi_cnt][capture] == 0)
        fnode->ftval1 = 0;
      else if (tgl_tpi[tpi_cnt][capture] == X)
        printf("error: Not support X value\n"), exit(1);
      tpi_cnt++;
    }

    /*if(CP_CTR_MODE==1&& fnode->cp_flag==1){
if(tgl_val==fnode->ftval1)
  fnode->ftval1= ~fnode->ftval1;printf(" %d: %x %x\n",fnode->line,fnode->gdval1,fnode->ftval1);
}*/

#if DEBUG_NODE
    if (fnode->line == 380)
      printf(" Line %d gdval %x ftval %x %d\n", fnode->line, fnode->gdval_slow, fnode->ftval1, count1);
#endif
  }
  //printf("%d ",count1);
  //exit(1);
}


ftvalsim(capture) int capture;
{
  L_NODE *fnode;
  FIN_NODE *finnode;
  unsigned int val1;
  int ia;
  int count1 = 0;
  unsigned int tgl_val, tmp_val, new_val1;
  int tpi_cnt = 0;
  fnode = gnode.next;
  for (; fnode != NULL; fnode = fnode->next)
  {
    count1++;
    finnode = fnode->finlst;
    new_val1 = finnode->node->ftval1;
    finnode = finnode->next;
    tmp_val = fnode->ftval1;
//if(CP_CTR_MODE==1&&fnode->cp_flag==1){
// tgl_val= fnode->ftval1; printf(" %d: %x %x\n",fnode->line,fnode->gdval1,fnode->ftval1);
//}
#if DEBUG1
    if (fnode->line == 38)
      printf(" Faulty val In-line %x\n", new_val1);
#endif
    for (; finnode != NULL; finnode = finnode->next)
    {
      val1 = finnode->node->ftval1;
      switch (fnode->type)
      {
      case AND:
      case NAND:
        new_val1 &= val1;
        break;
      case OR:
      case NOR:
        new_val1 |= val1;
        break;
      default:
        printf(" error type %d -284-\n", fnode->type);
        exit(1);
      }
    }
    if (fnode->type == NAND || fnode->type == NOR || fnode->type == NOT)
    {
      fnode->ftval1 = ~new_val1;
    }
    else
    {
      fnode->ftval1 = new_val1;
    }

#if DEBUG_NODE
    if (fnode->line == 380)
      printf(" Line %d gdval %x ftval %x %d\n", fnode->line, fnode->gdval_slow, fnode->ftval1, count1);
#endif
  }

}
