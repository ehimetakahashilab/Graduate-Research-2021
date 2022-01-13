#include"declare.h"
#include"def_gtype.h"
#include"def_flt.h"
#include <math.h>

SO_Control_line(ff_select)
char ff_select[ffnum];
{
	int ia,ib,ic,ie;
	int count=0, unccount=0;
	int select_FF[ffnum];
	int tmp;
int unchangeFF_ratio=0.01*30000;
ib=0;//printf("%d \n",ib);
unccount=schain[0].length*UNCFFRATE;
	for(ia=0;ia<ffnum; ia++){
		if(ia==schain[ib+1].top){
				ib++;
				unccount=schain[ib].length*UNCFFRATE;
				//printf("\n%d ",ib);
					}
		if(SO_Reduction_Capa[ia]==0) select_FF[ia]=0;
		else{count=0;//mp=SO_Reduction_Capa[ia];
		for(ic=1;ic<schain[ib].length;ic++){
		if(SO_Reduction_Capa[ia]<SO_Reduction_Capa[ic+schain[ib].top]) count++;
		//if(changed_freq[ia]<=unchangeFF_ratio&&SO_Reduction_Capa[ia]>10000) select_FF[ia]=1;
			}
			if(count<unccount) select_FF[ia]=1;
			else select_FF[ia]=0;
		}
		//else select_FF[ia]=0;
		for(ic=0;ic<3;ic++){
		FF_flag[ia][ic]=0;

			}
		//printf("%d ",select_FF[ia]);
		}
/*for(ia=0;ia<ffnum; ia++){
	if(changed_freq[ia]<=unchangeFF_ratio&&SO_Reduction_Capa[ia]>10000) select_FF[ia]=1;
else select_FF[ia]=0;
	for(ic=0;ic<3;ic++){
		FF_flag[ia][ic]=0;

			}
		printf("%d ",select_FF[ia]);
		}*/
if(SOControlflag==1){
	if(SoControlMode==0){
		ib=0;
	unccount=schain[0].length*UNCFFRATE;
	count=0;
	for(ia=0;ia<ffnum; ia++){
		if(ia==schain[ib+1].top){
				ib++;count=0;
				unccount=schain[ib].length*UNCFFRATE;
					}
		if(ff_select[ia] == '0'&&count<unccount&&select_FF[ia]==1){
			//if(ff_select[ia+1] == '0'){
				FF_flag[ia][0]=2;//FF_flag[ia][1]=1;//FF_flag[ia][3]=ia+1;
				//FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
				count++;
				//}
			}
		else if(ff_select[ia] == '1')
			FF_flag[ia][0]=2;
			}
		}
	else{
	ib=0;
	for(ia=0;ia<ffnum; ia++){
	if(ia==schain[ib+1].top&&ib<chainnum) ib++;
		if(ia<schain[ib].top+3){
			if(ia==schain[ib].top&&ff_select[ia] == '1'&&ff_select[ia+1] == '1'&&ff_select[ia+2] == '1'){
				 FF_flag[ia][0]=1; FF_flag[ia][1]=2;//FF_flag[ia][2]=ia+2;
				 FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
				 FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
				}
			else if(ia==schain[ib].top&&ff_select[ia] == '0'&&ff_select[ia+1] == '1') {
					FF_flag[ia][0]=1;FF_flag[ia][1]=1;
					FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
					if(ff_select[ia+2] == '1'){
						FF_flag[ia][1]++;	//FF_flag[ia][3]=ia+1;FF_flag[ia][4]=ia+2;
						//FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
						FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;

					}
				}
			if(ia==schain[ib].top+1&&ff_select[ia-1] == '1'&&ff_select[ia] == '0'){
					FF_flag[ia][0]=1; FF_flag[ia][1]=1;//FF_flag[ia][2]=ia-1;FF_flag[ia-1][0]=2;
					FF_flag[ia-1][0]=2;FF_flag[ia-1][1]=ia;
					if(ff_select[ia+1] == '1'){
						FF_flag[ia][1]++;
						FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
						if(ff_select[ia+2] == '1'){
							FF_flag[ia][1]++;	//FF_flag[ia][3]=ia+1;FF_flag[ia][4]=ia+2;
							//FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
							FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
							}
						}
				}
			if(ia==schain[ib].top+2&&ff_select[ia-1] == '1'&&ff_select[ia-2] == '1'&&ff_select[ia] == '0'){
					FF_flag[ia][0]=1; FF_flag[ia][1]=2;	//FF_flag[ia][1]=ia-2;FF_flag[ia][2]=ia-1;
					FF_flag[ia-1][0]=2;FF_flag[ia-1][1]=ia;
					FF_flag[ia-2][0]=2;FF_flag[ia-2][1]=ia;
					if(ff_select[ia+1] == '1'){
						FF_flag[ia][1]++; //FF_flag[ia][3]=ia+1;FF_flag[ia+1][0]=2;
						FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;

						if(ff_select[ia+2] == '1'){
							FF_flag[ia][1]++; //FF_flag[ia][3]=ia+1;FF_flag[ia][4]=ia+2;
							//FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
							FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
							}
					}
				}
			}
		else {
			if(ff_select[ia] == '0'&&FF_flag[ia][0]==0){
				if(ff_select[ia-1] == '1'&&ff_select[ia-2] == '1'&&ff_select[ia-3] == '1'&&(FF_flag[ia-3][0]==2&&FF_flag[ia-2][0]!=1)){
					FF_flag[ia][0]=1; FF_flag[ia][1]=1;
					FF_flag[ia-1][0]=2;FF_flag[ia-1][1]=ia;
					}
				if(((ib+1<chainnum&&ia+1<schain[ib+1].top)||(ib+1>=chainnum&&ia+1<ffnum))&&ff_select[ia+1] == '1') {
					FF_flag[ia][0]=1;FF_flag[ia][1]++; //FF_flag[ia][3]=ia+1;
					FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
					if(((ib+1<chainnum&&ia+2<schain[ib+1].top)||(ib+1>=chainnum&&ia+2<ffnum))&&ff_select[ia+2] == '1'){
						FF_flag[ia][1]++;//FF_flag[ia][4]=ia+2;
						FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
							}
						}
					}

			else if(ff_select[ia] == '1'&&FF_flag[ia][0]==0){
				if(ff_select[ia-1] == '1'&&ff_select[ia-2] == '1'&&ff_select[ia+1] == '1'&&FF_flag[ia-1][0]==2&&FF_flag[ia-2][0]==2&&((ib+1<chainnum&&ia+1<schain[ib+1].top)||(ib+1>=chainnum&&ia+1<ffnum))){
				FF_flag[ia][0]=1;FF_flag[ia][1]=1;//FF_flag[ia][3]=ia+1;
				FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
				if(((ib+1<chainnum&&ia+2<schain[ib+1].top)||(ib+1>=chainnum&&ia+2<ffnum))&&ff_select[ia+2] == '1'){
					FF_flag[ia][1]++;//FF_flag[ia][4]=ia+2;
					FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
					}
				}


			}
		}
	}


	if(SoControlMode==1){

		ib=0;
	unccount=schain[0].length*UNCFFRATE;
	count=0;
	for(ia=0;ia<ffnum; ia++){
		if(ia==schain[ib+1].top){
				ib++;count=0;
				unccount=schain[ib].length*UNCFFRATE;
					}
		if(ff_select[ia] == '0'&&FF_flag[ia][0]!=2&&count<unccount){
			if(ia==schain[ib].top){
				if(changed_freq[ia]<=unchangeFF_ratio&&ff_select[ia+1] == '0'&&select_FF[ia]==0&&select_FF[ia+1]==1&&FF_flag[ia+1][0]==0){
					 FF_flag[ia][0]=1; FF_flag[ia][1]++;//FF_flag[ia][2]=ia+2;
				 	FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
				 	count++;
						}
				else if(select_FF[ia]==1&&(changed_freq[ia+1]>unchangeFF_ratio||select_FF[ia+1]==0)&&FF_flag[ia+1][0]!=2) {
					FF_flag[ia][0]=2;FF_flag[ia][1]=ia+1;
					FF_flag[ia+1][0]=1;FF_flag[ia+1][1]=1;
					count++;
						}
				}

			else{


		if(FF_flag[ia][0]!=2){
				if(FF_flag[ia][0]==1){
					if(select_FF[ia+1]==1&&FF_flag[ia+1][0]==0){
					//if(FF_flag[ia+1][0]!=2)
						FF_flag[ia][0]=1; FF_flag[ia][1]++;
						FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
						count++;
						}
					}
				else if(FF_flag[ia][0]==0){
					if(select_FF[ia]==1&&FF_flag[ia+1][0]!=2){
						if(select_FF[ia+1]==0&&FF_flag[ia+1][0]==0){
					//if(FF_flag[ia+1][0]!=2)
							FF_flag[ia+1][0]=1; FF_flag[ia+1][1]=1;
							FF_flag[ia][0]=2;FF_flag[ia][1]=ia+1;
							count++;
							}
						else if(FF_flag[ia+1][0]==1){
					//if(FF_flag[ia+1][0]!=2)
							FF_flag[ia+1][0]=1; FF_flag[ia+1][1]++;
							FF_flag[ia][0]=2;FF_flag[ia][1]=ia+1;
							count++;
							}

						}
					else if(select_FF[ia]==0&&FF_flag[ia+1][0]==0){
						if(select_FF[ia+1]==1){
							FF_flag[ia][0]=1; FF_flag[ia][1]=1;
							FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
							count++;
							}
						}
					}
					}
				}
			}
		}
		}
	else{
		ib=0;
	unccount=schain[0].length*UNCFFRATE;
	count=0;
	for(ia=0;ia<ffnum; ia++){
		if(ia==schain[ib+1].top){
				ib++;count=0;
				unccount=schain[ib].length*UNCFFRATE;
					}
		if(ff_select[ia] == '0'&&count<unccount){
			if(ff_select[ia+1] == '0'){
				FF_flag[ia][0]=2;//FF_flag[ia][1]=1;//FF_flag[ia][3]=ia+1;
				//FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
				count++;
						}
					}
				}
			}
		}
		}
	}

