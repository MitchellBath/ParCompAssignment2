#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

//Mutex
pthread_mutex_t iteration_mut, chunk_mut, get_mut;

float sum=0;
int nextend, nextbegin;
int n;

struct args {

  int functionid;
  float a;
  float b;
  int start;
  int end;
  int intensity;
  float threadsum;

  // New for dynamic, how many iterations it takes
  int granularity;

}threadargs;

void* iteration_calc_numerical_integration(void* arguments) {

    struct args *params = (struct args *)arguments;

    // floats?
    int functionid = params->functionid;
    int a = params->a;
    int b = params->b;
    int start = params->start;
    int end = params->end;
    int intensity = params->intensity;

    float (*f)(float, int);
    if (functionid == 1) {
        f = f1;
    }
    else if (functionid == 2) {
        f = f2;
    }
    else if (functionid == 3) {
        f = f3;
    }
    else if (functionid == 4) {
        f = f4;
    } else {
        return 0;
    }

    for (int i = start; i < end; i++) {

        //double compute = f(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        double compute;
      switch(functionid) {
        case 1:
          compute = f1(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 2:
          compute = f2(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 3:
          compute = f3(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 4:
          compute = f4(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
      }

        pthread_mutex_lock(&iteration_mut);
        sum += compute;
        pthread_mutex_unlock(&iteration_mut);
    }

    // Return NULL? Mutex violation maybe here
    return NULL;
}

void* thread_calc_numerical_integration(void* arguments){

    struct args *params = (struct args *)arguments;

    // floats?
    int functionid = params->functionid;
    int a = params->a;
    int b = params->b;
    int start = params->start;
    int end = params->end;
    int intensity = params->intensity;

    double summation = 0;

    float (*f)(float, int);
    if (functionid == 1) {
        f = f1;
    }
    else if (functionid == 2) {
        f = f2;
    }
    else if (functionid == 3) {
        f = f3;
    }
    else if (functionid == 4) {
        f = f4;
    } else {
        return 0;
    }

    for (int i = start; i < end; i++) {
        //summation += f(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        switch(functionid) {
        case 1:
          summation += f1(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 2:
          summation += f2(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 3:
          summation += f3(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 4:
          summation += f4(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
      }
    }

    params->threadsum = summation;
    return NULL;
}

void* chunk_calc_numerical_integration(void* arguments){
  struct args *params = (struct args *) arguments;

  int functionid = params->functionid;
  int a = params->a;
  int b = params->b;
  int start = params->start;
  int end = params->end;
  int intensity = params->intensity;

  float summation = 0;

  for (int i = start; i < end; i++) {
      //summation += f(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
      switch(functionid) {
      case 1:
        summation += f1(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
      break;
      case 2:
        summation += f2(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
      break;
      case 3:
        summation += f3(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
      break;
      case 4:
        summation += f4(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
      break;
      }
  }

  pthread_mutex_lock(&chunk_mut);
  sum += summation;
  pthread_mutex_unlock(&chunk_mut);

  // Null??
  return sum;

}

// Dynamic Stuff
void* worker_chunk(void* arguments) {

  /*
    while(!loop.done) {
      int begin, end;
      loop.getnext(&begin, &end);
      execute_inner_loop(begin, end);
    }
  */

  struct args *params = (struct args *) arguments;

  int functionid = params->functionid;
  int a = params->a;
  int b = params->b;
  int start = params->start;
  int end = params->end;
  int intensity = params->intensity;
  int granularity = params->granularity;

  float summation = 0;

  bool done = false;

  while(!done) {

    int begin, end;


  }


}





int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }
    
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  // do your calculation here
   // commandline arguments
  int function_id = atoi(argv[1]);
  int a = atoi(argv[2]);
  int b = atoi(argv[3]);
  n = atoi(argv[4]);
  int intensity = atoi(argv[5]);

  // New arguments for parallelization & dynamic
  int nbthreads = atoi(argv[6]);
  char* sync = argv[7];
  int granularity = atoi(argv[8]);
  //printf("funcid:%i, a:%i, b:%i, n:%i, int:%i\n", function_id, a, b, n, intensity);//10 intensity?
  //printf("threads:%i, type:%s\n", nbthreads, sync);

  if(n<nbthreads) nbthreads = n; // Too many threads?
  int divide = n/nbthreads;
  int extra = n % nbthreads;
  //if (divide*nbthreads < n) int extra = n - (divide*nbthreads); // throw on last threads for uneven n
  threadargs = new args[nbthreads];

  pthread_t *threads = new pthread_t[nbthreads];
  pthread_mutex_init(&mut,NULL);
  
  //clock_t t; // t represents clock ticks which is of type 'clock_t'
  //t = clock(); // start clock

  float outside = ((float)b - (float)a)/(float)n;
  // multiply thread result with outside

  if(strcmp(sync, "thread")==0) {
    for(int i = 0; i < nbthreads; i ++) {
        threadargs[i].functionid = function_id;
        threadargs[i].a = a;
        threadargs[i].b = b;
        threadargs[i].start = divide*(i);
        threadargs[i].end = divide*(i+1);
        if (i + divide >= n) threadargs[i].end = n; // Throw on extra threads for uneven n
        threadargs[i].intensity = intensity;

      pthread_create(&threads[i], NULL, thread_calc_numerical_integration, (void*)&threadargs[i]);

    }
  }
  // Dynamic
  else if (strcmp(sync, "chunk")==0){
    
    for(int i = 0; i < nbthreads; i ++) {
        threadargs[i].functionid = function_id;
        threadargs[i].a = a;
        threadargs[i].b = b;
        threadargs[i].start = divide*(i);
        threadargs[i].end = divide*(i+1);
        if (i + divide >= n) threadargs[i].end = n; // Throw on extra threads for uneven n
        threadargs[i].intensity = intensity;
        threadargs[i].granularity = granularity;

      pthread_create(&threads[i], NULL, worker_chunk, (void*)&threadargs[i]);
    }

  }
  else if(strcmp(sync, "iteration")==0) {
    for(int i = 0; i < nbthreads; i ++) {
        threadargs[i].functionid = function_id;
        threadargs[i].a = a;
        threadargs[i].b = b;
        threadargs[i].start = divide*(i);
        threadargs[i].end = divide*(i+1);
        if (i + divide >= n) threadargs[i].end = n; // Throw on extra threads for uneven n
        threadargs[i].intensity = intensity;

        pthread_create(&threads[i], NULL, iteration_calc_numerical_integration, (void*)&threadargs[i]);  

    }
  }
  else {
    printf("Invalid input (sync)");
    return 0;
  }

  float total = 0;
  float results[nbthreads];
  for(int i = 0; i < nbthreads; i++) {
      pthread_join(threads[i], NULL);
      results[i] = threadargs[i].threadsum;
  }
  // Results are for thread, not iteration
  if(strcmp(sync, "thread")==0){
    for(int i = 0; i < nbthreads; i++) {
        total += results[i];
    }
  }
  // Iteration result
  else if(strcmp(sync, "iteration")==0) {
    total = sum;
  }
  // Apply product
  total *= outside;

  float result = total;
  // end code

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

  // report reult and time
  std::cout<<result<<std::endl;
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}