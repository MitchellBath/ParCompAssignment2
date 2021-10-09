#include <iostream>
#include <stdio.h>
#include <stdlib.h>

struct args {
  long i;
  int nbthread;
};

void* print(void* input) {

  struct arg_struct *args = arguments;

  long num = ((struct args*)input)->i;
  int nbthread = ((struct args*)input)->nbthread;
  cout<<"I am thread " <<(long)num<< " in "<<(int)nbthread;
  return NULL;
}


int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
  }

  int threadcount = atol(argv[1]);

  pthread_t *hellothreads = new pthread_t[threadcount];

  for (long i = 0; i < threadcount; i++) {
    struct args *arguments = (struct args *)malloc(sizeof(struct args));
    arguments->i = i;
    arguments->nbthread = threadcount;
    pthread_create(&hellothreads[i], NULL, print,(void*)arguments);
  }
  for (long i = 0; i < threadcount; i++) {
    pthread_join(hellothreads[i], NULL));
  }
  
  return 0;
}
