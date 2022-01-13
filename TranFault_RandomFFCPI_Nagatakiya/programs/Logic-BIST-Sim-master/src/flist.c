#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"

#define ERR_MSG(x) printf(" x \n"), exit(1);
#define NALLOC (L_NODE *)malloc(sizeof(L_NODE))
#define FALLOC (FLT_NODE *)malloc(sizeof(FLT_NODE))
#define INALLOC (FIN_NODE *)malloc(sizeof(FIN_NODE))
#define CKT_FORM 0 /* 0 for Osaka-format, 1 for Ehime-format */
ELEMENT *gate;
int *pitbl, *potbl, *fftbl, *ffintbl, *flist;

make_line_list(argv) char *argv[13];
{
  L_NODE *fnode, *head_node, **address;
  FIN_NODE *finnode, *head_fin;
  int i, ni, fil, line, fin, fol, saval;
  int count2 = 0;
#if CKT_FORM == 0
  //printf("herere?\n");exit(1);
  readf(argv);
  //  printf("herere?%d\n", MODE_TOOL);exit(1);
  if (MODE_TOOL == 4 && (CP_CTR_MODE==LCP_TOG||CP_CTR_MODE==LCP_RAN)){
  //printf("herere?\n"); exit(1);
  read_tpi_list(argv);
}
  int ie;
  int ic = 0;

/*for (ie = 1; ie <= lpnt; ie++)
  {
    if (toggle_gates[ie] == 1 && gate[ie].type == 3)
    //if (gate[ie].type == 3)
    {
      gate[ie].type = TPI;
      ic++;
    }
    //printf("%d\n", gate[ie].line);
  }*/
  //exit(1);
#else
  readf_ehime(argv);
#endif

  address = (L_NODE **)calloc(lpnt + 1, sizeof(L_NODE *));
  if (address == NULL)
    printf("memory error @make_line_list\n"), exit(1);

  head_node = NULL;
  for (ni = lpnt; ni >= 1; ni--)
  {
    if (gate[ni].type == FAN)
      continue;

    if (NULL == (fnode = NALLOC))
      printf(" error -984-\n"), exit(1);
    //count2++;
    address[ni] = fnode;
    fnode->line = ni;
    fnode->type = gate[ni].type;
    fnode->innum = gate[ni].nfi;
    fnode->outnum = gate[ni].nfo;
    fnode->next = head_node;

    //printf("%d\n", fnode->line);

    //printf("%d %d %d %d\n", fnode->line, fnode->type, fnode->innum, fnode->outnum);

    if (head_node != NULL)
      head_node->prev = fnode;
    head_node = fnode;
  }

  //printf("%d ",count2);exit(1);
  for (ni = 1; ni <= lpnt; ni++)
  {
    if (gate[ni].type == FAN)
    {
      fil = gate[ni].fil;
      address[ni] = address[fil];
    }
  }
  fnode = head_node;
  for (; fnode != NULL; fnode = fnode->next)
  {
    line = fnode->line;
    /*** make fan-in lists ***/
    fil = gate[line].fil;
    if (gate[line].type == PI)
    {
      fnode->finlst = NULL;
    }
    else if (gate[line].nfi == 1)
    {
      if (NULL == (finnode = INALLOC))
        printf(" error -239-\n"), exit(1);
    //  finnode=fin_tmp+(fill_fin_tmp++);
      finnode->node = address[fil];
      finnode->next = NULL;
      fnode->finlst = finnode;
    }
    else
    {
      head_fin = NULL;
      for (ni = 0; ni < gate[line].nfi; ni++)
      {
        fin = flist[ni + fil];
        if (NULL == (finnode = INALLOC))
          printf(" error -233-\n"), exit(1);
      //  finnode=fin_tmp+(fill_fin_tmp++);
        finnode->node = address[fin];
        finnode->next = head_fin;
        head_fin = finnode;
      }
      fnode->finlst = head_fin;
    }
  }

  for (ni = 1; ni <= lpnt; ni++)
  {
    if (gate[ni].type == FAN)
    {
      fol = gate[ni].fol;
      address[ni] = address[fol];
    }
  }
  fnode = head_node;
  for (; fnode != NULL; fnode = fnode->next)
  {
    line = fnode->line;
#if DEBUG_NODE
    printf(" Line = %d (WI89)\n", line);
#endif

    /*** make fan-out lists ***/
    fol = gate[line].fol;
    if (gate[line].type == PO)
    {
      fnode->foutlst = NULL;
    }
    else if (gate[line].nfo == 1)
    {

      if (NULL == (finnode = INALLOC))
        printf(" error -239-\n"), exit(1);
      /*
      finnode=fin_tmp+(fill_fin_tmp++);
*/
      finnode->node = address[fol];
      finnode->next = NULL;
      fnode->foutlst = finnode;
    }
    else
    {
      head_fin = NULL;
      for (ni = 0; ni < gate[line].nfo; ni++)
      {
        fin = flist[ni + fol];

        if (NULL == (finnode = INALLOC))
          printf(" error -233-\n"), exit(1);
        /*
	finnode=fin_tmp+(fill_fin_tmp++);

#if DEBUG1
	printf(" ni = %d fin = %d fill_fin_tmp = %d \n",ni, fin,fill_fin_tmp );
#endif*/
        finnode->node = address[fin];
        finnode->next = head_fin;
        head_fin = finnode;
      }
      fnode->foutlst = head_fin;
    }
  }

  head_fin = &pinode;
  for (ni = 1; ni <= inpnum; ni++)
  {
    line = pitbl[ni];

    if (NULL == (finnode = INALLOC))
      printf(" error -259-\n"), exit(1);
    /*
    finnode=fin_tmp+(fill_fin_tmp++);
*/
    finnode->node = address[line];
    head_fin->next = finnode;
    head_fin = finnode;
  }
  head_fin->next = NULL;

  head_fin = &ffnode;
  for (ni = 1; ni <= ffnum; ni++)
  {
    line = fftbl[ni];

    if (NULL == (finnode = INALLOC))
      printf(" error -259-\n"), exit(1);
    /*
    finnode=fin_tmp+(fill_fin_tmp++);
*/
    finnode->node = address[line];
    head_fin->next = finnode;
    head_fin = finnode;
  }
  head_fin->next = NULL;

  head_fin = NULL;
  for (ni = numout; ni >= 1; ni--)
  {
    line = gate[potbl[ni]].fol;

    if (NULL == (finnode = INALLOC))
      printf(" error -259-\n"), exit(1);
    /*
    finnode=fin_tmp+(fill_fin_tmp++);
*/
    finnode->node = address[line];
    finnode->next = head_fin;
    head_fin = finnode;
  }
  ponode.next = head_fin;
  gnode.next = head_node;
  head_node->prev = &gnode;

#if TRANSITIONFAULT
  make_Tranfault_list(address);
#else
  make_fault_list(address);
#endif

  free(gate);
  free(flist);
  free(pitbl);
  free(potbl);
  free(fftbl);
  free(ffintbl);
}

