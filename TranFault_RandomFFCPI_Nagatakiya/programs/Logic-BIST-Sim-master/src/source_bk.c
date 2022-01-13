

int numout,slist,numgate;
//int pitbl[MAXPT],potbl[MAXPT],fftbl[MAXPT],ffintbl[MAXPT],flist[MAXLINE];
int lpnt,inpnum,ffnum,sum_flt,remain_flt,sum_Tran_flt,chainnum,interstatecount[2],tgl_gt_cnt,
flag_ff_state[2000],FF_Fault_Det[2000],changed_freq[2000],FF_Fault_List[2000][60000],
Ob_FF_Faults[2000][60000],FF_SA_Fault_List[2000][60000],Ob_FF_SA_Faults[2000][60000];
int lfsr_count,FaultyFreeflag[2000],Faultyflag[2000],FF_flag[2000][4],SO_Reduction_Capa[2000];

double CTR,toggle_shift_perpat,toggle_cap_perpat[MAXCAP],WSA[MAXCAP],MaxWSA,UnChangeFF,Sum_UnChangeFF,
sub_unchangedFF,sub_zero_unchangeFF,UnchangedFF_Logic;

float ShiftPeak[CHAINNUM];
float CapMaxPeek[MAXCAP],CapMaxPeekPerPat[MAXCAP],CapMaxPeekEvePat[MAXCAP],
ff_toggle_density[2000],SaFltCount,SaFltOBCount,TranFltCount,TranFltOBCount,OBSERVE_RATE;
unsigned int TapGen_state[5+1],initial_ff_state[5][2000],Ex_Seed;
FILE *flist_out;

//2進数でパターンごとにキャプチャ毎に故障を記録するため変数_2015710_王

int *scan_pat_flt,*scan_by_total_pat_flt,*scan_by_multi_pat_flt,*scan_by_last_pat_flt,**scan_by_OB_pat_flt;
int TESTMODE;
unsigned int **FF_Faults_lists,**FF_CapFaults_lists;
int **Pat_Faults;
//2進数でパターンごとにキャプチャ毎に故障を記録するため変数_2015710_王
char fltpp_path[50];
FILE *fout_fpp;

FILE *fp_pat_flt;
char pat_flt_out[50];
float GATE_GP_END;










#if FAULTOB & FAULTSIMULATION //Observe fault coverage of each FF
/*fault list of each FF*/
#if TRANSITIONFAULT

  fout = fopen("TraFltDet_By_FF_LastCap", "w");
  fprintf(fout,"Transition Fault num= %d\n",sum_Tran_flt);

for(ia=0;ia<ffnum;ia++){
	fprintf(fout,"%d %d ",ia,FF_Fault_List[ia][0]);
	for(ib=1;ib<=FF_Fault_List[ia][0];ib++)
	fprintf(fout,"%d ",FF_Fault_List[ia][ib]);
fprintf(fout,"\n");
}
fclose(fout);

  fout = fopen("TraFaultsList_DetBy_OBFF", "w");
  fprintf(fout,"Transition Fault num= %d\n",sum_Tran_flt);

for(ia=0;ia<ffnum;ia++){
	fprintf(fout,"%d %d ",ia,Ob_FF_Faults[ia][0]);
	for(ic=1;ic<=Ob_FF_Faults[ia][0];ic++)
	fprintf(fout,"%d ",Ob_FF_Faults[ia][ic]);
	//for(ib=1;ib<=Ob_FF_Faults[ia][0];ib++)
	//fprintf(fout,"%d ",Ob_FF_Faults[ia][ib]);
	fprintf(fout,"\n");
}
fclose(fout);

#else
//scan_by_multi_pat_flt[time]=scan_by_multi_detflt;
//scan_by_OB_pat_flt[time]=scan_by_OB_detflt;