/*wiring program as only Control OBFF*/
SO_Control_line1(ff_select)
char ff_select[ffnum];
{
	int ia,ib,ic,ie;
	int count=0, unccount=0;
	int select_FF[ffnum];
	int tmp;
for(ia=0;ia<ffnum;ia++){FF_flag[ia][0]=0;FF_flag[ia][1]=0;}
if(SOControlflag==1){
	if(SoControlMode==0){
		for(ia=0;ia<ffnum;ia++){
			if(ff_select[ia]=='1')
				FF_flag[ia][0]=2;
				}
			}
	else if(SoControlMode==1){

	ib=0;
	for(ia=0;ia<ffnum; ia++){
	if(ia==schain[ib+1].top&&ib<chainnum) ib++;
		if(ia<schain[ib].top+3){
			if(ia==schain[ib].top&&ff_select[ia] == '1'&&ff_select[ia+1] == '1'&&ff_select[ia+2] == '1'){
				 FF_flag[ia][0]=1; FF_flag[ia][1]=2;//FF_flag[ia][2]=ia+2;
				 FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
				 FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
				}
			else if(ia==schain[ib].top&&ff_select[ia] == '0'&&ff_select[ia+1] == '1') {
					FF_flag[ia][0]=1;FF_flag[ia][1]=1;
					FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
					if(ff_select[ia+2] == '1'){
						FF_flag[ia][1]++;	//FF_flag[ia][3]=ia+1;FF_flag[ia][4]=ia+2;
						//FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
						FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;

					}
				}
			if(ia==schain[ib].top+1&&ff_select[ia-1] == '1'&&ff_select[ia] == '0'){
					FF_flag[ia][0]=1; FF_flag[ia][1]=1;//FF_flag[ia][2]=ia-1;FF_flag[ia-1][0]=2;
					FF_flag[ia-1][0]=2;FF_flag[ia-1][1]=ia;
					if(ff_select[ia+1] == '1'){
						FF_flag[ia][1]++;
						FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
						if(ff_select[ia+2] == '1'){
							FF_flag[ia][1]++;	//FF_flag[ia][3]=ia+1;FF_flag[ia][4]=ia+2;
							//FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
							FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
							}
						}
				}
			if(ia==schain[ib].top+2&&ff_select[ia-1] == '1'&&ff_select[ia-2] == '1'&&ff_select[ia] == '0'){
					FF_flag[ia][0]=1; FF_flag[ia][1]=2;	//FF_flag[ia][1]=ia-2;FF_flag[ia][2]=ia-1;
					FF_flag[ia-1][0]=2;FF_flag[ia-1][1]=ia;
					FF_flag[ia-2][0]=2;FF_flag[ia-2][1]=ia;
					if(ff_select[ia+1] == '1'){
						FF_flag[ia][1]++; //FF_flag[ia][3]=ia+1;FF_flag[ia+1][0]=2;
						FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;

						if(ff_select[ia+2] == '1'){
							FF_flag[ia][1]++; //FF_flag[ia][3]=ia+1;FF_flag[ia][4]=ia+2;
							//FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
							FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
							}
					}
				}
			}
		else {
			if(ff_select[ia] == '0'&&FF_flag[ia][0]==0){
				if(ff_select[ia-1] == '1'&&ff_select[ia-2] == '1'&&ff_select[ia-3] == '1'&&(FF_flag[ia-3][0]==2&&FF_flag[ia-2][0]!=1)){
					FF_flag[ia][0]=1; FF_flag[ia][1]=1;
					FF_flag[ia-1][0]=2;FF_flag[ia-1][1]=ia;
					}
				if(((ib+1<chainnum&&ia+1<schain[ib+1].top)||(ib+1>=chainnum&&ia+1<ffnum))&&ff_select[ia+1] == '1') {
					FF_flag[ia][0]=1;FF_flag[ia][1]++; //FF_flag[ia][3]=ia+1;
					FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
					if(((ib+1<chainnum&&ia+2<schain[ib+1].top)||(ib+1>=chainnum&&ia+2<ffnum))&&ff_select[ia+2] == '1'){
						FF_flag[ia][1]++;//FF_flag[ia][4]=ia+2;
						FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
							}
						}
					}

			else if(ff_select[ia] == '1'&&FF_flag[ia][0]==0){
				if(ff_select[ia-1] == '1'&&ff_select[ia-2] == '1'&&ff_select[ia+1] == '1'&&FF_flag[ia-1][0]==2&&FF_flag[ia-2][0]==2&&((ib+1<chainnum&&ia+1<schain[ib+1].top)||(ib+1>=chainnum&&ia+1<ffnum))){
				FF_flag[ia][0]=1;FF_flag[ia][1]=1;//FF_flag[ia][3]=ia+1;
				FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
				if(((ib+1<chainnum&&ia+2<schain[ib+1].top)||(ib+1>=chainnum&&ia+2<ffnum))&&ff_select[ia+2] == '1'){
					FF_flag[ia][1]++;//FF_flag[ia][4]=ia+2;
					FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
					}
				}


			}
		}
	}

}
else if(SoControlMode==2){
	ib=0;count=schain[ib].length;
	for(ia=0;ia<ffnum; ia++){
	if(ia==schain[ib+1].top&&ib<chainnum-1){ ib++;count+=schain[ib].length; }
		if( FF_flag[ia][0]!=0) continue;
		if(ia<count-3&&ia<ffnum-3){
			if(ff_select[ia] == '1'&&ff_select[ia+1] == '1'&&ff_select[ia+2] == '1'){
				 FF_flag[ia][0]=3;
				 FF_flag[ia+1][0]=3;
				 FF_flag[ia+2][0]=3;
						}
			else if(ff_select[ia] == '0'&&ff_select[ia+1] == '1'){
				if(ff_select[ia+2] == '0'){
					FF_flag[ia][0]=1;FF_flag[ia][1]+=1;
					FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
						}
				else if(ff_select[ia+2] == '1'&&ff_select[ia+3] == '0'){
					FF_flag[ia][0]=1;FF_flag[ia][1]=2;
					FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
					FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
						}
				else if(ff_select[ia+2] == '1'&&ff_select[ia+3] == '1'){
					FF_flag[ia+1][0]=3;
					FF_flag[ia+2][0]=3;
					FF_flag[ia+3][0]=3;
						}
					}
			else if(FF_flag[ia][0]==0&&ff_select[ia] == '1'&&ff_select[ia-1] == '1'&&ff_select[ia-2] == '1')	{
					 FF_flag[ia][0]=3;
					}
				}
		else {
			if(FF_flag[ia][0]==0&&ff_select[ia] == '1'&&ff_select[ia-1] == '1'&&ff_select[ia-2] == '1')FF_flag[ia][0]=3;
			else if(FF_flag[ia][0]==0&&ff_select[ia] == '0'&&ff_select[ia+1] == '1'&&ia+1<schain[ib+1].top&&ia+1<ffnum){
				if(ff_select[ia+2] == '0'&&ia+2<schain[ib+1].top&&ia+2<ffnum){
					FF_flag[ia][0]=1;FF_flag[ia][1]+=1;
					FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
						}
				else if(ff_select[ia+2] == '1'&&ia+2<schain[ib+1].top&&ia+2<ffnum){
					FF_flag[ia][0]=1;FF_flag[ia][1]=2;
					FF_flag[ia+1][0]=2;FF_flag[ia+1][1]=ia;
					FF_flag[ia+2][0]=2;FF_flag[ia+2][1]=ia;
							}
						}
					}
				}
			}
		}


}




