#include <stdio.h>
#include <stdlib.h>

int main() {

  FILE *file;
  if((file = fopen("meshVerts.txt", "r")) == NULL) {
    printf("file not found");
    return 1;
  }

  float firstF, secndF, thirdF;
  while(fscanf(file, "%f %f %f", &firstF, &secndF, &thirdF) == 3) {
    printf("hey bud I found these floats: %f %f %f\n", firstF, secndF, thirdF);
  }
  return 0;
}