/*2進数でパターンごと、FFごと、キャプチャごとの故障情報を記録する＿＿2015710_王*/
/*開始*/
#if BITRECORD
//四つの故障リストを出力する、故障リストは2進数の形で32個ごとに格納している
//例：故障番号＝10であれば、2進数0000　0000　0100　0000　0000　0000　0000　0000で表し、10進数（4194304）で格納する　
char *OPName1= "Total_Results";
char *OPName2="Last_Cap_Results";
char *OPName3="OB_Cap_Results";
char *OPName4="Eve_Cap_Results";
char out1[50]={'\0'};
char out2[50]={'\0'};
char out3[50]={'\0'};
char out4[50]={'\0'};
char tmp[10]={'\0'};
char *atpg="ATPG";
char *lfsr="LFSR";
char mode[4]={'\0'};
printf("%s\n",mode);
strcpy(tmp,argv[1]);

if(TESTMODE==1) strcpy(mode,atpg);
else strcpy(mode,lfsr);

sprintf(out1,"./SIM_RESULT/FLT_LIST/%s_%s_%s",tmp,OPName1,mode);
sprintf(out2,"./SIM_RESULT/FLT_LIST/%s_%s_%s",tmp,OPName2,mode);
sprintf(out3,"./SIM_RESULT/FLT_LIST/%s_%s_%s",tmp,OPName3,mode);
sprintf(out4,"./SIM_RESULT/FLT_LIST/%s_%s_%s",tmp,OPName4,mode);

fout=fopen(out1, "w");
fout1=fopen(out2, "w");
fout2=fopen(out3, "w");
fout3=fopen(out4, "w");

//fprintf(fout,"Fault List of each FF with Full Test\nPatNO.  FFNO.  Number of Faults   FltLists\n");
//fprintf(fout1,"Fault List of each FF with Full Test by the Last Capture Cycle\nPatNO.  FFNO.  Number of Faults   FltLists\n");
//fprintf(fout2,"Fault List of each FF with Full Test by Multicycle Capture\nPatNO.  FFNO.  Number of Faults   FltLists\n");
//fprintf(fout3,"Fault List of each FF with Full Test by Each Capture During MultiCycle test\nPatNO.  FFNO. CaptureNO.  Number of Faults   FltLists\n");
remain_flt=count_flt(fltlst.next);
printf("\nFaultCoverage = %4.3f \n",(1-(float)remain_flt/(float)sum_flt)*100.0);
fprintf(fout,"%d %d %d \n",length, ffnum, sum_flt);
fprintf(fout1,"%d %d %d \n",length, ffnum, sum_flt);
fprintf(fout2,"%d %d %d \n",length, ffnum, sum_flt);
fprintf(fout3,"%d %d %d %d\n",length, ffnum, cap_freq,sum_flt);
for(ia=0;ia<=length;ia++){
if(ia % PRN_FLT_INT == 0)
 //printf("I Am alive,%d/%d was finished, please wait!!\n",ia,length);
fprintf(fout,"patten %d  ",ia);
if(ia!=0){
fprintf(fout1,"patten %d  ",ia);
fprintf(fout2,"patten %d  ",ia);
}
fprintf(fout3,"patten %d  ",ia);

	for(ib=0;ib<ffnum;ib++){
		fprintf(fout,"\nFF %1d %1d",ib,Pat_FF_Faults[ia][ib].Total_Faults[0]);
	if(ia!=0){
		fprintf(fout1,"\nFF %1d %1d",ib,Pat_FF_Faults[ia][ib].Last_Cap_Faults[0]);
		fprintf(fout2,"\nFF %1d %1d",ib,Pat_FF_Faults[ia][ib].OB_Cap_Faults[0]);
		}
		fprintf(fout3,"\nFF %1d",ib);
	/*for(ic=1;ic<cap_freq;ic++){
		fprintf(fout3,"\nCap %1d %1d",ic,Pat_FF_Faults[ia][ib].Cap_Faults[ic][0]);
		for(id=1;id<=sum_flt/32+1;id++)
		fprintf(fout3," %1x ",Pat_FF_Faults[ia][ib].Cap_Faults[ic][id]);
		}	*/
	for(id=1;id<=sum_flt/32+1;id++){
		fprintf(fout," %1x ",Pat_FF_Faults[ia][ib].Total_Faults[id]);
		if(ia!=0){
		fprintf(fout1," %1x ",Pat_FF_Faults[ia][ib].Last_Cap_Faults[id]);
		fprintf(fout2," %1x ",Pat_FF_Faults[ia][ib].OB_Cap_Faults[id]);
		}
		}

	}
		fprintf(fout3,"\n");
fprintf(fout,"\n");
if(ia!=0){
fprintf(fout1,"\n");
fprintf(fout2,"\n");
}
	}

