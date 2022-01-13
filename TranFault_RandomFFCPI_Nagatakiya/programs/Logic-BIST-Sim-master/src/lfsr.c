#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"
#include "math.h"

//#define DEBUG 1 //20140922
//#define ALPMODE 1 //0:AI-RTPG 1:Basic ALP-RTPG 2:Combination of AI&ALP
#define TAP_NUM 5
unsigned int TapGen_state[TAP_NUM];
void InternalLfsrGen(int *Xor);
void phase_shifter(int LFSR[], int nBit);

void phase_shifter(int LFSR[], int nBit)
{
  int ia;
  for (ia = 1; ia < nBit; ia++)
    LFSR[ia] ^= LFSR[ia - 1];
}

void InternalLfsrGen(int *Xor)
{
  int i, count = 0, tmp[LFSR_BIT + 1];
  unsigned int feedbackbit = 0, mask = 0, temple = 0;
  while (count < TAP_NUM / 2 + 1)
  {
    feedbackbit = TapGen_state[TAP_NUM / 2 + count] & 1;
    temple = (feedbackbit << (LFSR_BIT - 1)) | (TapGen_state[TAP_NUM / 2 + count] >> 1);
    mask = 0;
    for (i = 2; i <= Xor[0]; i++)
      mask |= (feedbackbit << (LFSR_BIT - Xor[i] - 1));
    count++;
    TapGen_state[TAP_NUM / 2 + count] = temple ^ mask;
#if PHASE
    for (i = 0; i < LFSR_BIT; i++)
      tmp[i] = TapGen_state[TAP_NUM / 2 + count] >> (LFSR_BIT - i - 1);
    phase_shifter(tmp, LFSR_BIT);

#endif
  }
}

void reseeding(lfsr_state, cnt) int lfsr_state[];
int cnt;
{
  int ia;
  unsigned int seed = 0;
  FILE *fin;

#if RANDOMSEED
  seed = (unsigned int)(rand() % 65535);
  printf("seedcount=%d seed=%d--", cnt, seed);
  for (ia = 16 - 1; ia >= 0; ia--)
    printf("%d", (seed >> ia) & 0x0001);
  printf("\n");
#elif SEEDMEMORY
  if ((fin = fopen("seed.dat", "r")) == NULL)
  {
    printf("error: 'seed.dat' is not found\n");
    exit(1);
  }
  for (ia = 0; ia < cnt; ia++)
    fscanf(fin, "%d", &seed);
  fclose(fin);
#endif
  TapGen_state[TAP_NUM / 2] = seed;
  printf("TapGen_state=%d\n", TapGen_state[TAP_NUM / 2]);
}

void initial_lfsr(EX_OR, lfsr_state, ff_state) int EX_OR[];
int lfsr_state[];
int ff_state[];
{
  int ia, max;
  char buf[20];

  FILE *fin;
  srand((unsigned)time(NULL));
  for (ia = 0; ia <= LFSR_BIT; ia++)
  { //initialize SI's LFSR
    if (ia % 4)
      lfsr_state[ia] = 0;
    else
      lfsr_state[ia] = 1;
  }

  for (ia = 0; ia <= ffnum; ia++) //initialize FF state
    ff_state[ia] = 0;
  CHAINLENGTH = 100;

  if (ffnum > LFSR_BIT * CHAINLENGTH)
  {
    printf("warning:ffnum is must smaller than LFSR_BIT*CHAINLENGTH\n");
    CHAINLENGTH = (1 + ffnum / (LFSR_BIT * CHAINLENGTH)) * 100;
  }
  chainnum = (ffnum - 1) / CHAINLENGTH + 1;
#if POWEREVA
  for (ia = 0; ia < chainnum; ia++)
    ShiftPeak[ia] = 0.0;
#endif
  schain = (SCAN_CHAIN *)calloc(chainnum, sizeof(SCAN_CHAIN));
  if (schain == NULL)
    printf("memory error @initial_lfsr\n"), exit(1);

  fin = fopen("lfsr.dat", "r");
  if (fin == NULL)
  {
    printf("error: 'lfsr.dat' is not found\n");
    exit(1);
  }
  fscanf(fin, "%d", &max);

  if ((LFSR_BIT > max) || (chainnum > max))
  {
    printf("error: too many LFSR bits!\n");
    fclose(fin);
    exit(1);
  }

  for (ia = 0; ia < LFSR_BIT; ia++)
    if (fgets(buf, 20, fin) == NULL)
    {
      printf("error: too many fins!\n");
      fclose(fin);
      exit(1);
    }
  fscanf(fin, "%d", &EX_OR[0]);
  for (ia = 1; ia <= EX_OR[0]; ia++)
    fscanf(fin, "%d", &EX_OR[ia]);
  fclose(fin);

  for (ia = 0; ia < chainnum; ia++)
  {
    if (ia == 0)
      schain[ia].top = 0;
    else
      schain[ia].top = schain[ia - 1].top + schain[ia - 1].length;

    schain[ia].length = (ffnum - schain[ia].top) / (chainnum - ia);
    //schain[ia].lastval = ff_state[schain[ia].top + schain[ia].length - 1];
    schain[ia].lastval = ff_state[schain[ia].top];
  }
}

