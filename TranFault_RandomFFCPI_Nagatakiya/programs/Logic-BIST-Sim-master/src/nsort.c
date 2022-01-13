#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"

read_tpi_list(argv) char *argv[14];
{
  int i, ia, ib;
  float tmp, gt_cnt = 0.0;
  char tgl_file[100];
  sprintf(tgl_file, "%s_tgl_FF_input.dat", argv[1]);
  FILE *fin1;
  for (i = 0; i <= lpnt; i++)
    toggle_gates[i] = 0;
printf("\n\nCPI Number=%d\n-----list,location\n", (int)(numgate * atof(argv[5])));
//  if (atoi(argv[4]) == 1 || atoi(argv[4]) == 4)
//  { //Structure Based toggle Gate selection

    fin1 = fopen("tgl_gt_input.dat", "r");
    if (fin1 == NULL)
      printf("'tgl_gt_input.dat' is not found!\n"), exit(1);

    for (ia = 0; ia < (int)(numgate * atof(argv[5])); ia++)
    {
      //for(ia=0;ia<TGL_GT_NUM;ia++){
      fscanf(fin1, "%d\n", &ib);
      toggle_gates[ib + inpnum + numout + ffnum] = 1;
    //   toggle_gates[ib] = 1;
      printf("-----%d %d\n", ib, ib + inpnum + numout + ffnum);
    }
    fclose(fin1);
  // exit(1);
//  }
}

initial_node(argv) char *argv[1];
{
  L_NODE *fnode;
  FIN_NODE *finnode, *finnode1;

  int i, ia, ib;
  float tmp, gt_cnt = 0.0;

  char tgl_file[100];

  sprintf(tgl_file, "%s_tgl_FF_input.dat", argv[1]);

  if (CP_CTR_MODE == LCP_TOG || CP_CTR_MODE == LCP_RAN)
  {
    //tgl_gt_cnt=TGL_GT_NUM;
    tgl_gt_cnt = numgate * Tgl_rate;

   //printf("%d\n", numgate); exit(1);
    fnode = gnode.next;
    gt_cnt = 0.0;

    for (; fnode != NULL; fnode = fnode->next)
    {
      fnode->cp_flag = 0;
      if (toggle_gates[fnode->line] == 1) {  fnode->cp_flag = 1;
        printf("-%d : %d ",fnode->line,fnode->type);
      }
      for (i = 0; i < MAXCAP; i++)
        fnode->toggle_rate[i] = 0.0;

    }
  //  printf("\n");
   //exit(1);

  }
  else if (CP_CTR_MODE == FCP_TOG || CP_CTR_MODE == FCP_RAN)
  { //FF TPI

    FILE *fin;
    int toggle_FFs[lpnt];
    for (i = 0; i <= lpnt; i++)
      toggle_FFs[i] = 0;

    fin = fopen(tgl_file, "r");
    if (fin == NULL)
      printf("'tgl_FF_input.dat' is not found!\n"), exit(1);
    //printf("%d %f \n",numgate-ffnum-inpnum,GATE_GP_START*(numgate-ffnum-inpnum));

    printf("\n\nFF-CPI Number=%d\n", (int)(ffnum * ff_rate));
    for (ia = 0; ia < (int)(ffnum * ff_rate); ia++)
    //for (ia = 0; ia < 2; ia++)
    {
      //for(ia=0;ia<TGL_GT_NUM;ia++){
      fscanf(fin, "%d\n", &ib);
      toggle_FFs[ib] = 1;
      printf(" %d,%d|",ia,ib);
    }
    //tgl_gt_cnt=TGL_GT_NUM;
    tgl_gt_cnt = ffnum * ff_rate;
    fclose(fin);

    ib = 0;
    finnode = ffnode.next;
    for (; finnode != NULL; finnode = finnode->next)
    {
      fnode = finnode->node;
      fnode->cp_flag = 0;
      ib = fnode->line - inpnum - numout;
      //printf("%d, %d\n", fnode->line, toggle_FFs[fnode->line]);
      if (toggle_FFs[ib] == 1)
      {
        fnode->cp_flag = 1;
        // printf("%d\n", fnode->line);
        // printf("----maru\n");
      }
      //printf("%d, %d, %d\n",ib,fnode->line,toggle_FFs[ib]);

      /*	if(toggle_FFs[fnode->line]==1){
		fnode->cp_flag=1;
	}*/
      for (i = 0; i < MAXCAP; i++)
        fnode->toggle_rate[i] = 0.0;
    }
    //exit(1);
  }
  else
  {
    fnode = gnode.next;
    for (; fnode != NULL; fnode = fnode->next)
    {
      for (i = 0; i < MAXCAP; i++)
        fnode->toggle_rate[i] = 0.0;
    }
  }

  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    for (i = 0; i < MAXCAP; i++)
      fnode->toggle_rate[i] = 0.0;
  }
  finnode = ponode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    for (i = 0; i < MAXCAP; i++)
      fnode->toggle_rate[i] = 0.0;
  }
  finnode = pinode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    for (i = 0; i < MAXCAP; i++)
      fnode->toggle_rate[i] = 0.0;
  }
}

