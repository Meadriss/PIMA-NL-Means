#ifndef OPERATOR_H
#define OPERATOR_H

#include <cmath>
#include <vector>

#include "CImg.h"
#include "utils.h"

using namespace std;
using namespace cimg_library;
using namespace utils;

#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif

namespace operatorMorpho{
  
  // max vector
  float maxElem(vector<float> gsvI){
    float max = 0;
    if(gsvI.size() == 0){
      return max;
    }
    for(unsigned int i = 0; i < gsvI.size(); i++){
      if(gsvI[i] > max){
	max = gsvI[i];
      }
    }
    return max;
  }

  //min vector
  float minElem(vector<float> gsvI){
    float min = -1;
    if(gsvI.size() == 0){
      return 0;
    }
    for(int i = 0; i < gsvI.size(); i++){
      if (min == -1){
	min = gsvI[i];
      }
      if(gsvI[i] < min){
	min = gsvI[i];
      }
    }
    return min;
  }

  float morphoStandarDeviationGauss(int x, int y, float h){
    float content = -((pow(x,2)+pow(y,2))/(2*h*h));
    float div = 1/(h*h*sqrt(2*M_PI));
    return div*exp(content);
  }
  
  vector<float> elementStruct(int size){
    vector<float> element;
    int ksize = (size-1)/2;
    for(int i = -ksize; i <= ksize; i++){
      for(int j = -ksize; j <= ksize; j++){
	element.push_back(1);
      }
    }
    return element;
  }

  double morphoEucliDistGauss(vector<float>& a, vector<float>& b,float h,float alpha,vector<float> kernel){
    float ed = 0;
    if(a.size() == b.size()){
      int size = (sqrt(a.size())-1)/2;
      vector<float> g;
      for(int i = -size; i <= size; i++){
	for(int j = -size; j <= size; j++){
	  g.push_back(morphoStandarDeviationGauss(i,j,h));
	}
      }
      for(int k = 0; k < a.size();k++){
	float x = b[k]-a[k];
	ed += pow(sqrt(pow(x,2)),2)*g[k];
      }
      return ed;
    }
    else{
      return ed;
    }
  }
  
}

#endif
