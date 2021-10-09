#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void* print(void* text) {

  cout<<"I am thread " <<(long)p<< " in nbthreads"<<endl;
  return NULL;
}


int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
  }

  int threadcount = atol(argv[1]);

  pthread_t hellothreads = new pthread_t[threadcount];

  for (int i = 0; i < threadcount; i++) {
    pthread_create(&hellothreads[i], NULL, print,(void*)i);
  }
  for (int i = 0; i < threadcount; i++) {
    pthread_join(hellothreads[i], NULL));
  }


  }
  
  return 0;
}