fclose(fout);
fclose(fout1);
fclose(fout2);
fclose(fout3);

  for(ia=0; ia<length+1; ia++)
free(Pat_FF_Faults[ia]);
  free(Pat_FF_Faults);
/*終了*/
#elif PAT_FCOV

char OUTPUT[20]={'\0'};
char *Name= "Pat_Fcov";

sprintf(OUTPUT,"%s_%s",Name,argv[1]);
fout=fopen(OUTPUT, "w");

for(ia=0;ia<length/100;ia++){

fprintf(fout,"%d %d ",ia,Pat_Faults[ia][0]);
for(ib=1;ib<=Pat_Faults[ia][0];ib++)
	fprintf(fout,"%d ",Pat_Faults[ia][ib]);
fprintf(fout,"\n");
}
fclose(fout);
#elif NORMAL_REC
  strcpy(path_last,PATH_LAST);
  strcat(path_last,argv[7]);
  strcat(path_last,"/");
  strcat(path_last,"LastCap_");
  strcat(path_last,argv[1]);//回路名
  strcat(path_last,"_");
  strcat(path_last,argv[7]);//サイクル数
  strcat(path_last,"_");
  strcat(path_last,length);//ベクトル数
  //printf("%s\n",path_last);
  strcpy(path_middle,PATH_MIDDLE);
  strcat(path_middle,argv[7]);
  strcat(path_middle,"/");
  strcat(path_middle,"MiddleCap_");
  strcat(path_middle,argv[1]);
  strcat(path_middle,"_");
  strcat(path_middle,argv[7]);
  strcat(path_middle,"_");
  strcat(path_middle,length);
  //printf("%s\n",path_middle);
  strcpy(name,"SIM_RESULT/");
  strcat(name,"coverage_");
  strcat(name,argv[7]);
  strcat(name,"_");
  strcat(name,length);
  strcat(name,".txt");
  //
  strcpy(path_toggle,PATH_TOGGLE);
  strcat(path_toggle,argv[7]);
  strcat(path_toggle,"/");
  strcat(path_toggle,"togglerate_");
  strcat(path_toggle,argv[1]);
  strcat(path_toggle,"_");
  strcat(path_toggle,length);
  strcat(path_toggle,".SCOAP_txt");
  //printf("%s\n",path_toggle);
/*for(ia=0;ia<ffnum;ia++){
	printf("fault_%d #%d : ",ia,FF_SA_Fault_List[ia][0]);
	for(ib=1;ib<=FF_SA_Fault_List[ia][0];ib++)
	printf("%d ",FF_SA_Fault_List[ia][ib]);
printf("\n");
}*/
///*2014_10_21
 fout=fopen(path_last,"w");//
//  fout=fopen("SaFltDet_By_FF_LastCaps", "w");
 printf("%s\n",path_last);
  fprintf(fout,"%d %d\n",ffnum,sum_flt);
for(ia=0;ia<ffnum;ia++){
	fprintf(fout,"%d %d ",ia+1,FF_SA_Fault_List[ia][0]);
	for(ib=1;ib<=FF_SA_Fault_List[ia][0];ib++)
	fprintf(fout,"%d ",FF_SA_Fault_List[ia][ib]);
fprintf(fout,"\n");
}
fclose(fout);
 fout=fopen(path_middle,"w");//
 printf("%s\n",path_middle);
//  fout = fopen("SaFaultsList_DetBy_OBFF", "w");
  fprintf(fout,"%d %d\n",ffnum,sum_flt);
