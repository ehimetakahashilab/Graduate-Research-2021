#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10000
#define capmax 20

// ./a.out フリップフロップ総数
// ex) ./a.out 669

int main(int argc, char* argv[])
{
  FILE* fp;
  int qnt;
  int i, j, tmp;
  int arr[SIZE];

  qnt = atoi(argv[1]);
  srand((unsigned)time(NULL));

  for (i = 0; i < qnt; i++) {
	  arr[i] = i + 1;
  }
  

  for (i = 0; i < qnt; i++) {
	  j = rand() % qnt;
	  tmp = arr[i];
	  arr[i] = arr[j];
	  arr[j] = tmp;
  }


  fp = fopen("rffcp", "w");
  
  for(i = 0; i < qnt; i++){
	  fprintf(fp, "%d\n" , arr[i] );
  }

  fclose(fp);
  
  return 0;

}