sort_node()
{
  L_NODE *fnode, *new_head, *target, *last_node;
  FIN_NODE *finnode, *finnode1;
  int i, ni, line, fil, fincheck[lpnt + 1];

  for (ni = 1; ni <= lpnt; ni++)
    fincheck[ni] = 0;
  new_head = NULL;

  /** extract FF nodes **/
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;

    if (fnode->next != NULL)
      fnode->next->prev = fnode->prev;
    fnode->prev->next = fnode->next;
    fnode->next = new_head;
    new_head = fnode;
    fincheck[fnode->line] = 1;
  }
  last_node = ffnode.next->node;
  /** extract PI nodes **/
  finnode = pinode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    fnode = finnode->node;
    if (fnode->next != NULL)
      fnode->next->prev = fnode->prev;
    fnode->prev->next = fnode->next;
    fnode->next = new_head;
    new_head = fnode;
    fincheck[fnode->line] = 1;
  }
  inode.next = new_head;

#if DEBUG_NODE
  prn_node(new_head);   /** print PI & FF nodes list **/
  prn_node(gnode.next); /** print the other nodes list **/
#endif
  new_head = NULL;
  while (gnode.next != NULL)
  {
    fnode = gnode.next;
    for (; fnode != NULL;)
    {
      if (fin_check(fnode->finlst, fincheck) == TRUE)
      {
        target = fnode;
        fnode = fnode->next;
        if (target->next != NULL)
          target->next->prev = target->prev;
        target->prev->next = target->next;
        target->next = new_head;
        new_head = target;
        fincheck[target->line] = 1;
      }
      else
        fnode = fnode->next;
    }
  }
#if DEBUG_NODE
  prn_node(new_head);
#endif
  /** reverse order **/
  fnode = new_head;
  new_head = NULL;
  for (; fnode != NULL;)
  {
    target = fnode;
    fnode = fnode->next;
    target->next = new_head;
    target->prev = fnode;
    new_head = target;
  }
  /** exclude PI & FF nodes **/
  new_head->prev = &gnode;
  gnode.next = new_head;

  // fnode = gnode.next;
  // for (; fnode != NULL; fnode = fnode->next)
  // {
  //   printf("--%d\n", fnode->line);
  //   finnode = fnode->finlst;
  //   finnode1 = fnode->foutlst;
  //   // for (; finnode != NULL; finnode = finnode->next)
  //   // {
  //   //   printf("%d\n", finnode->node->line);
  //   // }
  //   for (; finnode1 != NULL; finnode1 = finnode1->next)
  //   {
  //     printf("%d\n", finnode1->node->line);
  //   }
  // }

  /** include PI & FF nodes **
  last_node->next=new_head;
  new_head->prev=last_node;
  gnode.next=inode.next;
  inode.next->pre=&gnode;
**/

#if DEBUG_NODE
  prn_node(gnode.next);
#endif
}
fin_check(finnode, fincheck)
    FIN_NODE *finnode;
int fincheck[];
{
  for (; finnode != NULL; finnode = finnode->next)
  {
    if (fincheck[finnode->node->line] == 0)
      break;
  }
  if (finnode == NULL)
    return TRUE;
  else
    return FALSE;
}
