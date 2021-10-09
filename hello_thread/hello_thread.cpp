#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
  }
  
  return 0;
}