make_fault_list(address)
    L_NODE **address;
{
  FIN_NODE *finnode, *head_fin;
  FLT_NODE *head_flt, *flttag;
  int ni, saval, fil, fol;

  /** make fault list **/
  head_flt = &fltlst;
  sum_flt = 0;

  for (ni = 1; ni <= lpnt; ni++)
  {
    //printf("%d\n", gate[ni].line);
    /*     if(ni<=250 || ni>=300) */
    /*       continue; */

    fol = gate[ni].fol;

    /*    if(gate[ni].nfo==1 && gate[fol].type==PO || gate[ni].type==PO) */
    if (gate[ni].type == PO)
      continue;
    if (gate[ni].nfo == 1 && gate[fol].type != FF && gate[fol].type != PO)
    {

      switch (gate[fol].type)
      {
      case AND:
      case NAND:
        saval = 1;
        break;
      case OR:
      case NOR:
        saval = 0;
        break;
      case NOT: //インバーター出力側のs0,s1故障は、入力のs1とs0と等価
      case TPI://TPIの故障を考慮しない
      case XOR:  //XORタイプＴＰＩ
        continue;
      default:
        printf(" ni = %d %d %d error -9183-\n", ni, fol, gate[fol].type), exit(1);
      }
      if (NULL == (flttag = FALLOC))
        printf(" ni = %d error -639-\n", ni), exit(1);
      /***
      flttag=(fltlst_tmp+sum_flt);
***/
      sum_flt++;
      flttag->num = sum_flt;
      flttag->line = gate[ni].line; //$B=$@5#1#5#0#9#1#1(B
      if (sum_flt >= MAXFLT)
        printf(" error 19485 sumflt = %d\n", sum_flt), exit(1);

      if (gate[ni].type == FAN)
        flttag->back = address[gate[ni].fil];
      else
        flttag->back = address[ni];
      flttag->forwd = address[fol];
      flttag->saval = saval;
      flttag->dfflst = NULL;
      /**
      printf(" ni %d fol %d address[fol] %d\n",ni,fol,address[fol]->line);
**/
      flttag->prev = head_flt;
      head_flt->next = flttag;
      head_flt = flttag;
    }

    else if (gate[ni].nfo == 1)
    { /** gate[fol].type==FF or PO **/
      for (saval = 0; saval < 2; saval++)
      {
        if (NULL == (flttag = FALLOC))
          printf(" error -639-\n"), exit(1);
        /***
	flttag=(fltlst_tmp+sum_flt);
***/
        sum_flt++;
        flttag->num = sum_flt;
        flttag->line = gate[ni].line; //$B=$@5#1#5#0#9#1#1(B
        if (sum_flt >= MAXFLT)
          printf(" error 19486 sumflt = %d\n", sum_flt), exit(1);

        if (gate[ni].type != FAN)
          flttag->back = address[ni];
        else
          flttag->back = address[gate[ni].fil];
        flttag->forwd = address[fol];
        flttag->saval = saval;
        flttag->dfflst = NULL;
        flttag->prev = head_flt;
        head_flt->next = flttag;
        head_flt = flttag;
      }
    }
    else
    {
      for (saval = 0; saval < 2; saval++)
      {
        if (NULL == (flttag = FALLOC))
          printf(" error -639-\n"), exit(1);
        /***
	flttag=(fltlst_tmp+sum_flt);
***/
        sum_flt++;

        if (sum_flt >= MAXFLT)
          printf(" error 19487 sumflt = %d\n", sum_flt), exit(1);

        flttag->back = address[ni];
        flttag->forwd = NULL;
        flttag->saval = saval;
        flttag->num = sum_flt;
        flttag->line = gate[ni].line; //$B=$@5#1#5#0#9#1#1(B
        flttag->dfflst = NULL;
        flttag->prev = head_flt;
        head_flt->next = flttag;
        head_flt = flttag;
      }
    }
  }

  head_flt->next = NULL;
#if DEBUG_NODE
  prn_fltlst(fltlst.next);
#endif
}

