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
    int n = (size-1)/2;
    for (int i = -n; i <n ; i++){
      for(int j = -n; j < n; j++){
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
  
  float euclideanDistanceNL(vector<float>& a, vector<float>& b){
    float retour = 0;
    vector<float> calcul;
    for(int i = 0; i < a.size(); i++){
      calcul.push_back(b[i]-a[i]);
    }
    retour = means(calcul);
    retour = retour*retour;
    return retour;
  }

  
  float euclideanDistance(vector<float>& a, vector<float>& b){
    float ed = 0;
    if(a.size() == b.size()){
      vector<float> normeA = normalise(a);
      vector<float> normeB = normalise(b);
      for(int i = 0; i < a.size();i++){
	ed += pow((normeA[i]-normeB[i]),2);
      }
      return sqrt(ed);
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
