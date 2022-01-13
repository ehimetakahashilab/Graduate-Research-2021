#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"

#define ADD_LPNO 110000
#define ADD_PINO 120000
#define NUM_BIT 32

fault_inject(startflt, injarray)
    FLT_NODE *startflt;
FLT_NODE *injarray[32];
{
  L_NODE *fnode, *ff_node;
  FIN_NODE *foutnode, *finnode;
  FLT_NODE *flttag;
  DINFF *dfflst;
  int ni, iv, ia, fout_cnt, mask;
  L_NODE *inf_back[32], *inj_fwd[32], *new_injback, *new_injfwd;
  int nj;
  //
  //copy_state();
  flttag = startflt->next;
  mask = ITI;
  for (ni = 0; flttag != NULL && ni < NUM_BIT; flttag = flttag->next, ni++, mask = mask << 1)
  {
    new_injback = flttag->back;
    new_injfwd = flttag->forwd;
    inf_back[ni] = flttag->back;
    inj_fwd[ni] = flttag->forwd;

#if DEBUG_NODE
    if (new_injfwd != NULL)
      printf(" FAULT %d->%d SA-%d injected -1094-\n", new_injback->line, new_injfwd->line, flttag->saval);
    else
      printf(" FAULT %d SA-%d injected -1094-\n", new_injback->line, flttag->saval);
#endif
    if (flttag->forwd != NULL)
    {
      finnode = flttag->forwd->finlst;
      for (; finnode != NULL; finnode = finnode->next)
      {
        if (finnode->node == flttag->back)
          break;
      }
      if (finnode == NULL)
      {
        for (nj = ni - 1; nj >= 0; nj--)
        {
          if (inf_back[nj] == flttag->back)
          {
            if (inj_fwd[nj] == flttag->forwd)
            {
              new_injback = &add_gate[nj];
              break;
            }
            else if (inj_fwd[nj] == NULL)
            {
              new_injback = &add_gate[nj];
            }
          }
        }
      }
    }
    /* more than one fan-out and already injected fault */
    /* flttag->back->foutlst==NULL is a case of a PO-fault */
    else if (flttag->back->foutlst != NULL && flttag->back->foutlst->next == NULL)
    {
      new_injfwd = flttag->back->foutlst->node;
    }

    injarray[ni] = flttag;
    fnode = &add_gate[ni];
    fnode->line = ADD_LPNO + ni;
    fnode->addgateflag = 1;
    if (new_injback->type == PI || new_injback->type == FF)
    {
      gnode.next->prev = fnode;
      fnode->next = gnode.next;
      fnode->prev = &gnode;
      gnode.next = fnode;
    }
    else
    {
      fnode->next = new_injback->next;
      fnode->prev = new_injback;
      new_injback->next = fnode;
      if (fnode->next != NULL)
        fnode->next->prev = fnode;
      else if (new_injback->foutlst->node->type != FF && new_injback->foutlst->node->type != PO)
      {
        printf(" line %d flt->back %d error -591-\n", fnode->line, flttag->back->line);
        exit(1);
      }
    }
    /** SET GOOD VALUES **/
    fnode->gdval1 = flttag->back->gdval1;

    /** SET PI NODE VALUES **/
    if (flttag->saval == 0)
    { //initialization values of PI for inseted gates
      fnode->type = AND;
      /*#if TRANSITIONFAULT
//    for(ia=0;ia<cap_freq;ia++){
	add_pi[ni].gdval1 = ALL_F;
	add_pi[ni].ftval1 = ALL_F;
//	}
#else*/
      add_pi[ni].gdval1 = ALL_F;
      add_pi[ni].ftval1 = ~mask;
      //#endif
    }
    else
    {
      fnode->type = OR;
      //#if TRANSITIONFAULT
      //    for(ia=0;ia<cap_freq;ia++){
      //	add_pi[ni].gdval1 = 0;
      //	add_pi[ni].ftval1 = 0;

      //	}
      //#else
      add_pi[ni].gdval1 = 0;
      add_pi[ni].ftval1 = mask;
      //#endif
    }
    fnode->line = -flttag->line;

    /** SET FAULTY VALUES TO FFS **/
    dfflst = flttag->dfflst;
    for (; dfflst != NULL; dfflst = dfflst->next)
    {
      ff_node = dfflst->node;
#if DEBUG1
      if (flttag->back->line == 24)
        printf(" FF %d set faulty value\n", ff_node->line);
#endif
      if (dfflst->val == 1)
      {
        //for(iv=0;iv<=cap_freq;iv++)
        //	ff_node->ftval0[iv] |= mask;
        ff_node->ftval1 |= mask;
      }
      else if (dfflst->val == 0)
      {
        //for(iv=0;iv<=cap_freq;iv++)
        //	ff_node->ftval0[iv] &= ~mask;
        // ff_node->ftval0 &= ~mask;
        ff_node->ftval1 &= ~mask;
      }
    }
    /** SET PI NODE **/
    /*#if TRANSITIONFAULT

     add_pi[ni].type=PI;
      add_pi[ni].line=ADD_PINO+ni;
      add_pi[ni].finlst=NULL;
      add_pi[ni].foutlst=&pi_fout[ni];
      pi_fout[ni].node=fnode;
      pi_fout[ni].next=NULL;

   fnode->finlst=&add_fin[0][ni];
    for(ia=0;ia<=cap_freq;ia++){
	if(ia<cap_freq){
          add_fin[ia][ni].node=&add_pi[ni];
          add_fin[ia][ni].next=&add_fin[ia+1][ni];
		}
   	else{
    	  add_fin[ia][ni].node=new_injback;
      	  add_fin[ia][ni].next=NULL;
		}
	}
#else*/

    add_pi[ni].type = PI;
    add_pi[ni].line = ADD_PINO + ni;
    add_pi[ni].finlst = NULL;
    add_pi[ni].foutlst = &pi_fout[ni];
    pi_fout[ni].node = fnode;
    pi_fout[ni].next = NULL;

    fnode->finlst = &add_fin[0][ni];
    add_fin[0][ni].node = &add_pi[ni];
    add_fin[0][ni].next = &add_fin[1][ni];
    add_fin[1][ni].node = new_injback;
    add_fin[1][ni].next = NULL;
    //#endif

    /** ARRANGE FAN-IN FAN-OUT LISTS **/
    if (new_injfwd != NULL)
    {
      fnode->foutlst = &add_fout[ni];
      add_fout[ni].node = new_injfwd;
      add_fout[ni].next = NULL;

      finnode = new_injfwd->finlst;
      for (; finnode != NULL; finnode = finnode->next)
      {
        if (finnode->node == new_injback)
        {
          finnode->node = fnode;
          break;
        }
      }
      if (finnode == NULL)
      {
        printf(" error ni %d  -214-\n", ni), exit(1);
      }

      foutnode = new_injback->foutlst;
      for (; foutnode != NULL; foutnode = foutnode->next)
      {
        if (foutnode->node == new_injfwd)
        {
          foutnode->node = fnode;
          break;
        }
      }
    }
    else
    {
      fnode->foutlst = new_injback->foutlst;
      new_injback->foutlst = &add_fout[ni];
      add_fout[ni].node = fnode;
      add_fout[ni].next = NULL;
      foutnode = fnode->foutlst;
      for (; foutnode != NULL; foutnode = foutnode->next)
      {
        finnode = foutnode->node->finlst;
        for (; finnode != NULL; finnode = finnode->next)
        {
          if (finnode->node != flttag->back)
            continue;
          finnode->node = fnode;
          break;
        }
      }
    }
  }
  startflt->next = flttag;
  return ni;
}