make_Tranfault_list(address)
    L_NODE **address;
{
  FIN_NODE *finnode, *head_fin;
  FLT_NODE *head_flt, *flttag;
  int ni, saval, saval1, fil, fol;

  /** make fault list **/
  head_flt = &fltlst;
  sum_flt = 0;
  //sum_Tran_flt = 0;

  for (ni = 1; ni <= lpnt; ni++)
  {

    /*     if(ni<=250 || ni>=300) */
    /*       continue; */

    fol = gate[ni].fol;

    /*    if(gate[ni].nfo==1 && gate[fol].type==PO || gate[ni].type==PO) */
    if (gate[ni].type == PO)
      continue;
    if (gate[ni].nfo == 1 && gate[fol].type != FF && gate[fol].type != PO)
    {
    //  if (gate[fol].type != NOT)
    //    sum_flt++;
      for (saval = 0; saval < 2; saval++)
      {
        //if(gate[fol].type==NOT)
        //continue;
		  /*
        switch (gate[fol].type)
        {
        case AND:
        case NAND:
          flttag->typeflag = 1;
          break;
        case OR:
        case NOR:
          flttag->typeflag = 2;
          break;
        case NOT: //インバーター出力側のs0,s1故障は、入力のs1とs0と等価
        case TPI://TPIの故障を考慮しない
        case XOR:  //XORタイプＴＰＩ
          continue;
        default:
          printf(" ni = %d %d %d error -9183-\n", ni, fol, gate[fol].type), exit(1);
        }
		*/
        if (NULL == (flttag = FALLOC))
          printf(" ni = %d error -639-\n", ni), exit(1);
        /***
      flttag=(fltlst_tmp+sum_flt);

***/
        sum_flt++;
        flttag->num = sum_flt;
        //sum_Tran_flt++;
        //flttag->num = sum_Tran_flt;
        //if (sum_Tran_flt >= MAXFLT)
        //  printf(" error 19485 sumflt = %d\n", sum_Tran_flt), exit(1);
        if (sum_flt >= MAXFLT)
            printf(" error 19485 sumflt = %d\n", sum_flt), exit(1);

        if (gate[ni].type == FAN)
          flttag->back = address[gate[ni].fil];

        else
          flttag->back = address[ni];
        flttag->forwd = address[fol];
        flttag->saval = saval;
        flttag->dfflst = NULL;
        /**
      printf(" ni %d fol %d address[fol] %d\n",ni,fol,address[fol]->line);
**/
        flttag->prev = head_flt;
        head_flt->next = flttag;
        head_flt = flttag;
      }
    }

    else if (gate[ni].nfo == 1)
    { /** gate[fol].type==FF or PO **/
      for (saval = 0; saval < 2; saval++)
      {
        if (NULL == (flttag = FALLOC))
          printf(" error -639-\n"), exit(1);
        /***
	flttag=(fltlst_tmp+sum_flt);
***/
        sum_flt++;
        flttag->num = sum_flt;
      //  sum_Tran_flt++;
      //  flttag->num = sum_Tran_flt;
        if (sum_flt >= MAXFLT)
          printf(" error 19486 sumflt = %d\n", sum_flt), exit(1);

        if (gate[ni].type != FAN)
          flttag->back = address[ni];
        else
          flttag->back = address[gate[ni].fil];
        flttag->forwd = address[fol];
        flttag->saval = saval;
        flttag->typeflag = 0;
        flttag->dfflst = NULL;
        flttag->prev = head_flt;
        head_flt->next = flttag;
        head_flt = flttag;
      }
    }
    else
    {
      for (saval = 0; saval < 2; saval++)
      {
        if (NULL == (flttag = FALLOC))
          printf(" error -639-\n"), exit(1);
        /***
	flttag=(fltlst_tmp+sum_flt);
***/
        sum_flt++;
        flttag->num = sum_flt;
        //sum_Tran_flt++;
        //flttag->num = sum_Tran_flt;
        if (sum_flt >= MAXFLT)
          printf(" error 19487 sumflt = %d\n", sum_flt), exit(1);

        flttag->back = address[ni];
        flttag->forwd = NULL;
        flttag->saval = saval;
        flttag->typeflag = 0;
        flttag->dfflst = NULL;
        flttag->prev = head_flt;
        head_flt->next = flttag;
        head_flt = flttag;
      }
    }
  }
  head_flt->next = NULL;
#if DEBUG_NODE
  prn_fltlst(fltlst.next);
#endif
}

