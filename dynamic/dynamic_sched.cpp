#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//mine
#include <math.h>
#include <time.h>
using namespace std;

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

// Undefined reference to functions so I manually added them here

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



//Mutex
pthread_mutex_t mut, schedule_mut;

float sum=0.0;
int n;
int segment_count, segments = 0;

struct args {

  int functionid;
  float a;
  float b;
  int start;
  int end;
  int intensity;
  float threadsum;
  char* sync;

  // New for dynamic, how many iterations it takes
  int granularity;

}*threadargs;


// Dynamic Stuff
void* worker(void* arguments) {

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
  char* sync = params->sync;

  segments = n/granularity;
  if(strcmp(sync, "chunk")==0) {
    // Store inner loop locally, add to shared variable once per call
    while (segment_count < segments) {

      // Claim next segment of iterations
      pthread_mutex_lock(&schedule_mut);

        start = segment_count * granularity;
        end = start + granularity;
        if (end > n) end = n; //Don't go over max
        if (start > n) start = n;

        segment_count++;

      pthread_mutex_unlock(&schedule_mut);

      float compute = 0.0;
      for (int i = start; i < end; i++) {
        switch(functionid) {
        case 1:
          compute += f1(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 2:
          compute += f2(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 3:
          compute += f3(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 4:
          compute += f4(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        }
      }

      pthread_mutex_lock(&mut);
      sum += compute;
      pthread_mutex_unlock(&mut);

    }
    return NULL;
  }
  else if(strcmp(sync, "thread")==0) {
    //One value per thread, add to shared variable at the end
    float compute = 0.0;
    while(segment_count < segments) {

      // Claim next segment of iterations
      pthread_mutex_lock(&schedule_mut);

        start = segment_count * granularity;
        end = start + granularity;
        if (end > n) end = n; //Don't go over max
        if (start > n) start = n;

        segment_count++;

      pthread_mutex_unlock(&schedule_mut);

      for (int i = start; i < end; i++) {
        switch(functionid) {
        case 1:
          compute += f1(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 2:
          compute += f2(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 3:
          compute += f3(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 4:
          compute += f4(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        }
      }
    }

    pthread_mutex_lock(&mut);
    sum = compute;
    pthread_mutex_unlock(&mut);

    return NULL;
  }
  else if(strcmp(sync, "iteration")==0) {
    //Add to shared variable in inner loop
    while(segment_count < segments) {

      // Claim next segment of iterations
      pthread_mutex_lock(&schedule_mut);

        start = segment_count * granularity;
        end = start + granularity;
        if (end > n) end = n; //Don't go over max
        if (start > n) start = n;

        segment_count++;

      pthread_mutex_unlock(&schedule_mut);

      for (int i = start; i < end; i++) {
        pthread_mutex_lock(&mut);
        switch(functionid) {
        case 1:
          sum += f1(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 2:
          sum += f2(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 3:
          sum += f3(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        case 4:
          sum += f4(a + (i+.5)*(((float)b-(float)a)/(float)n), intensity);
        break;
        }
        pthread_mutex_unlock(&mut);
      }
    }
    return NULL;
  }
  else {
    printf("Invalid sync input");
    return 0;
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

    for(int i = 0; i < nbthreads; i ++) {
        threadargs[i].functionid = function_id;
        threadargs[i].a = a;
        threadargs[i].b = b;
        threadargs[i].intensity = intensity;
        threadargs[i].sync = sync;
        threadargs[i].granularity = granularity;

      pthread_create(&threads[i], NULL, worker, (void*)&threadargs[i]);

    }
  
  for(int i = 0; i < nbthreads; i++) {
      pthread_join(threads[i], NULL);
  }


  // Apply product
  sum *= outside;

  float result = sum;
  // end code

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

  // report reult and time
  std::cout<<result<<std::endl;
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}