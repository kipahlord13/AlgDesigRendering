#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int x;
pthread_mutex_t xLock;

void *inc_x(void *in) {

  while(1) {
    //pthread_mutex_lock(&xLock);
    x++;
    //pthread_mutex_unlock(&xLock);
  }

  return NULL;
}

int main() {

  if (pthread_mutex_init(&xLock, NULL) != 0) {
    printf("\n mutex init has failed\n");
    return 1;
  }

  pthread_t inc_x_thread;

  if(pthread_create(&inc_x_thread, NULL, inc_x, NULL)) {

    fprintf(stderr, "Error creating thread\n");
    return 1;

  }

  while(1) {
    //pthread_mutex_lock(&xLock);
    printf("X: %d\n", x);
    //pthread_mutex_unlock(&xLock);
    usleep(10000);
  }

  return 0;
}