for(ia=0;ia<ffnum;ia++){
	fprintf(fout,"%d %d ",ia+1,Ob_FF_SA_Faults[ia][0]);
	for(ic=1;ic<=Ob_FF_SA_Faults[ia][0];ic++)
	fprintf(fout,"%d ",Ob_FF_SA_Faults[ia][ic]);
	//for(ib=1;ib<=Ob_FF_Faults[ia][0];ib++)
	//fprintf(fout,"%d ",Ob_FF_Faults[ia][ib]);
	fprintf(fout,"\n");
}
fclose(fout);
#elif SEL_OB_FF_FLT
fclose(fp_pat_flt);

sprintf(pat_flt_out,"./SIM_RESULT/ScanBy_%s_cap%d",argv[1],cap_freq);

  if(NULL==(fp_pat_flt=fopen(pat_flt_out,"w")))
    printf(" error 111\n"),exit(0);
fprintf(fp_pat_flt,"#pat |　検出故障	\n");
fprintf(fp_pat_flt,"#pat SC Multi 中間+最終 | TOGGLE BRANCH TYPE1 TYPE2 TYPE3 COMPLEX SCOAP COP_OB COP_CTR COP_OB_DIF OP AVE MIN TOPSIS\n");

/*RedundantFlts関係は不要なため削除 160509
FILE *fp_Rdunt;
int rd_flt_num, *rd_flt;
fp_Rdunt = fopen("redundflt.dat", "r");
  if(fp_Rdunt == NULL)
    printf("'redundflt.dat' is not found!\n"), exit(1);

    fscanf(fp_Rdunt, "%d", &rd_flt_num);
    fscanf(fp_Rdunt, "%d", &rd_flt_num);

rd_flt = (int *)calloc(rd_flt_num,sizeof(int));

if(rd_flt == NULL)
    printf("memory error @rd_flt_Faults_lists \n"), exit(1);
for(ia=0; ia<rd_flt_num; ia++)
    fscanf(fp_Rdunt, "%d %d", &rd_flt[ia],&ib);

fclose(fp_Rdunt);
*/

FLT_NODE *flttag;
flttag=fltlst.next;
int flt_num_tmp1=0,flt_num_tmp2=0,flt_num_tmp3=0,flt_num_tmp4=0,count=0;
int flt_num_ob_tmp[FF_FILE];
for(ia=0;ia<FF_FILE;ia++) flt_num_ob_tmp[ia]=0;
 for( ;flttag!=NULL;flttag=flttag->next){
	if((flttag->scan_dtime)){
		scan_pat_flt[flttag->first_det_pat_scan]++;
		count++;
	/*for(ia=0; ia<rd_flt_num; ia++)
   	if((flttag->line+ffnum)==rd_flt[ia]) printf("reduent fault %d has been detected by scan\n",rd_flt[ia]);//exit(1);*/
	}
   	if((flttag->scan_dtime)&&(flttag->last_cap_dtime))
		scan_by_last_pat_flt[flttag->first_det_pat_last]++;
   	if((flttag->scan_dtime)&&(flttag->dtime))
		scan_by_total_pat_flt[flttag->first_det_pat_total]++;
   	if((flttag->scan_dtime)&&(flttag->OBdtime))
		scan_by_multi_pat_flt[flttag->first_det_pat_multi]++;

for(ia=0;ia<FF_FILE;ia++){
   	if((flttag->scan_dtime)&&(flttag->OBdtime_sel_FF[ia]))
		scan_by_OB_pat_flt[flttag->first_det_pat_obsel[ia]][ia]++;
}
	}
//printf("Outputing Final Results, please wait!!\n");