HighFreqFilter(ff_state)
int ff_state[ffnum];
{
	int ia,ib=0;

	for(ia=0;ia<ffnum; ia++){
	FF_flag[ia][2]=0;
		if(ia==schain[ib+1].top){
				ib++;
				//unccount=schain[ib].length*UNCFFRATE;
					}
		if(ia==schain[ib].top){
			if(FF_flag[ia][0]==2){
				if(ff_state[ia+1]!=FIXVALUE)
					FF_flag[ia][2]=0;
				else	FF_flag[ia][2]=1;
				}
			}
		else{
			if(FF_flag[ia][0]==2){
				if(ff_state[ia-1]!=FIXVALUE&&ff_state[ia+1]!=FIXVALUE&&ia+1<schain[ib+1].top)
					FF_flag[ia][2]=0;
				else	FF_flag[ia][2]=1;
				}
			}

	}


}

HighFreAreaDet(ff_state)
int ff_state[ffnum];
{
int ia,ib,iB,iF;
int toggle,Max_toggle;

	ib=0;
	for(ia=0;ia<ffnum; ia++){
		toggle=0;
		Max_toggle=0;
		if(ia==schain[ib+1].top&&ib<chainnum) ib++;

		if(ia-OFFSET<schain[ib].top) iB=schain[ib].top;
		else iB=ia-OFFSET;
		if(ia+OFFSET>=schain[ib+1].top) iF=schain[ib+1].top-1;
		else iF=ia+OFFSET;

		for(;iB<iF;iB++){
			if(ff_state[iB]!=ff_state[iB+1]){
				toggle++;
				//if(iB+2<=iF&&ff_state[iB]==ff_state[iB+2])
				//toggle++;
				}
			Max_toggle++;
			}
	ff_toggle_density[ia]+=(float)toggle/Max_toggle;
}
}


