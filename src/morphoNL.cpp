#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "CImg.h"
#include "utils.h"

using namespace cimg_library;
using namespace std;
using namespace utils;
using namespace morphologie;


float calculPixel(int x, int y, CImg<unsigned char>& I,vector<float> gsvI, int patchSize, int search, int h, int alpha){
  int size = (patchSize-1)/2, s = (search-1)/2;
  double w = 0, somme = 0, sommeW = 0;
  vector<float> gsvJ;
  float max = 0;
  for(int i = -patchSize; i <= patchSize; i++){
    for(int j = -patchSize; j <= patchSize; j++){
      if( i == 0 && j == 0){
	continue;
      }
      gsvJ = getGrayScaleVector(I,x+i,y+j,size);
      w = morphoWeight(gsvI,gsvJ,alpha,h);
      //cout << w << " okoko" << endl;
      if (I(x+i,y+j)+w > max){
	max = I(x+i,y+j)+w;
      }
      //somme += I(x+i,y+j)+(w/(h*h));
    }
  }
  return max;
}

float 


CImg<unsigned char> dilatation(CImg<unsigned char>& I, int patchSize, int search, int h, int alpha){
  CImg<unsigned char> zero = zeroPadding(I,patchSize,search);
  CImg<unsigned char> retour(I);
  int n = I.width(), m = I.height();
  int s = (search-1)/2;
  vector<float> gsvI;
  for(int i = 0; i < n; i++){
    for(int j =0; j < m; j++){
      gsvI = getGrayScaleVector(zero,i+s,j+s,(patchSize-1)/2);
      retour(i,j) = calculPixel(i+s,j+s,zero,gsvI,patchSize,search,h,alpha);
    }
  }
  return retour;
}


int main(int argc, char **argv){
  const int patchSize = cimg_option("-s",3,"Patch Size");
  const char *im = cimg_option("-i","Picture/house2.jpg","Picture");
  const int noise = cimg_option("-n",0,"Noise type");
  const float h = cimg_option("-h",1,"Filtering parameter");
  const float sigma = cimg_option("-sig",10,"Sigma noise");
  CImg<unsigned char> original(im);
  original.channel(0);
  original.noise(sigma,noise);
  CImg<unsigned char> filter = dilatation(original,patchSize,3*patchSize,h,2);
  CImgList<unsigned char> main_visu(filter);
  main_visu.display();
  return 0;
}
