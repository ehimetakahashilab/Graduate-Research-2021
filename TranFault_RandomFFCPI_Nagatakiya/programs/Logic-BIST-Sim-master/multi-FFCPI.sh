#!/bin/sh
User_DIR=$(cd "$(dirname "$0")";pwd)

OP_LIST_DIR=${User_DIR}/FF_STATION
CP_LIST_DIR=${User_DIR}/CP_STATION

if [ ! -e ${OP_LIST_DIR} ]; then
  mkdir -p ${OP_LIST_DIR}
fi

if [ ! -e ${CP_LIST_DIR} ]; then
  mkdir -p ${CP_LIST_DIR}/LCP
      mkdir -p ${CP_LIST_DIR}/FFCP
fi
TPG=0 #=0:LFSR,=1:ATPG
TEST_VEC=100 #0 00 # Number of Test patterns
TOOLMODE=4 #=1:Normal Scan test, =2:Multi-cycle Test, =3: Multi-cycle test with Seq OB, =4:Toggle Gate TPI
CP_CTRL=2
	CAPTURE=10 #the number of capture cycles
  OBRATE=0.0 #ratio of OP FF
	FFCP_rate=0.1	#the ration of FF-CPs in all FFs
  SKIP_CAP=3 #SKIP_CAP=3, the CP control starts from the third capture cycle.
  INTERVAL_CYCLE=1 #=1: the number of interval cycles

ulimit -s unlimited
rm -f *.dat
for CIRCUIT in s9234 #s15850
#s13207 #s9234 #s13207 #s15850 s38417 s38584 #b14s.osaka b15s.osaka b20s.osaka

do
rm -f lfsr*.dat ATPG.dat
rm -f ${CIRCUIT}_tgl_FF_input.dat
rm -f ${CIRCUIT}_tgl_ff_tpi.dat

  ln -s ./circuits/$CIRCUIT
  ln -s ./tpg/lfsr.dat

	if [ "$TPG" = "0" ];
	then
		rm -f ${CIRCUIT}_lfsr_pi.dat
		./tpg/lfsr $CIRCUIT $TEST_VEC ${CIRCUIT}_lfsr_pi.dat
		ln -s tmp.test ${CIRCUIT}_lfsr_pi.dat
	else
 		 ln -s ./ATPG/SA/Com1Test/"$CIRCUIT".tests ATPG.dat
	fi

  if [ ! -e ${OP_LIST_DIR}/${CIRCUIT} ]; then
    mkdir -p ${OP_LIST_DIR}/${CIRCUIT}
  fi

  if [ ! -e ${User_DIR}/OUTPUTS/CPI/${CAPTURE}_cycles/ ]; then
    mkdir -p ${User_DIR}/OUTPUTS/CPI/${CAPTURE}_cycles/
  fi
#	  for ff_sta_file in  ./FF_STATION/$CIRCUIT/TOPSIS #./FF_STATION/$CIRCUIT/BRANCH ./FF_STATION/$CIRCUIT/COMPLEX ./FF_STATION/$CIRCUIT/TYPE_1 ./FF_STATION/$CIRCUIT/TYPE_2 ./FF_STATION/$CIRCUIT/TYPE_3 ./FF_STATION/$CIRCUIT/TOPSIS
# you can evaluate multiple FF OP list in one simulation by specifying the path of OB-FF List
	 cnt1=0
 for ff_sta_file in ${OP_LIST_DIR}/${CIRCUIT}/TOPSIS
  #  for ff_sta_file in  ~/renesas/Renesas-TPI/Seq_FF_OPI/DATA/cp_$CIRCUIT/FF_STATION/TOPSIS
	   do

		if [ -e $ff_sta_file ];
		then
 		 echo === OB_STATION INFO of $ff_sta_file is FOUND, CONTINUE THE FAULT SIM PROCESS===
		else
 		 echo === NO OB_STATION INFO, PLEASE BACK TO FF_SELECTION PROCESS to Generate $ff_sta_file===
			exit
		fi

		ln -s ${ff_sta_file} ff_station_${cnt1}.dat
		let cnt1=${cnt1}+1
	 done


   echo ==Simulation for FF-CPI=== #>> $LOG_FILE

ln -s ${CP_LIST_DIR}/FFCP/${CIRCUIT}/rffcp1  ${CIRCUIT}_tgl_FF_input.dat #rffcp (FFCPI) file select

echo ===$CIRCUIT: Interval capture = $INTERVAL_CYCLE===========

time ./lbistsim $CIRCUIT $TOOLMODE $TPG $CP_CTRL $FFCP_rate $CAPTURE $INTERVAL_CYCLE $SKIP_CAP $cnt1 $OBRATE $TEST_VEC

rm -rf tgl_*.dat ff_station_*
rm -f ${CIRCUIT}_tgl_FF_input.dat

    if [ -e fault_list.dat ];
      then
      mv fault_list.dat "$CIRCUIT"_fault_list.dat
      if [ ! -e ./fault_list ];
        then
        mkdir fault_list
      fi
      mv "$CIRCUIT"_fault_list.dat ./fault_list
    fi
rm -f $CIRCUIT lfsr.dat *~ tmp*  ff_station.dat ATPG.dat *lfsr_pi.dat
done
