#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <vector>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif


namespace utils{
  
  vector<float> getGrayScaleVector(CImg<unsigned char>& I,int x,int y,int size){
    vector<float> gsv;
    int n = (size-1)/2;
    for (int i = -n; i <n ; i++){
      for(int j = -n; j < n; j++){
	gsv.push_back((float)I(x+i,y+j));
      }
    }
    return gsv;
  }

  float euclideanDistance(vector<int>& a, vector<int>& b){
    float ed = 0;
    if(a.size() == b.size()){
      for(int i = 0; i < a.size();i++){
	ed += pow((b[i]-a[i]),2);
      }
      return sqrt(ed);
    }
    else{
      return ed;
    }
  }

  float norme(vector<float>& x){
    float n = 0;
    for (int i = 0; i < x.size(); i++){
      n += pow(x[i],2);
    }
    return sqrt(n);
  }

  float gaussianKernel(vector<float>& x, float h){
    float content = -(pow(norme(x),2)/(4*h*h));
    float div = 1/(4*h*h*M_PI);
    return div*exp(content);
  }

  float normaliseConstent(CImg<unsigned char>& I, vector<float>& gsv, int x, int y, int patchSize, float h){
    float a = gaussianKernel(gsv,h);
  }
      
};


#endif