for(ia=1;ia<=length;ia++){
flt_num_tmp1+=scan_by_last_pat_flt[ia];
scan_by_last_pat_flt[ia]=flt_num_tmp1;

flt_num_tmp2+=scan_pat_flt[ia];
scan_pat_flt[ia]=flt_num_tmp2;

flt_num_tmp3+=scan_by_multi_pat_flt[ia];
scan_by_multi_pat_flt[ia]=flt_num_tmp3;

flt_num_tmp4+=scan_by_total_pat_flt[ia];
scan_by_total_pat_flt[ia]=flt_num_tmp4;

for(ib=0;ib<FF_FILE;ib++){
flt_num_ob_tmp[ib]+=scan_by_OB_pat_flt[ia][ib];
scan_by_OB_pat_flt[ia][ib]=flt_num_ob_tmp[ib];
}

//printf("%5d %4.3f %4.3f %4.3f %4.3f \n",ia,(float)scan_pat_flt[ia]/(float)count*100.0,(float)scan_by_last_pat_flt[ia]/(float)count*100.0,(float)scan_by_multi_pat_flt[ia]/(float)count*100.0,(float)scan_by_total_pat_flt[ia]/(float)count*100.0);
/*if(ia<SAMPLE){
fprintf(fp_pat_flt,"%5d  %4.4f %4.4f %4.4f %4.4f | ",ia,(float)scan_pat_flt[ia]/(float)count*100.0,(float)scan_by_last_pat_flt[ia]/(float)count*100.0,(float)scan_by_multi_pat_flt[ia]/(float)count*100.0,(float)scan_by_total_pat_flt[ia]/(float)count*100.0);

for(ib=0;ib<FF_FILE;ib++)
fprintf(fp_pat_flt,"%4.4f ",ia,(float)scan_by_OB_pat_flt[ia][ib]/(float)count*100.0);
fprintf(fp_pat_flt,"\n");
}*/
//else if(ia%PRN_FLT_INT == 0){
if(ia%PRN_FLT_INT== 0){
//printf("%s Outputing,%d/%d was finished, please wait!!\n",argv[1],ia,length);
fprintf(fp_pat_flt,"%5d, %4.4f, %4.4f, %4.4f, | ",ia,(float)scan_pat_flt[ia]/(float)count*100.0,(float)scan_by_last_pat_flt[ia]/(float)count*100.0,(float)scan_by_total_pat_flt[ia]/(float)count*100.0);

for(ib=0;ib<FF_FILE;ib++)
fprintf(fp_pat_flt,",%4.4f ",ia,(float)scan_by_OB_pat_flt[ia][ib]/(float)count*100.0);
fprintf(fp_pat_flt,"\n");
}
}
fclose(fp_pat_flt);

#endif
//*/
#endif
#endif

//fault_det_cap(FF_Fault_List,saf_list_check(fltlst.next),argv);


#if TRANSITIONFAULT
ib=0;
printf("%d ",ib);
for(ia=0;ia<ffnum;ia++){
		if(ia==schain[ib+1].top){
			ib++;
			printf("\n%d ",ib);
					}
	printf("%4.2f ",(float)FF_Fault_Det[ia]/TranFltCount);
		}
printf("\n");
#endif

#if POWEREVA
	remain_flt=count_flt(fltlst.next);
char outpath[100];
#if EXTERNSEED
sprintf(outpath, "./SEED_RESULTS/%s_%d.txt", argv[1],cap_freq);
if((fout=fopen(outpath,"a"))==NULL)
printf("results output file is not exist!\n"), exit(1);
printf("EX_Seed=%d\n",Ex_Seed);
fprintf(fout,"%d,%4.3f,%.0lf,%.0lf,%.0lf,%.0lf",Ex_Seed,(1-(float)remain_flt/(float)sum_flt)*100.0,(float)toggle_scn,(float)toggle_scn_in,(float)toggle_scn_out,(float)toggle_scn_max);
#if WSA_REC

for(ia=1;ia<=cap_freq;ia++)
fprintf(fout,",%.0f",WSA[ia]/clocktime);// exit(1);
fprintf(fout,",%.0f\n",MaxWSA);

#else
fprintf(fout,"\n");
#endif
fclose(fout);
#endif

#if TOGGLE_GATE
  L_NODE *fnode;
  FIN_NODE *finnode;
	float variation=0.0,mean=0.0,ave_var=0.0;
	int cnter=0;
#if SWITCH_GATE

