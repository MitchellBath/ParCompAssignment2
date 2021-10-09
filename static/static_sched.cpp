#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cmath>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <mutex>

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);
// Old functions. Undefined reference to f1/f2/f3/f4 when make

#ifdef __cplusplus
}
#endif

// Mutex and mutex resource sum
pthread_mutex_t mutex;
float sum = 0.0;

int n;

float f1(float x, int intensity) {
  int sign = x > 0;
  
  for (int i=0; i< intensity; ++i) {
    x = x*x;
    x = sqrt(x);
  }
  return (sign?1:-1)*x;
}

float f2(float x, int intensity) {
  float realx = f1(x, intensity);
  return realx*realx;
}

float f3(float x, int intensity) {
  return sin(f1(x,intensity));
}

float f4(float x, int intensity) {
  return exp(cos(f1(x,intensity)));
}

struct args {

  int functionid;
  float a;
  float b;
  int start;
  int end;
  int intensity;
  float threadsum;

}*threadargs;

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

        double compute = f(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        pthread_mutex_lock(&mutex);
        sum += compute;
        pthread_mutex_unlock(&mutex);
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
        summation += f(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
    }

    params->threadsum = summation;
    return NULL;
}

int main (int argc, char* argv[]) {

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
  
  // commandline arguments
  int function_id = atoi(argv[1]);
  int a = atoi(argv[2]);
  int b = atoi(argv[3]);
  n = atoi(argv[4]);
  int intensity = atoi(argv[5]);

  // New arguments for parallelization
  int nbthreads = atoi(argv[6]);
  char* sync = argv[7];
  //printf("funcid:%i, a:%i, b:%i, n:%i, int:%i", function_id, a, b, n, intensity); 10 intensity?

  if(n<nbthreads) nbthreads = n; // Too many threads?
  int divide = n/nbthreads;
  //if (divide*nbthreads < n) int extra = n - (divide*nbthreads); // throw on last threads for uneven n
  threadargs = new args[nbthreads];

  pthread_t *threads = new pthread_t[nbthreads];
  pthread_mutex_init(&mutex,NULL);
  
  clock_t t; // t represents clock ticks which is of type 'clock_t'
  t = clock(); // start clock

  float outside = ((float)b - (float)a)/(float)n;
  // multiply thread result with outside

  if(strcmp(sync, "thread")==0) {
    for(int i = 0; i < n; i += divide) {
      threadargs[i].functionid = function_id;
      threadargs[i].a = a;
      threadargs[i].b = b;
      threadargs[i].start = i;
      threadargs[i].end = i+divide;
      if (i + divide >= nbthreads) threadargs[i].end = n; // Throw on extra threads for uneven n
      threadargs[i].intensity = intensity;

      pthread_create(&threads[i], NULL, thread_calc_numerical_integration, (void*)&threadargs[i]);

    }
  }
  else if(strcmp(sync, "iteration")==0) {
    for(int i = 0; i < n; i += divide) {
      threadargs[i].functionid = function_id;
      threadargs[i].a = a;
      threadargs[i].b = b;
      threadargs[i].start = i;
      threadargs[i].end = i+divide;
      if (i + divide >= nbthreads) threadargs[i].end = n; // Throw on extra threads for uneven n
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
  
  //double r = calc_numerical_integration(function_id, a, b, n, intensity);
  t = clock()-t; // end clock
  float time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds; CLOCKS_PER_SEC is the number of clock ticks per second
  printf("%lf\n", total);
  fprintf(stderr, "%f\n", time_taken);

  return 0;
}
