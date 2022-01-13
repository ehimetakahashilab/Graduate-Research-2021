#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"
#include <math.h>

drop_flt(num_injgate, injarray, time) int num_injgate;
FLT_NODE *injarray[32];
int time;
{
	FLT_NODE *fgnode;
	FIN_NODE *finnode;
	L_NODE *fnode;
	int ia, ib, ic, mask;
	int num_observe;
	unsigned int signature, OB_CAP_Signature[MAXCAP], signature_mul, signature_full;
#if SELECT_STATION
	unsigned int ob_sel_FF_Signature[FF_FILE];
	for (ic = 0; ic < FF_FILE; ic++)
		ob_sel_FF_Signature[ic] = 0;
#endif

	for (ia = 0; ia < cap_freq; ia++)
		OB_CAP_Signature[ia] = 0;
	/*debug end*/
	signature = 0;
	signature_mul = 0;
	signature_full = 0;

	if (MODE_TOOL == MULTITEST || MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP)
	{ /*Multi-cycle test or Multi-Cycle test with Observation*/
#if FAULTOB
		unsigned int Last_FF_Signature[ffnum], OBFF_Signature[ffnum], OBFF_CAP_Signature[ffnum][cap_freq], First_Cap_Signature = 0, Last_Cap_Signature = 0;
		for (ia = 0; ia < ffnum; ia++)
		{
			Last_FF_Signature[ia] = 0;
			OBFF_Signature[ia] = 0;
			for (ib = 0; ib < cap_freq; ib++)
			{
				OBFF_CAP_Signature[ia][ib] = 0;
			}
		}
#endif


#if PO_OBSERVE
		for (ia = 0; ia < numout; ia++)
		{
			for (ib = 0; ib < cap_freq; ib++)
			{
				signature |= po_observe[ia][ib].gdval1 ^ po_observe[ia][ib].ftval1;
			}
		}
#endif
	}

	finnode = ffnode.next;
	for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
	{
		fnode = finnode->node;
		signature |= fnode->gdval1^fnode->ftval1;
		signature_mul |= fnode->gdval1^fnode->ftval1;
		signature_full |= fnode->gdval1^fnode->ftval1;
		OB_CAP_Signature[cap_freq] |= fnode->gdval1 ^ fnode->ftval1;

#if FAULTOB
	if (MODE_TOOL == MULTITEST || MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP)
		{
			Last_FF_Signature[ia] |= fnode->gdval1 ^ fnode->ftval1;
			Last_Cap_Signature = signature;
		}
	#endif
	}

	if (MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP)
	{

		#if SELECT_STATION
				for (ic = 0; ic < FF_FILE; ic++)
					ob_sel_FF_Signature[ic] |= signature_mul;
		#endif

		finnode = ffnode.next;
		for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
			for (ib = SKIP_CAPTURE; ib < cap_freq; ib++)
			{
				OB_CAP_Signature[ib] |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
				signature_full |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
				if (MODE_TOOL == MULTI_OP)
				{
#if SELECT_STATION
					for (ic = 0; ic < FF_FILE; ic++)
					{
						if (finnode->node->sel_flag[ic])
							ob_sel_FF_Signature[ic] |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;

						signature |= ob_sel_FF_Signature[ic];
					}
#else
					signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
#endif
				}
				if (MODE_TOOL == MULTI_CP)
				{
#if FF_OBSERVE
#if SELECT_STATION
					for (ic = 0; ic < FF_FILE; ic++)
					{
						if (finnode->node->sel_flag[ic])
							ob_sel_FF_Signature[ic] |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;

						signature |= ob_sel_FF_Signature[ic];
					}
#else
					signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
#endif
#endif
				}

#if FAULTOB
				OBFF_Signature[ia] |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
				OBFF_CAP_Signature[ia][ib] = ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1; //キャプチャ毎のFF故障検出状況を見るため_2015710_王
				if (ib == 0)
					First_Cap_Signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
//if(ib<cap_freq-1) OB_Cap_Signature |=ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
#endif
			}
	}

	mask = ITI;

	for (ia = 0; ia < num_injgate; ia++, mask = mask << 1)
	{
		if (signature_full & mask)
		{

			fgnode = injarray[ia];

			if ( MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP)
			{
#if SELECT_STATION
				for (ib = 0; ib < FF_FILE; ib++)
				{
					if (ob_sel_FF_Signature[ib] & mask)
					{
						if (flt_det_flag[fgnode->num][ib+1] == 0)
						{
							flt_det_flag[fgnode->num][ib+1] = 1;

						}
					}
				}

				if (flt_det_flag[fgnode->num][FF_FILE+1] == 0)
				{
					flt_det_flag[fgnode->num][FF_FILE+1] = 1;
					//printf("+++%d\n", fgnode->num);
				}

#endif
			}

		if (signature_mul & mask)
		{
		/*	#if TRANSITIONFAULT
				fgnode->TranDetTimes++;
			#else*/
				fgnode->dtime++;
				if (flt_det_flag[fgnode->num][0] == 0)
				{
					flt_det_flag[fgnode->num][0] = 1;
				}
		//	#endif
		 }

			if (MODE_TOOL == MULTI_OP || MODE_TOOL == MULTI_CP)
			{
				for (ib = 0; ib < cap_freq; ib++)
				{
					if (OB_CAP_Signature[ib] & mask)
					{
						//if(!fgnode->dtime)
						flt_cap[ib] += 1;
						break;
					}
				}
			}

#if DEBUG1 || PRNT_DET_FLT
			printf(" DETECT FAULT %d", fgnode->back->line);
			if (fgnode->forwd != NULL)
				printf(" >> %d", fgnode->forwd->line);
			else
				printf(" >> 0");
			printf(" sa %d \n", fgnode->saval);
#endif

#if OUTPUT_FLIST
			fprintf(flist_out, "1");
			fgnode->detect[time / 32] |= ITI << (time % 32);
			fgnode->dtime++;
#else
if (fgnode->dtime){
//	printf("fltnum=%d \n",fgnode->num);
				if (fgnode->next != NULL)
					fgnode->next->prev = fgnode->prev;
				fgnode->prev->next = fgnode->next;
				injarray[ia] = NULL;
			}
#endif
		}
	}
}

