#include <stdio.h>
#include <stdlib.h>

int main() {

  FILE *file;
  if((file = fopen("triangles.txt", "r")) == NULL) {
    printf("file not found");
    return 1;
  }

  int row = 0;

  int firstI;
  while(fscanf(file, "%d", &firstI) == 1) {
    printf("%d\n", (short int)firstI);
    row++;
  }


  printf("found %d rows\n", row);
}
