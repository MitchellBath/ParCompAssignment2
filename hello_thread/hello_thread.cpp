#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void* print(void* num) {

  cout<<"I am thread " <<(long)num<< " in nbthreads"<<endl;
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
    pthread_create(&hellothreads[i], NULL, print,(void*)i);
  }
  for (long i = 0; i < threadcount; i++) {
    pthread_join(hellothreads[i], NULL));
  }


  }
  
  return 0;
}