#if STUCT_TGL_GT
switch((int)GATE_GP_END){
	case 0: sprintf(outpath, "./GATE_TOGGLE/STRUCT/%s_tgl_pro_GT=%d.csv", argv[1],TGL_GT_NUM);break;
	case 1: sprintf(outpath, "./GATE_TOGGLE/STRUCT/%s_tgl_pro_overlap_GT=%d.csv", argv[1],TGL_GT_NUM);break;
	case 2: sprintf(outpath, "./GATE_TOGGLE/STRUCT/%s_tgl_struct_GT=%d.csv", argv[1],TGL_GT_NUM);break;
	case 3: sprintf(outpath, "./GATE_TOGGLE/STRUCT/%s_tgl_struct_overlap_GT=%d.csv", argv[1],TGL_GT_NUM);break;
	default: exit(1);break;
}
#else
sprintf(outpath, "./GATE_TOGGLE/%s_%d_switched_%.1f_%.1f.csv", argv[1],cap_freq,(GATE_GP_END-0.1),GATE_GP_END);
#endif
if((fout=fopen(outpath,"w"))==NULL)
printf("Gate_TOGGLE results output file is not exist!\n"), exit(1);

#else
sprintf(outpath, "./GATE_TOGGLE/STRUCT/test_%s_%d.csv", argv[1],cap_freq);
if((fout=fopen(outpath,"w"))==NULL)
printf("Gate_TOGGLE results output file is not exist!\n"), exit(1);
#endif
	remain_flt=count_flt(fltlst.next);

	fprintf(fout,"Fcov=%4.3f,TGL_GT=%d \n\n",(1-(float)remain_flt/(float)sum_flt)*100.0,tgl_gt_cnt);
	printf("Fcov=%4.3f,TGL_GT=%d \n\n",(1-(float)remain_flt/(float)sum_flt)*100.0,tgl_gt_cnt);
  fnode=gnode.next;
  for( ;fnode!=NULL;fnode=fnode->next){
	variation=0.0,mean=0.0;cnter++;
  //printf("%d ",fnode->line);
	for(ia=1;ia<=cap_freq;ia++){
	mean+=fnode->toggle_rate[ia];

	}
	mean/=cap_freq;
	for(ia=1;ia<=cap_freq;ia++){
	variation+=(fnode->toggle_rate[ia]-mean)*(fnode->toggle_rate[ia]-mean);
	 // printf("%.0f ",fnode->toggle_rate[ia]);
	}
 //printf("%.4f ",variation/(cap_freq-1));
	variation=sqrtf(variation/(cap_freq-1));
	ave_var+=variation;
	fnode->toggle_rate[cap_freq+2]=variation;
 //printf("%.0f %.4f\n",fnode->toggle_rate[cap_freq+1],fnode->toggle_rate[cap_freq+2]);
	}
ave_var/=cnter;
printf("average variation=%.4f \n",ave_var);
  fnode=gnode.next;
  for( ;fnode!=NULL;fnode=fnode->next){
  fprintf(fout,"%d,",fnode->line);
	if(fnode->toggle_flag==1){
	  for(ia=1;ia<=cap_freq;ia++){
	   fprintf(fout,"%.0f,",0.0);
			}
		}
	else{
	for(ia=1;ia<=cap_freq;ia++){
	  fprintf(fout,"%.0f,",fnode->toggle_rate[ia]);
	}
	}
#if SWITCH_GATE

#else
if(fnode->toggle_rate[cap_freq+2]>=ave_var) fnode->toggle_flag=1;
else fnode->toggle_flag=0;
 #endif
  fprintf(fout,"%.0f,%.4f,%d\n",fnode->toggle_rate[cap_freq+1],fnode->toggle_rate[cap_freq+2],fnode->toggle_flag);
	}



/*fprintf(fout,"=======================\n");
finnode=ffnode.next;
  for(; finnode!=NULL; finnode=finnode->next){
    fnode=finnode->node;
  fprintf(fout,"%d, %d, %d",fnode->line-numout-inpnum,fnode->type,fnode->innum);
	for(ia=1;ia<=cap_freq;ia++)
	  fprintf(fout,", %.0f, %.4f",(float)fnode->toggle_rate[ia],(float)fnode->toggle_rate[ia]/clocktime*100.0);
  fprintf(fout,", %.0f, %.4f\n",fnode->toggle_rate[cap_freq+1],(float)fnode->toggle_rate[cap_freq+1]/(float)(clocktime*cap_freq)*100.0);
	}
*/

