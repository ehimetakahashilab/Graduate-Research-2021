#include<stdio.h>
#include<stdlib.h>

#define BUF_MAX 20

int main(int argc, char *argv[]){
  int i, count;
  char buf[BUF_MAX];
  FILE *fin, *fout;

/*
  if(argc < 2){
    printf("Please input filename on command line\n");
    exit(1);
  }
*/

//fin = fopen(argv[1], "r");
  fin = fopen("lfsr.dat", "r");
  if(fin == NULL){
    printf("error: File '%s' is not found\n", argv[1]);
    exit(1);
  }

  fout = fopen("tmp.dat", "w");
  while(fgets(buf, BUF_MAX, fin) != NULL){
    i=0;
    count = 1;
    while(buf[i] != '\n'){
      if(buf[i] == ','){
	buf[i] = ' ';
	count++;
      }
      i++;
    }
    fprintf(fout, "%d ", count);
    fprintf(fout, "%s", buf);
  }
  
  fclose(fin);
  fclose(fout);

  return 0;
}
	
