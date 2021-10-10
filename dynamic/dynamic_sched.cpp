#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <math.h>

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);


pthread_mutex_t mutex_iter, mutex_chunk, task_getter_mutex;
int next_available_beginning, next_available_end;

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



bool task_done = false;
int current_start = 0;
float summed_value = 0;

struct args
{
  int f;
  float a;
  float b;
  int start;
  int end;
  int intensity;
  float pre_product;
  float inside_sum;
  float granularity;
  int n;
}current_args;

float summation_iteration(int begin,int end)
{
	int i;
	float function_eval;

	int f = current_args.f;
        float a = current_args.a;
        float b= current_args.b;
        int start = begin;
        int intensity = current_args.intensity;
        float pre_product = current_args.pre_product;
	float mysummed_valuei = 0;
  for(i=start;i<end;i++)
  {
    switch(f)
    {
      case 1:
        function_eval = f1(a + (i+0.5)*pre_product,intensity);
	break;
      case 2:
        function_eval = f2(a + (i+0.5)*pre_product,intensity);
        break;
      case 3:
        function_eval = f3(a + (i+0.5)*pre_product,intensity);
        break;
      case 4:
        function_eval = f4(a + (i+0.5)*pre_product,intensity);
        break;
  }

	          pthread_mutex_lock(&mutex_iter);
		  summed_value += function_eval;
		  pthread_mutex_unlock(&mutex_iter);
}	
	return summed_value;
}

float summation_thread(int begin,int end)
{
	int i;
	float function_eval;

	int f = current_args.f;
        float a = current_args.a;
        float b= current_args.b;
        int start = begin;
        int intensity = current_args.intensity;
        float pre_product = current_args.pre_product;
	float mysummed_value = 0;
  for(i=start;i<end;i++)
  {
    switch(f)
    {
      case 1:
        function_eval = f1(a + (i+0.5)*pre_product,intensity);
	break;
      case 2:
        function_eval = f2(a + (i+0.5)*pre_product,intensity);
        break;
      case 3:
        function_eval = f3(a + (i+0.5)*pre_product,intensity);
        break;
      case 4:
        function_eval = f4(a + (i+0.5)*pre_product,intensity);
        break;
  }
		  mysummed_value += function_eval;
}	
		  return mysummed_value;
}

float summation_chunk(int begin,int end)
{
	int i;
	float function_eval;

	int f = current_args.f;
        float a = current_args.a;
        float b= current_args.b;
        int start = begin;
        int intensity = current_args.intensity;
        float pre_product = current_args.pre_product;
	float mysummed_value = 0;
  for(i=start;i<end;i++)
  {
    switch(f)
    {
      case 1:
        function_eval = f1(a + (i+0.5)*pre_product,intensity);
	break;
      case 2:
        function_eval = f2(a + (i+0.5)*pre_product,intensity);
        break;
      case 3:
        function_eval = f3(a + (i+0.5)*pre_product,intensity);
        break;
      case 4:
        function_eval = f4(a + (i+0.5)*pre_product,intensity);
        break;
  }
    mysummed_value += function_eval;
}	

    		  pthread_mutex_lock(&mutex_chunk);
		  summed_value += mysummed_value;
    		  pthread_mutex_unlock(&mutex_chunk);
		  return summed_value;
}

