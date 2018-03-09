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

  float dot(const vector<float>& a, const vector<float>& b){
    int result = 0;
    int s = a.size();
    for(int i = 0; i < a.size(); i++){
      result += a[i]*b[i];
    }
    return result;
  }
  
  vector<float> getGrayScaleVector(CImg<unsigned char>& I,int x,int y,int size){
    vector<float> gsv;
    for (int i = -size; i <size ; i++){
      for(int j = -size; j < size; j++){
	gsv.push_back((float)I(x+i,y+j));
      }
    }
    return gsv;
  }

  vector<float> normalise(vector<float>& a){
    vector<float> retour(a);
    for (auto& n: retour){
      n = n/255;
    }
    return retour;
  }

  float norme(vector<float>& x){
    float n = 0;
    for (int i = 0; i < x.size(); i++){
      n += pow(x[i],2);
    }
    return sqrt(n);
  }

  const float means(vector<float>& a){
    float somme = 0;
    for(auto& n:a){
      somme += n;
    }
    return somme/a.size();
  }
  
  float euclideanDistance(vector<float>& a, vector<float>& b,float g){
    float ed = 0;
    if(a.size() == b.size()){
      vector<float> normeA = normalise(a);
      vector<float> normeB = normalise(b);
      for(int i = 0; i < a.size();i++){
	ed += pow((b[i]-a[i]),2);
      }
      return (sqrt(ed*g));
    }
    else{
      return ed;
    }
  }

  float standarDeviationGauss(int x, int y, float h){
    float content = -((pow(x,2)+pow(y,2))/(2*h*h));
    float div = 1/(2*h*h*M_PI);
    return div*exp(content);
  }
      
};


#endif
