#include <math.h>
#include <stdlib.h>


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
  
float sequential_integrate (int argc, char* argv[]) {
  
  int functionid = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);

  float (*f) (float, int);
  f = NULL;
  
  if (functionid == 1) {
    f = f1;
  }
  if (functionid == 2) {
    f = f2;
  }
  if (functionid == 3) {
    f = f3;
  }
  if (functionid == 4) {
    f = f4;
  }

  double sum = 0;
  
  for (int i = 0; i< n; ++i) {
    float x = a+(i+.5)*(b-a)/n;

    // manual functions
    float val;
    if (functionid == 1) {
        int sign = x > 0;
      for (int i=0; i< intensity; ++i) {
        x = x*x;
        x = sqrt(x);
      }
      val = (sign?1:-1)*x;
    }
    if (functionid == 2) {
      int sign = x > 0;
      for (int i=0; i< intensity; ++i) {
        x = x*x;
        x = sqrt(x);
      }
      val = (sign?1:-1)*x;
      val = val*val;
    }
    if (functionid == 3) {
      int sign = x > 0;
      for (int i=0; i< intensity; ++i) {
        x = x*x;
        x = sqrt(x);
      }
      val = (sign?1:-1)*x;
      val = sin(val);
    }
    if (functionid == 4) {
      int sign = x > 0;
      for (int i=0; i< intensity; ++i) {
        x = x*x;
        x = sqrt(x);
      }
      val = (sign?1:-1)*x;
      val = exp(cos(val));
    }
    //float val = f(x, intensity);

    //std::cout<<val<<std::endl;

    sum += val;
  }
  float integrate = sum * (b-a)/n;


  return integrate;
}
