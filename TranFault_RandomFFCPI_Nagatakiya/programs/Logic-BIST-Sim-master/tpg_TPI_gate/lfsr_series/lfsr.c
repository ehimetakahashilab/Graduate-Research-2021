#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
  int ia, ib, ic, test_vec, nfin, tmp;
  FILE *fin, *fout;

  if(argc < 3){
    printf("error: too less arguments!\n");
    exit(1);
  }

  fin = fopen(argv[1], "r");
  if(fin == NULL){
    printf("error: '%s' is not found\n", argv[1]);
    exit(1);
  }
  fscanf(fin, "%d %d %d %d %d", &nfin, &nfin, &nfin, &tmp, &tmp);
  fclose(fin);

  nfin += tmp;

  int X[17];

  fout = fopen("tmp.test", "w");

  test_vec = atoi(argv[2]);

  fprintf(fout, "%d\n", test_vec);

  for(ia=0; ia<17; ia++){
    if((ia % 3)^(ia % 5)^(ia < 10))
      X[ia] = 1;
    else
      X[ia] = 0;
  }

  for(ia=0; ia<test_vec; ia++){
    for(ib=0; ib<nfin; ib++){
      for(ic=16; ic>=1; ic--)
        X[ic] = X[ic-1];

      X[5] ^= X[16];
      X[4] ^= X[16];
      X[3] ^= X[16];
      X[0] = X[16];

      fprintf(fout, " %d", X[16]);
    }
    fprintf(fout, "\n");
  }
  //fprintf(fout, "END\n");
  fclose(fout);

  return 0;
}