Toggle_reduction_capability(ff_state,ff_select)
int ff_state[ffnum];
char ff_select[ffnum];
{
	int ia,ib=0;

	for(ia=0;ia<ffnum; ia++){
		//FF_flag[ia][3]=0;
		if(ia==schain[ib+1].top)	ib++;

		if(ia==schain[ib].top){
			if(changed_freq[ia]<=300&&ff_select[ia]=='0'&&FaultyFreeflag[ia]==0){
				if(ff_state[ia+1]!=ff_state[ia])
					FF_flag[ia][3]++;
				}
			}
		else{
			if(changed_freq[ia]<=300&&ff_select[ia]=='0'&&FaultyFreeflag[ia]==0){
				if(ff_state[ia-1]!=ff_state[ia])
					FF_flag[ia][3]++;
				}
			}

	}


}

//drop_flt with SO control

drop_flt_SAF(num_injgate, injarray, time,ff_select,ff_state)
int num_injgate;
FLT_NODE *injarray[32];
int time;
char ff_select[ffnum];
int ff_state[ffnum];
{
  FLT_NODE *fgnode;
  FIN_NODE *finnode,*foutnode;
  L_NODE *fnode;
  int ia, ib, ic, mask,flag,count=100000;
  int num_observe;
int flag1,flag2,flag3,flag4;
  unsigned int signature;
unsigned int FF_Signature[ffnum],OBFF_Signature[ffnum];

num_observe = ffnum * OBSERVE_RATE;


  signature=0;
#if FAULTOB
for(ia=0;ia<ffnum;ia++){
FF_Signature[ia]=0;
OBFF_Signature[ia]=0;
}

#endif


#if PO_OBSERVE
  for(ia=0; ia<numout; ia++){
    for(ib=0; ib<cap_freq; ib++){
      signature |= po_observe[ia][ib].gdval1 ^ po_observe[ia][ib].ftval1;
    }
  }
#endif

if(SOControlflag==1){
	if(SoControlMode==0){
  finnode=ffnode.next;
  for(ia=0;finnode!=NULL;finnode=finnode->next, ia++){
	fnode=finnode->node;

	if(FF_flag[ia][0]==2){
		signature |= 0;
		}
	else signature |= fnode->gdval1 ^ fnode->ftval1;

	}
}
else if(SoControlMode==1){

  	finnode=ffnode.next;
  for(ia=0;finnode!=NULL;finnode=finnode->next, ia++){
	fnode=finnode->node;
	if(FF_flag[ia][0]==2)
		signature |= initial_ff_state[0][FF_flag[ia][1]]^initial_ff_state[1][FF_flag[ia][1]];
	else signature |= fnode->gdval1 ^ fnode->ftval1;
}
}

else if(SoControlMode==2){

  	finnode=ffnode.next;
  for(ia=0;finnode!=NULL;finnode=finnode->next, ia++){
	fnode=finnode->node;
	if(FF_flag[ia][0]==3) signature |= 0;
	else if(FF_flag[ia][0]==2)
		signature |= initial_ff_state[0][FF_flag[ia][1]]^initial_ff_state[1][FF_flag[ia][1]];
	else signature |= fnode->gdval1 ^ fnode->ftval1;
		}

	}
}
else{

  finnode=ffnode.next;
  for(ia=0 ;finnode!=NULL;finnode=finnode->next,ia++){
    fnode=finnode->node;
    	signature |= fnode->gdval1 ^ fnode->ftval1;
#if FAULTOB
 	FF_Signature[ia]|= fnode->gdval1 ^ fnode->ftval1;
#endif
  }

}

#if FF_OBSERVE
 #if FAULTOB
 finnode=ffnode.next;
  for(ia=0 ;finnode!=NULL;finnode=finnode->next,ia++){
  //for(ia=0; ia<ffnum; ia++){
    for(ib=SKIP_CAPTURE; ib<cap_freq; ib++){
	signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
	OBFF_Signature[ia]|= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
		}
//printf(" %x %x\n",signature,FF_Signature[ia]);
	}//exit(1);
#endif
#if SELECT_STATION
  for(ia=0; ia<num_observe; ia++)
    for(ib=SKIP_CAPTURE; ib<cap_freq; ib++)
	signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
#else
  for(ia=0; ia<ffnum; ia++)
    for(ib=SKIP_CAPTURE; ib<cap_freq; ib++)
	signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
#endif
#endif


//printf(" signature=%x \n",signature);
  mask=ITI;

  for(ia=0; ia<num_injgate; ia++, mask=mask<<1){
   if(signature&mask){
      fgnode = injarray[ia];
	      fgnode->dtime++;
#if DEBUG1 || PRNT_DET_FLT
      printf(" DETECT FAULT %d",fgnode->back->line);
      if(fgnode->forwd!=NULL)
	printf(" >> %d",fgnode->forwd->line);
      else
	printf(" >> 0");
      printf(" sa %d \n",fgnode->saval);
#endif


#if OUTPUT_FLIST
      fprintf(flist_out, "1");
      fgnode->detect[time/32]|=ITI<<(time%32);
      fgnode->dtime++;
#else
#if FAULTOB
 finnode=ffnode.next;
 for(ib=0 ;finnode!=NULL;finnode=finnode->next,ib++){
//for(ib=0;ib<ffnum;ib++){

	if(FF_Signature[ib]&mask){
				//if(FF_Fault_List[ib][fgnode->num]==0){FF_Fault_List[ib][0]++;FF_Fault_List[ib][fgnode->num]=1;}
				flag=0;
				for(ic=1;ic<=FF_SA_Fault_List[ib][0];ic++){
					if(FF_SA_Fault_List[ib][ic]==fgnode->num){flag=1;break; }
					}
				if(flag==0){
				FF_SA_Fault_List[ib][0]++;
				FF_SA_Fault_List[ib][FF_SA_Fault_List[ib][0]]=fgnode->num;
					}

				}


	if(OBFF_Signature[ib]&mask){
			//if(Ob_FF_Faults[ib][fgnode->num]==0){Ob_FF_Faults[ib][0]++;Ob_FF_Faults[ib][fgnode->num]=1;}
					flag=0;
				for(ic=1;ic<=Ob_FF_SA_Faults[ib][0];ic++){
					if(Ob_FF_SA_Faults[ib][ic]==fgnode->num){flag=1;break; }
					}
				if(flag==0){
					Ob_FF_SA_Faults[ib][0]++;
					Ob_FF_SA_Faults[ib][Ob_FF_SA_Faults[ib][0]]=fgnode->num;
						}

			}


	}
#endif
     if(fgnode->next!=NULL)
      fgnode->next->prev=fgnode->prev;
    fgnode->prev->next=fgnode->next;
      injarray[ia]=NULL;

#endif

  //  }

}

  }//exit(1);
}