read_fault_list(address, flt_file)
    L_NODE **address;
char *flt_file;
{
  FIN_NODE *finnode, *head_fin;
  FLT_NODE *head_flt, *flttag;
  int ni, saval, fil, fol,
      d_time, back, forwd, det_flt_num, det_fltnum2, und_fltnum;
  FILE *fp;
  char fault_moji[] = "Fault", moji[100];
  /* char length_moji[]="Length", */
  /* 	  detected_moji[]="Detected"; */

  /** make fault list **/
  head_flt = &fltlst;
  sum_flt = det_flt_num = 0;

  if (NULL == (fp = fopen(flt_file, "r")))
    printf("%s", flt_file), ERR_MSG(29421);

  while (EOF != fscanf(fp, "%s", moji))
  {
    if (!strncmp(moji, fault_moji, 5))
    {
      det_flt_num++;

      if (NULL == (flttag = FALLOC))
        printf(" ni = %d error -639-\n", ni), exit(1);

      fscanf(fp, " %d %*s %d ", &back, &forwd);
      fscanf(fp, " %*s %d ", &saval);

      flttag->back = address[back];
      if (forwd)
        flttag->forwd = address[forwd];
      else
        flttag->forwd = NULL;

      flttag->saval = saval;
      flttag->dfflst = NULL;

      flttag->prev = head_flt;
      head_flt->next = flttag;
      head_flt = flttag;
    }
  }
  head_flt->next = NULL;
  fclose(fp);
  if (det_flt_num >= MAXFLT)
    printf(" 9914"), exit(0);
  sum_flt = det_flt_num;
}

