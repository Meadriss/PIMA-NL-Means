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

  /*
   * produit scalaire
   */
  float dot(const vector<float>& a, const vector<float>& b){
    int result = 0;
    int s = a.size();
    for(int i = 0; i < a.size(); i++){
      result += a[i]*b[i];
    }
    return result;
  }
  
  /*
   * retourne le vecteur des valeurs du patch centrer en x,y
   */
  vector<float> getGrayScaleVector(CImg<unsigned char>& I,int x,int y,int size){
    vector<float> gsv;
    for (int i = -size; i <size ; i++){
      for(int j = -size; j < size; j++){
	gsv.push_back((float)I(x+i,y+j));
      }
    }
    return gsv;
  }

  /*
   * normalise un vecteur
   */
  vector<float> normalize(vector<float>& a){
    vector<float> retour(a);
    for (auto& n: retour){
      n = n/255;
    }
    return retour;
  }

  /*
   * Norme vecteur
   */ 
  float norme(vector<float>& x){
    float n = 0;
    for (int i = 0; i < x.size(); i++){
      n += pow(x[i],2);
    }
    return sqrt(n);
  }

  /*
   * filtre moyenneur
   */
  const float means(vector<float>& a){
    float somme = 0;
    for(auto& n:a){
      somme += n;
    }
    return somme/a.size();
  }

  /*
   * filtre median
   */
  const float median(vector<float>& a){
    sort(a.begin(),a.end());
    return a[ceil(a.size()/2)];
  }

  /*
   * Deviation standard de Gauss 2D
   */
  float standarDeviationGauss(int x, int y, float h){
    float content = -((pow(x,2)+pow(y,2))/(2*h*h));
    float div = 1/(h*h*sqrt(2*M_PI));
    return div*exp(content);
  }
  
  float standarDeviationGauss1D(int x, float h){
    float content = -(pow(x,2)/(4*h*h));
    float div = 1/(h*h*4*M_PI);
    return div*exp(content);
  }
    
  /*
   * Distance euclidienne 2 par rapport a un noyau gaussian g
   */
  float euclideanDistance(vector<float>& a, vector<float>& b,float g){
    float ed = 0;
    if(a.size() == b.size()){
      vector<float> normeA = normalize(a);
      vector<float> normeB = normalize(b);
      for(int i = 0; i < a.size();i++){
	ed += pow(((b[i]-a[i])),2);
      }
      return sqrt(standarDeviationGauss1D(ed,g));
    }
    else{
      return ed;
    }
  }

  float euclideanDistanceGauss(vector<float>& a, vector<float>& b,float h){
    float ed = 0;
    if(a.size() == b.size()){
      int size = sqrt(a.size())/2;
      vector<float> g;
      for(int i = -size; i < size; i++){
	for(int j = -size; j < size; j++){
	  g.push_back(standarDeviationGauss(i,j,h));
	}
      }
      for(int k = 0; k < a.size();k++){
	float x = b[k]-a[k];
	//float g = standarDeviationGauss1D(i,h);
	ed += pow(sqrt(pow(x,2)),2)*g[k];
      }
      return ed;
    }
    else{
      return ed;
    }
  }


  /*
   * Just a Zero padding fonction for filter
   */
  CImg<unsigned char> zeroPadding(const CImg<unsigned char>& I,int patchSize){
  int n = I.width();
  int m = I.height();
  int search = 3*patchSize-1;
  CImg<unsigned char> F(n+search,m+search,1,I.spectrum(),0);
  //F.rand(0,255);
  F.draw_image(search/2,search/2,0,0,I);
  return F;
}
  
  CImg<unsigned char> zeroPadding(const CImg<unsigned char>& I, int patchSize, int search){
    int n = I.width();
    int m = I.height();
    int full = search;
    CImg<unsigned char> F(n+search-1,m+search-1,1,I.spectrum(),0);
    F.draw_image((search-1)/2,(search-1)/2,0,0,I);
    return F;
  }

  float meansSquareError(const CImg<unsigned char>& I, const CImg<unsigned char>& F){
    float sommeError = 0;
    for(int i = 0; i < I.width(); i++){
      for(int j = 0; j < I.height(); j++){
	sommeError += pow((F(i,j)-I(i,j)),2);
      }
    }
    return (sommeError)/(F.width()*F.height());
  }

};


namespace morphologie{
  using utils::standarDeviationGauss;
  
  float morphoWeight(const vector<float>& a,const vector<float>& b, float alpha, float h){
    float somme = 0;
    int size = sqrt(a.size())/2;
    vector<float> g;
    for(int i = -size; i < size; i++){
      for(int j = -size; j < size; j++){
	g.push_back(standarDeviationGauss(i,j,h));
      }
    }
    for(int k = 0; k < a.size();k++){
      float x = b[k]-a[k];
      somme += pow(sqrt(pow(x,2)),2)*g[k];
    }
    return -alpha*somme;
  }
}


#endif