/*For Transition Fault detection*/

drop_flt_TRF(num_injgate, injarray, time,ff_select,ff_state)
int num_injgate;
FLT_NODE *injarray[32];
int time;
char ff_select[ffnum];
int ff_state[ffnum];
{
  FLT_NODE *fgnode;
  FIN_NODE *finnode,*foutnode;
  L_NODE *fnode;
  int ia, ib, ic, ie,iOB,mask,flag,count=100000;
  int num_observe;
int flag1,flag2,flag3,flag4,saflag,GdEvent[cap_freq],FtEvent[cap_freq];
  unsigned int signature,signature1;
int Tra_Faulty_state1,Tra_Faulty_state2,Tra_Fltfree_state1,Tra_Fltfree_state2;
unsigned int FF_Signature[ffnum],OBFF_Signature[ffnum][cap_freq],All_OB_FF_Signature[ffnum],OBSignature[cap_freq];

num_observe = ffnum * OBSERVE_RATE;

  signature=0;   signature1=0;
for(ia=0;ia<ffnum;ia++)OBSignature[ia]=0;
#if FAULTOB
for(ia=0;ia<ffnum;ia++){
FF_Signature[ia]=0;All_OB_FF_Signature[ia]=0;
for(ib=0;ib<cap_freq;ib++){
OBFF_Signature[ia][ib]=0;
if(ia==0)OBSignature[ib]=0;
}
}

#endif

#if PO_OBSERVE
  for(ia=0; ia<numout; ia++){
    for(ib=0; ib<cap_freq; ib++){
      signature |= po_observe[ia][ib].gdval1 ^ po_observe[ia][ib].ftval1;
    }
  }
#endif
if(SOControlflag==1){
	if(SoControlMode==0){
  finnode=ffnode.next;
  for(ia=0;finnode!=NULL;finnode=finnode->next, ia++){
	fnode=finnode->node;

	if(FF_flag[ia][0]==2){
		signature |= 0;
		signature1 |= 0;
		}
	else{
		signature |= fnode->gdval1 ^ fnode->ftval1;
	        signature1 |= fnode->gdval1 ^ fnode->ftval1;
		}

	}
}
else if(SoControlMode==1){

  	finnode=ffnode.next;
  for(ia=0;finnode!=NULL;finnode=finnode->next, ia++){
	fnode=finnode->node;
	if(FF_flag[ia][0]==2){
		signature |= initial_ff_state[0][FF_flag[ia][1]]^initial_ff_state[1][FF_flag[ia][1]];
		signature1 |= initial_ff_state[0][FF_flag[ia][1]]^initial_ff_state[1][FF_flag[ia][1]];
		}
	else{
		signature |= fnode->gdval1 ^ fnode->ftval1;
	        signature1 |= fnode->gdval1 ^ fnode->ftval1;
		}
}
}

else if(SoControlMode==2){

  	finnode=ffnode.next;
  for(ia=0;finnode!=NULL;finnode=finnode->next, ia++){
	fnode=finnode->node;
	if(FF_flag[ia][0]==3){
		signature |= 0;
		signature1 |= 0;
		}
	else if(FF_flag[ia][0]==2){
		signature |= initial_ff_state[0][FF_flag[ia][1]]^initial_ff_state[1][FF_flag[ia][1]];
		signature1 |= initial_ff_state[0][FF_flag[ia][1]]^initial_ff_state[1][FF_flag[ia][1]];
		}
	else{
		signature |= fnode->gdval1 ^ fnode->ftval1;
	     signature1 |= fnode->gdval1 ^ fnode->ftval1;
		}
		}

	}
}
else{

  finnode=ffnode.next;
  for(ia=0 ;finnode!=NULL;finnode=finnode->next,ia++){
    fnode=finnode->node;
    	signature |= fnode->gdval1 ^ fnode->ftval1;
    	signature1 |= fnode->gdval1 ^ fnode->ftval1;
#if FAULTOB
 	FF_Signature[ia]|= fnode->gdval1 ^ fnode->ftval1;
#endif
  }

}

#if FF_OBSERVE
 #if FAULTOB
 finnode=ffnode.next;
  for(ia=0 ;finnode!=NULL;finnode=finnode->next,ia++){
  //for(ia=0; ia<ffnum; ia++){
    for(ib=SKIP_CAPTURE; ib<cap_freq; ib++){
	OBSignature[ib]|= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
	signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
	OBFF_Signature[ia][ib]= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
	All_OB_FF_Signature[ia]|=ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
		}
//printf(" %x %x\n",signature,FF_Signature[ia]);
	}//exit(1);
#endif
#if SELECT_STATION
  for(ia=0; ia<num_observe; ia++){
    for(ib=SKIP_CAPTURE; ib<cap_freq; ib++){
	signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
	OBSignature[ib]|= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
			}
		}
#else
  for(ia=0; ia<ffnum; ia++){
    for(ib=SKIP_CAPTURE; ib<cap_freq; ib++){
	signature |= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
	OBSignature[ib]|= ff_observe[ia][ib].gdval1 ^ ff_observe[ia][ib].ftval1;
			}
		}
#endif
#endif

//printf(" signature=%x \n",signature);
  mask=ITI;

  for(ia=0; ia<num_injgate; ia++, mask=mask<<1){
      fgnode = injarray[ia];
	 if(signature&mask){
#if DEBUG1 || PRNT_DET_FLT
      printf(" DETECT FAULT %d",fgnode->back->line);
      if(fgnode->forwd!=NULL)
	printf(" >> %d",fgnode->forwd->line);
      else
	printf(" >> 0");
      printf(" sa %d \n",fgnode->saval);
#endif

/*Tran faultlist ana*/
//Last capture Detection
flag=0;
	if(((fgnode->back->ftval0[cap_freq-1]&mask)>>(ia))==fgnode->saval){
				if(signature1&mask){
			TranFltCount++;	TranFltOBCount++;flag=1;
			fgnode->TranDetTimes++;
		#if FAULTOB
			for(ib=0; (ib<ffnum)&&(ib<num_observe); ib++){
			if(FF_Signature[ib]&mask){
				flag=0;
				for(ic=1;ic<=FF_Fault_List[ib][0];ic++){
					if(FF_Fault_List[ib][ic]==fgnode->num){flag=1;break; }
					}
				if(flag==0){
					FF_Fault_List[ib][0]++;
					FF_Fault_List[ib][FF_Fault_List[ib][0]]=fgnode->num;
						}
					}
				}
		#endif

   		}
	}
//OBFF Detection Tran
#if FF_OBSERVE
	for(iOB=SKIP_CAPTURE;iOB<cap_freq;iOB++){
		if(((fgnode->back->ftval0[iOB-1]&mask)>>(ia))==fgnode->saval){
		if(OBSignature[iOB]&mask){
			if(flag==0)TranFltOBCount++;
			fgnode->TranOBDetTimes++;
		#if FAULTOB
			for(ib=0; (ib<ffnum)&&(ib<num_observe); ib++){
			if(OBFF_Signature[ib][iOB]&mask){
				flag=0;
				for(ic=1;ic<=Ob_FF_Faults[ib][0];ic++){
					if(Ob_FF_Faults[ib][ic]==fgnode->num){flag=1;break; }
					}
				if(flag==0){
					Ob_FF_Faults[ib][0]++;
					Ob_FF_Faults[ib][Ob_FF_Faults[ib][0]]=fgnode->num;
						}
					}
				}
		#endif
			break;
			}
		}
	}
#endif

if((fgnode->TranOBDetTimes)||(fgnode->TranDetTimes)){

#if OUTPUT_FLIST
      fprintf(flist_out, "1");
      fgnode->detect[time/32]|=ITI<<(time%32);
      fgnode->dtime++;
#else

      if(fgnode->next!=NULL)
       fgnode->next->prev=fgnode->prev;
     fgnode->prev->next=fgnode->next;
     injarray[ia]=NULL;
#endif

		}
	}
}
}