prn_fltlst(flttag)
    FLT_NODE *flttag;
{
  for (; flttag != NULL; flttag = flttag->next)
  {
    /*     printf(" --- Fault node %x  prev %x next %x\n",flttag,flttag->prev,flttag->next); */
    //printf("\tback %d forward",flttag->back->line);
    printf("%d ", flttag->num);
    //if(flttag->forwd!=NULL)
    //  printf(" %d",flttag->forwd->line);
    //printf(" Stuck-at %d  dfflst %x (fwp)\n",flttag->saval,flttag->dfflst);
  }
}

prn_fltlst_format(flttag)
    FLT_NODE *flttag;
{
  int cnt = 0;
  for (; flttag != NULL; flttag = flttag->next)
  {
    printf(" FLT_NODE %d ->", flttag->back->line);
    cnt++;
    if (cnt > 10000)
    {
      printf(" Fault out interruptted ...\n");
      break;
    }
    if (flttag->forwd != NULL)
      printf(" %d", flttag->forwd->line);
    else
      printf(" 0");
    printf(" SA-VAL %d  \n", flttag->saval);
  }
}

//指定したノードからノードリストの最後まで出力する
prn_subnode(subnode)
    FIN_NODE *subnode;
{
  for (; subnode != NULL; subnode = subnode->next)
  {
    printf(" Line %d >", subnode->node->line);
  }
  printf("|| EOL ||\n");
}

//ノードリストの最初から最後まで出力する
prn_node(head_node)
    L_NODE *head_node;
{
  L_NODE *fnode;
  FIN_NODE *finnode;

  printf(" ~~~ CIRCUIT NODE LIST ~~~\n");
  fnode = head_node;
  for (; fnode != NULL; fnode = fnode->next)
  {
    //printf(" --- Node[%x] %d Type %d next %x prev %x\n",fnode,fnode->line,fnode->type,fnode->next,fnode->prev);
    finnode = fnode->finlst;

    printf("\t in");
    for (; finnode != NULL; finnode = finnode->next)
    {
      printf(" > %d", finnode->node->line);
    }
    printf(" || EOL || ");

    finnode = fnode->foutlst;
    printf("\t out");
    for (; finnode != NULL; finnode = finnode->next)
    {
      printf(" > %d", finnode->node->line);
    }
    printf(" || EOL ||\n");
    /****/
  }
}