void* get_next_task(int &begin, int &end, bool &task_test)
{
	//mutex lock here
	pthread_mutex_lock(&task_getter_mutex);
	task_test = task_done;
	begin = next_available_beginning;
	end = next_available_end;
	//cout<<endl<<begin<<" "<<end<<endl;
	if(end == current_args.n)
	{
		task_done = true;
	        pthread_mutex_unlock(&task_getter_mutex);
		return NULL;
	}
	next_available_beginning = next_available_beginning + current_args.granularity;
	if(current_args.n - (next_available_beginning + current_args.granularity) >= current_args.granularity)
	{
		next_available_end = next_available_beginning + current_args.granularity;
	}
	else
		next_available_end = current_args.n;
	
	pthread_mutex_unlock(&task_getter_mutex);
	return NULL;
	//UNLOCK MUTEX
	 
}	
bool fetch_task_done()
{

	return task_done; 
}
void* worker_threads_iteration(void* i)
{
	bool task_test;
	pthread_mutex_lock(&task_getter_mutex);
	task_test = task_done;
	pthread_mutex_unlock(&task_getter_mutex);

	//bool task_done1 = fetch_task_done();
	while(task_test!=true)
	{
		int begin,end;
		float mysum;
		get_next_task(begin,end,task_test);
		if(task_test == true)
			return NULL;

		summation_iteration(begin,end);	
	        pthread_mutex_lock(&task_getter_mutex);
     	        task_test = task_done;
	        pthread_mutex_unlock(&task_getter_mutex);
	        		
	}
	return NULL;

}
void* worker_threads_thread(void* mysum)
{
	bool task_test;
	pthread_mutex_lock(&task_getter_mutex);
	task_test = task_done;
	pthread_mutex_unlock(&task_getter_mutex);

	while(task_test!=true)
	{
		int begin,end;
		get_next_task(begin,end,task_test);
		if(task_test == true)
			return NULL;
		(*((float*)mysum)) += summation_thread(begin,end);	
		pthread_mutex_lock(&task_getter_mutex);
		task_test = task_done;
		pthread_mutex_unlock(&task_getter_mutex);
	        		
	}
	return NULL;

}
void* worker_threads_chunk(void* sum1)
{
	bool task_test;
	pthread_mutex_lock(&task_getter_mutex);
	task_test = task_done;
	pthread_mutex_unlock(&task_getter_mutex);
        while(task_test!=true)
	{
		int begin,end;
		float mysum;
		get_next_task(begin,end,task_test);
		if(task_test == true)
			return NULL;
		summation_chunk(begin,end);	
		pthread_mutex_lock(&task_getter_mutex);
		task_test = task_done;
		pthread_mutex_unlock(&task_getter_mutex);
	        		
	}
	return NULL;

}
int main (int argc, char* argv[]) {
  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }


  int functionid = atoi(argv[1]); //Converting function ID from char* to int
  float a = atof(argv[2]); //Converting a value to float as per question
  float b = atof(argv[3]); // converting b value to float as per question
  int n = atoi(argv[4]); //converting n to integer as per q
  int intensity = atoi(argv[5]);
  int nbthreads = atoi(argv[6]); //number of threads to integer
  char* sync = argv[7];
  int granularity = atoi(argv[8]);

  pthread_mutex_init(&mutex_chunk,NULL);
  pthread_mutex_init(&mutex_iter,NULL);
  pthread_mutex_init(&task_getter_mutex,NULL);

  pthread_t *integral_threads = new pthread_t[nbthreads];

  current_args.f = functionid;
  current_args.a = a;
  current_args.b = b;
  current_args.n = n;
  current_args.intensity = intensity;


  float pre_product = (b-a)/n;

  current_args.pre_product = pre_product;
  current_args.granularity = granularity;


  next_available_beginning = 0;
  float mysum[nbthreads];
  if(granularity<n)
	  next_available_end = 0 + granularity;
  else
	  next_available_end = n;


  std::chrono::time_point<std::chrono::system_clock> start_clock, end_clock;
        start_clock = std::chrono::system_clock::now();

  if(strcmp(sync,"iteration")==0)
  {
    for(int i=0;i<nbthreads;i++)
	    pthread_create(&integral_threads[i],NULL,worker_threads_iteration,(void*)&i);
    
  
    for(int i =0;i<nbthreads;i++)
	  pthread_join(integral_threads[i],NULL);


  cout<<summed_value*pre_product;
    
    
    
  }

  else if(strcmp(sync,"chunk")==0)
  {
    for(int i=0;i<nbthreads;i++)
	    pthread_create(&integral_threads[i],NULL,worker_threads_chunk,(void*)&mysum);
  for(int i =0;i<nbthreads;i++)
	  pthread_join(integral_threads[i],NULL);
  cout<<summed_value*pre_product;
  }

  else if(strcmp(sync,"thread")==0)
  {
    for(int i=0;i<nbthreads;i++)
	    pthread_create(&integral_threads[i],NULL,worker_threads_thread,(void*)&mysum[i]);
  for(int i =0;i<nbthreads;i++)
	  pthread_join(integral_threads[i],NULL);

  float threadtotal = 0;
  for(int i=0;i<nbthreads;i++)
	  threadtotal += mysum[i];

  cout<<threadtotal*pre_product;



  }

  else
  {
	  cout<<endl<<" Give a valid sync value "<<endl;
	  return -1;
  }
  end_clock = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_clock-start_clock;
        cerr<<elapsed_seconds.count();



  
  



  return 0;
  
}