#if FAULTDROP == 0

Full_flt_sim(num_injgate, injarray, time) int num_injgate;
FLT_NODE *injarray[32];
int time;
{
	FLT_NODE *fgnode;
	FIN_NODE *finnode;
	L_NODE *fnode;
	int ia, ib, ic, mask;
	int num_observe;
	unsigned int signature;

	signature = 0;
	/*debug end*/

	if (MODE_TOOL == 2 || MODE_TOOL == 3)
	{ /*Multi-cycle test or Multi-Cycle test with Observation*/
#if FAULTOB
		unsigned int Last_FF_Signature[ffnum], OBFF_Signature[ffnum], OBFF_CAP_Signature[ffnum][cap_freq], First_Cap_Signature = 0, OB_Cap_Signature = 0, Last_Cap_Signature = 0;
		for (ia = 0; ia < ffnum; ia++)
		{
			Last_FF_Signature[ia] = 0;
			OBFF_Signature[ia] = 0;
			for (ib = 0; ib < cap_freq; ib++)
			{
				OBFF_CAP_Signature[ia][ib] = 0;
			}
		}
#endif
		if (MODE_TOOL == 3)
		{
#if PO_OBSERVE
			for (ia = 0; ia < numout; ia++)
			{
				for (ib = 0; ib < cap_freq; ib++)
				{
					signature |= po_observe[ia][ib].gdval1 ^ po_observe[ia][ib].ftval1;
				}
			}
#endif
#if SELECT_STATION
			unsigned int ob_sel_FF_Signature[FF_FILE];
			for (ic = 0; ic < FF_FILE; ic++)
				ob_sel_FF_Signature[ic] = 0;
#endif
		}
	}

	finnode = ffnode.next;
	for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
	{
		fnode = finnode->node;
		signature |= fnode->gdval1 ^ fnode->ftval1;

#if FAULTOB
			Last_FF_Signature[ia] |= fnode->gdval1 ^ fnode->ftval1;
			Last_Cap_Signature = signature;
#endif
			if (MODE_TOOL == 3)
			{
#if SELECT_STATION
			unsigned int ob_sel_FF_Signature[FF_FILE];
				for (ic = 0; ic < FF_FILE; ic++)
					ob_sel_FF_Signature[ic] |= fnode->gdval1 ^ fnode->ftval1;
#endif
			}
	}

	if (MODE_TOOL == MULTI_OP)
	{
		finnode = ffnode.next;
		//printf("aaa\n");//2014_10_21 選択手法を用いたテストでのバグ箇所
		//printf("%d\n",ff_observe[2][0].gdval1);
		//printf("%d\n",ff_observe[2][0].ftval1);
		for (ia = 0; finnode != NULL; finnode = finnode->next, ia++)
			for (ib = SKIP_CAPTURE; ib < cap_freq; ib++)
			{
				//printf("ia=%d ib=%d\n",ia,ib);
#if SELECT_STATION
			unsigned int ob_sel_FF_Signature[FF_FILE];
					for (ic = 0; ic < FF_FILE; ic++)
					{
						if (finnode->node->sel_flag[ic])
						{
							ob_sel_FF_Signature[ic] |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
							signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
						}
					}
#else
					signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
#endif

#if FAULTOB
				OBFF_Signature[ia] |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
				OBFF_CAP_Signature[ia][ib] = ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1; //キャプチャ毎のFF故障検出状況を見るため_2015710_王
				if (ib == 0)
					First_Cap_Signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
				if (ib < cap_freq - 1)
					OB_Cap_Signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
#endif
			}
	}

	mask = ITI;
	for (ia = 0; ia < num_injgate; ia++, mask = mask << 1)
	{
		if (signature & mask)
		{

			fgnode = injarray[ia];

/*#if TRANSITIONFAULT
			fgnode->TranDetTimes++;
#else*/
			fgnode->dtime++;
//#endif


#if DEBUG1 || PRNT_DET_FLT
			printf(" DETECT FAULT %d", fgnode->back->line);
			if (fgnode->forwd != NULL)
				printf(" >> %d", fgnode->forwd->line);
			else
				printf(" >> 0");
			printf(" sa %d \n", fgnode->saval);
#endif

			injarray[ia] = NULL;
		}
	}
}
#endif
