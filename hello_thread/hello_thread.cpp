#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void* print(void* args) {

  struct arg_struct *args = arguments

  long num = arguments->arg1;
  int nbthread = arguments->arg2;
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
    struct arg_struct args; args.arg1 = i; args.arg2 = threadcount;
    pthread_create(&hellothreads[i], NULL, print,(void*)&args);
  }
  for (long i = 0; i < threadcount; i++) {
    pthread_join(hellothreads[i], NULL));
  }


  }
  
  return 0;
}