readf(argv) char *argv[13];
{
  int i, j;
  FILE *fp;
  char name[30];
  if (NULL == (fp = fopen(argv[1], "r")))
  {
    printf("There is not such a file(%s).\n", argv[1]);
    exit(2);
  }

  fscanf(fp, "%d%d%d%d%d%d", &lpnt, &numout, &inpnum, &slist, &ffnum, &numgate);
  //printf("%d\n", inpnum);
  gate = (ELEMENT *)calloc(lpnt + 1, sizeof(ELEMENT));
  flist = (int *)calloc(slist + 1, sizeof(int));
  pitbl = (int *)calloc(inpnum + 1, sizeof(int));
  potbl = (int *)calloc(numout + 1, sizeof(int));
  fftbl = (int *)calloc(ffnum + 1, sizeof(int));
  ffintbl = (int *)calloc(ffnum + 1, sizeof(int));

  if ((gate == NULL) || (flist == NULL) || (pitbl == NULL) || (potbl == NULL) || (fftbl == NULL) || (ffintbl == NULL))
    printf("memory error @readf\n"), exit(1);

  for (i = 1; i <= lpnt; i++)
  {
    fscanf(fp, "%d", &j);
    gate[i].line = (CHGATA)j;
    fscanf(fp, "%d", &j);
    gate[i].type = (CHGATA)j;
    fscanf(fp, "%d", &j);
    gate[i].nfi = (CHGATA)j;
    fscanf(fp, "%d", &j);
    gate[i].fil = j;
#if NAME
    fscanf(fp, "%s", gate[i].name);
#else
    fscanf(fp, "%s", name);
#endif
    fscanf(fp, "%d", &j);
    gate[i].nfo = (CHGATA)j;

    fscanf(fp, "%d", &j);
    gate[i].fol = j;
  }
  for (i = 1; i <= slist; i++)
  {
    fscanf(fp, "%d%d", &j, &j);
    flist[i] = j;
  }

/* following is to make pilst[], polst[] and fflst[] in seqential circuit */
#if SEQEN
  for (i = 1; i <= inpnum; i++)
  {
    fscanf(fp, "%d%s%d", &j, name, &j);
    pitbl[i] = j;
  }
  for (i = 1; i <= numout; i++)
  {
    fscanf(fp, "%d%s%d", &j, name, &j);
    potbl[i] = j;
  }
  for (i = 1; i <= ffnum; i++)
  {
    fscanf(fp, "%d%d", &j, &j);
    fftbl[i] = j;
    ffintbl[i] = gate[j].fil;
  }
  /* following in using combinational circuit */

#else
  for (i = 1; i <= inpnum; i++)
  {
    fscanf(fp, "%d%s%d", &j, name, &j);
    pitbl[i] = j;
  }
  for (i = 1; i <= numout; i++)
  {
    fscanf(fp, "%d%s%d", &j, name, &j);
    potbl[i] = j;
  }
#endif
  fclose(fp);
}

readf_ehime(argv) char *argv[13];
{
  int i, j;
  FILE *fp;
  char name[30];

  if (NULL == (fp = fopen(argv[1], "r")))
  {
    printf("There is not such a file(%s).\n", argv[1]);
    exit(2);
  }

  fscanf(fp, "%d", &lpnt);

  //gate = (ELEMENT *)calloc(lpnt+1, sizeof(ELEMENT));
  flist = (int *)calloc(lpnt + 1, sizeof(int));

  if ((gate == NULL) || (flist == NULL))
    printf("memory error @readf\n"), exit(1);

  for (i = 1; i <= lpnt; i++)
  {
    fscanf(fp, "%d", &j);
    if (j == 9)
      j = 6;
    gate[i].type = (CHGATA)j;
    fscanf(fp, "%d", &j);
    gate[i].nfi = (CHGATA)j;
    fscanf(fp, "%d", &j);
    gate[i].fil = j;
    fscanf(fp, "%d", &j);
    gate[i].nfo = (CHGATA)j;

    fscanf(fp, "%d", &j);
    gate[i].fol = j;
  }

  fscanf(fp, "%d", &slist);

  for (i = 1; i <= slist; i++)
  {
    fscanf(fp, "%d", &j);
    flist[i] = j;
  }

  /* following is to make pilst[], polst[] and fflst[] in seqential circuit */
  fscanf(fp, "%d", &inpnum);
  //printf("%d\n", inpnum);
  for (i = 1; i <= inpnum; i++)
  {
    fscanf(fp, "%d", &j, &j);
    pitbl[i] = j;
  }
  fscanf(fp, "%d", &numout);
  for (i = 1; i <= numout; i++)
  {
    fscanf(fp, "%d", &j);
    potbl[i] = j;
  }

  ffnum = 0;
  for (i = 1; i <= lpnt; i++)
  {
    if (gate[i].type == FF)
    {
      fftbl[++ffnum] = i;
      ffintbl[ffnum] = gate[i].fil;
    }
  }

  fclose(fp);

  {
    int ni;
    for (ni = 1; ni <= lpnt; ni++)
    {
      printf(" %4d %2d %4d %2d %4d\n", gate[ni].type, gate[ni].nfi, gate[ni].fil, gate[ni].nfo, gate[ni].fol);
    }
  }
}