clear_addgate(num_injgate, add_gate, add_pi)
    L_NODE add_gate[32],
    add_pi[32];
int num_injgate;
{
  int ni;
  L_NODE *fnode, *orig_ingnode;
  FIN_NODE *foutnode, *finnode;

  for (ni = 0; ni < num_injgate; ni++)
  {
    fnode = &add_gate[ni];
    fnode->addgateflag = 0;
    if (fnode->next != NULL)
      fnode->next->prev = fnode->prev;
    fnode->prev->next = fnode->next;

    finnode = fnode->finlst;
    for (; finnode != NULL; finnode = finnode->next)
    {
      if (finnode->node == &add_pi[ni])
        continue;
      orig_ingnode = finnode->node;
      break;
    }

    if (fnode->foutlst != NULL && fnode->foutlst->next == NULL)
    { /** only 1 fan-out **/
      foutnode = orig_ingnode->foutlst;
      for (; foutnode != NULL; foutnode = foutnode->next)
      {
        if (foutnode->node == fnode)
        {
          foutnode->node = fnode->foutlst->node;
          break;
        }
      }
    }
    else
    { /** more than 1 fan-out or PO **/
      orig_ingnode->foutlst = fnode->foutlst;
    }
    foutnode = fnode->foutlst;
    for (; foutnode != NULL; foutnode = foutnode->next)
    {
      finnode = foutnode->node->finlst;
      for (; finnode != NULL; finnode = finnode->next)
      {
        if (finnode->node == fnode)
        {
          finnode->node = orig_ingnode;
          break;
        }
      }
    }
  }
}

#if TRANSITIONFAULT
copy_state()
{
  L_NODE *ff_node;
  FIN_NODE *finnode;
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    ff_node = finnode->node;
    ff_node->ftval1 = ff_node->gdval1;
  }
}
launch_initial()
{
  L_NODE *ff_node;
  FIN_NODE *finnode;
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    ff_node = finnode->node;
    ff_node->ftval1 = ff_node->la_val;//set ftval to the Launch value(good value)
  }
}

launch_copy()
{
  L_NODE *ff_node;
  FIN_NODE *finnode;
  finnode = ffnode.next;
  for (; finnode != NULL; finnode = finnode->next)
  {
    ff_node = finnode->node;
    ff_node->la_val = ff_node->gdval1;//set ftval to the Launch value(good value)
  }
}
#endif