int PeakTogCount(ff_state, chainum) int ff_state[];
{
  int ia, bitval, togglecount = 0;
  bitval = ff_state[schain[chainum].top];
  //printf("bitval %d \n",bitval);

  for (ia = schain[chainum].top + 1; ia < schain[chainum].top + schain[chainum].length; ia++)
  {
    if (ff_state[ia] != bitval)
    {
      togglecount++;
      bitval = ff_state[ia];
    }
  }
  return togglecount;
  //printf("chain %d \n",togglecount);
}

void lfsr_next(EX_OR, lfsr_state, ff_state) int EX_OR[];
int lfsr_state[];
int ff_state[];
{
  int ia, ib, i;
#if POWEREVA&&PEAK
  int OrigPatTog[chainnum], TogPerClk[chainnum];
  int Tem_state[ffnum];
  for (ia = 0; ia < ffnum; ia++)
    Tem_state[ia] = ff_state[ia];
  for (ia = 0; ia < chainnum; ia++)
  {
    OrigPatTog[ia] = 0;
    TogPerClk[ia] = 0;
    OrigPatTog[ia] = PeakTogCount(ff_state, ia);
    //printf("chain %d = %d \n",ia,OrigPatTog[ia]);
  }
#endif

  for (ia = 0; ia <= (ffnum - 1) / chainnum; ia++)
  {

    for (ib = 0; ib < chainnum; ib++)
    {

      if (schain[ib].length == (ffnum - 1) / chainnum)
      {
        if (ia != 0)
          ff_state[schain[ib].top + schain[ib].length - ia] = lfsr_state[ib];
      }
      else
        ff_state[schain[ib].top + schain[ib].length - ia - 1] = lfsr_state[ib];
#if POWEREVA
      if (ff_state[schain[ib].top + schain[ib].length - ia - 1] != schain[ib].lastval)
      {
        toggle_scn += schain[ib].length - ia;
        toggle_scn_in += schain[ib].length - ia;
        toggle_shift_perpat += schain[ib].length - ia;
#if PEAK
        TogPerClk[ib] = OrigPatTog[ib] + 1;
        if (ia <= schain[ib].length - 2 && Tem_state[schain[ib].top + schain[ib].length - ia - 1] == Tem_state[schain[ib].top + schain[ib].length - ia - 2])
          OrigPatTog[ib]++;

#endif
      }
      else
      {
#if PEAK
        TogPerClk[ib] = OrigPatTog[ib];
        if (ia <= schain[ib].length - 2 && Tem_state[schain[ib].top + schain[ib].length - ia - 1] != Tem_state[schain[ib].top + schain[ib].length - ia - 2])
          OrigPatTog[ib]--;
#endif
      }
#endif

#if PEAK
      if (TogPerClk[ib] > ShiftPeak[ib])
        ShiftPeak[ib] = TogPerClk[ib];
#endif
      schain[ib].lastval = ff_state[schain[ib].top + schain[ib].length - ia - 1];
    }

#if DEBUG
    if (ia == 0)
    {
      printf("\nLFSR state:\n");

      printf("     ");
      for (ib = 0; ib <= LFSR_BIT; ib++)
        printf("%2d ", ib);
      printf("\n");

      printf("     ");
      for (ib = 0; ib <= LFSR_BIT; ib++)
        printf("===", ib);
      printf("\n");
    }

    printf("%3d |", ia);

    for (ib = 0; ib <= LFSR_BIT; ib++)
      printf(" %d ", lfsr_state[ib]);
    printf("\n");
#endif //LFSR pattern input

    if (clocktime == 1 && ia == 0)
    {
      //phase_shifter(lfsr_state, LFSR_BIT);
      for (i = 0; i < TAP_NUM; i++)
        TapGen_state[i] = 0;
      for (i = 0; i < LFSR_BIT; i++)
        TapGen_state[TAP_NUM / 2] |= (lfsr_state[i] << (LFSR_BIT - i - 1));
    }

    InternalLfsrGen(EX_OR);

    for (ib = 0; ib < LFSR_BIT; ib++)
      lfsr_state[ib] = (TapGen_state[TAP_NUM / 2 + 1] >> (LFSR_BIT - ib - 1)) & 1;

    TapGen_state[TAP_NUM / 2] = TapGen_state[TAP_NUM / 2 + 1];
    TapGen_state[TAP_NUM / 2 + 1] = 0;
  }
#if DEBUG
  printf("\n");

  for (ia = 0; ia < chainnum; ia++)
  {
    printf("sc%-2d:", ia);

    for (ib = 0; ib < schain[ia].length; ib++)
      printf("%d", ff_state[schain[ia].top + ib]);
    printf("\n");
  }

#endif
}