fclose(fout);

#endif
	printf("Fcov    SI     SO     Shift   \n");
	remain_flt=count_flt(fltlst.next);
	printf("%d %4.3f %.0lf %.0lf %.0lf %.0lf \n",remain_flt,(1-(float)remain_flt/(float)sum_flt)*100.0,(float)toggle_scn,(float)toggle_scn_in,(float)toggle_scn_out,(float)toggle_scn_max);
	printf("%4.3f %4.3f %4.3f %ld %ld \n",(float)toggle_scn_in/(float)toggle_scn_max*200.0,(float)toggle_scn_out/(float)toggle_scn_max*200.0,(float)toggle_scn/(float)toggle_scn_max*100.0,toggle_scn_in,toggle_scn_out);

printf("Capture Power\n");
 for(ia=1;ia<=cap_freq;ia++)
	  printf("%4.3f ", (double)toggle_cap[ia]/(double)toggle_cap_max*100.0);
 printf("\n");

#if NORMAL_REC
 fout=fopen(path_toggle,"w");
 printf("%s\n",path_toggle);
fprintf(fout,"%d \n",ffnum);
 for(ia=0;ia<ffnum;ia++)//hamada 2014_09_22
  //printf("toggle rate[%d] = %4.3f = %d / 19000\n",ia+1,(double)toggle_cnt[ia]/(double)19000*100,toggle_cnt[ia]);//hamada 2014_09_22
 fprintf(fout," %d : %d / 200 = %4.3f\n",ia+1,toggle_cnt[ia],(double)toggle_cnt[ia]/(double)200*100);
 fclose(fout);
#endif
/*#if WSA_REC
  fout = fopen("WSA_all_Bcircuit.txt", "a");
  fprintf(fout, "%s  ",argv[1]);  fprintf(fout, " %d Cycle\n",cap_freq);
fprintf(fout,"  Max_WSA= %.0f \n",MaxWSA);
for(ia=1;ia<=cap_freq;ia++)
fprintf(fout," %.0f ",WSA[ia]/1000);// exit(1);
fprintf(fout,"  \n");
for(ia=1;ia<=cap_freq;ia++)
fprintf(fout,"%2.3f ",(float)WSA[ia]/MaxWSA*100);// exit(1);
fprintf(fout,"\n");
  fclose(fout);
#endif*/

#if PEAK
float Max=0;
for(ia=0;ia<chainnum;ia++){
if(ShiftPeak[ia]/(schain[ia].length-1)>Max)
Max=ShiftPeak[ia]/(schain[ia].length-1);
}
  fout = fopen("shift_and_peak.txt", "a");
fprintf(fout,"%4.3f %4.3f \n",(float)toggle_scn/(float)toggle_scn_max*100.0,Max*100);
  fclose(fout);
#endif

#else
	remain_flt=count_flt(fltlst.next);
	printf("\nFaultCoverage = %4.3f \n",(1-(float)remain_flt/(float)sum_flt)*100.0);
	/*
	fout = fopen(name,"a");
	fprintf(fout,"%s : %4.3f \n" , argv[1] ,(1-(float)remain_flt/(float)sum_flt)*100.0);
	fclose(fout);
	*/

#endif


#if DEBUG1
  prn_fltlst(fltlst.next);
#endif
  remain_flt=count_flt(fltlst.next);

#if MAKE_REC
  fout = fopen("fault coverage.log", "a");
  fprintf(fout, "%6.3f\n", 100.0-(float)remain_flt/(float)sum_flt*100.0);
  printf("shift_Toggle_Rate = %f\ntoggle_capRate = %f\n", (float)(shift_toggle_rate)*100/(chainnum*clocktime), (float)toggle_cap*100/(cap_freq*clocktime*ffnum));

  fclose(fout);
#endif


}
