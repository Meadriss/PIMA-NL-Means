#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

#include "CImg.h"
#include "utils.h"
using namespace cimg_library;
using namespace std;
using namespace utils;

/*
 * Just a Zero padding fonction for filter
 */
CImg<unsigned char> zeroPadding(const CImg<unsigned char>& I,int patchSize){
  int n = I.width();
  int m = I.height();
  CImg<unsigned char> F(n+patchSize-1,m+patchSize-1,1,I.spectrum(),0);
  //F.rand(0,255);
  F.draw_image(((patchSize-1)/2),((patchSize-1)/2),0,0,I);
  return F;
}

/*
 * distance euclidienne avec filtre moyenneur
 */
double weightTild(vector<float>& gsvI, vector<float>& gsvJ, float h){
  double eucli = euclideanDistanceNL(gsvI,gsvJ);
  //cout << "eucli = "<< eucli << endl;
  return exp(-(eucli*eucli)/(h*h));
}

/*
 * Constente de normalisation (pas sur de la formule)
 */
double normaliseConstant(CImg<unsigned char>& I, vector<float>& gsvI,int patchSize,int h){
  vector<float> gsvJ;
  double constant = 0;
  int n = I.width();
  int m = I.height();
  int size = (patchSize-1)/2;
  for(int i = size; i < n-size; i++){
    for(int j = size; j < m-size; j++){
      gsvJ = getGrayScaleVector(I,i,j,patchSize);
      constant += weightTild(gsvI,gsvJ,h);
    }
  }
  return constant;
}

/*
 * fonction calcul poids ( pour l'instant je ne suis pas sur de la fonc)
 */
double weight(vector<float>& gsvI, vector<float>& gsvJ, int h, double c){
  double wt = weightTild(gsvI,gsvJ,h);
  //cout << wt << " je sais pas " <<  endl;
  return wt/c;
}

double filterPixel(CImg<unsigned char>& I,CImg<unsigned char>& F, vector<float> gsvI, int patchSize, int h,int x, int y){
  int n = I.width();int m = I.height();
  int size = (patchSize-1)/2;
  double w = 0, somme = 0;
  double c = normaliseConstant(I,gsvI,patchSize,h);
  vector<float> gsvJ;
  
  for(int i= size; i < n+size; i++){
    for(int j = size; j < m+size; j++){
      gsvJ = getGrayScaleVector(I,i,j,patchSize);
      w = weight(gsvI,gsvJ,h,c);
      somme += w*F(i-size,j-size);
    }
  }
  return somme;
}

/*
 * NLMeans algo 
 * pour l'instant test le poids des pixels par rapport a un pixel
 */
CImg<unsigned char> nlMeans(CImg<unsigned char>& I, int patchSize,int h){
  CImg<unsigned char> F(I);
  I = zeroPadding(I,patchSize);
  int n = F.width();
  int m = F.height();
  int size = (patchSize-1)/2;
  vector<float> gsvI;
  vector<float> gsvJ;
  double sommeWeight = 0;
  double w = 0;
  double maxW = 0;
  double minW = 0;
  double c = 0;
  double somme =0;
  for(int x = 0; x < n; x++){
    for(int y = 0; y < m; y++){
      gsvI = getGrayScaleVector(I,x+size,y+size,patchSize);
      //c = normaliseConstant(I,gsvI,patchSize,h);
      /*
	for(int i = size; i < n+size; i++){
	for(int j = size; j < m+size; j++){
	gsvJ = getGrayScaleVector(I,i,j,patchSize);
	w = weight(gsvI,gsvJ,h,c);
	sommeWeight += w;
	somme += w*F(i-size,j-size);
	//F(i-size,j-size) = w*F(i-size,j-size)*c;
	maxW = max(w,maxW);
	minW = min(w,minW);
	}
	}
      */
      F(x,y) = filterPixel(I,F,gsvI,patchSize,h,x,y);
      somme = 0;
      w = 0;
      maxW = 0;
      minW = 0;
    }
  }
  //cout << "on test le poids : "<< sommeWeight << ", max | min w :" << maxW << "," << minW << endl;
  cout << " " << endl;
  cout << " test valeur pixel : " << somme << endl;
  cout << " " << endl;
  return F;
}


int main(int argc, char ** argv){
  int patchSize = 5;
  char *im;
  int noi = -1;
  int h = 10;
  for(int i = 1; i < argc; i++){
    string arg = argv[i];
    if (arg.compare("-i") == 0){
      i++;
      im = argv[i];
      cout << "l'image : "<< im << endl;
      continue;
    }
    if (arg.compare("-s") == 0){
      i++;
      patchSize = atoi(argv[i]);
      cout << "la taille du patch est : "<< patchSize << endl;
      continue;
    }
    if (arg.compare("-n") == 0){
      i++;
      noi = atoi(argv[i]);
      continue;
    }
    if (arg.compare("-h") ==0){
      i++;
      h = atoi(argv[i]);
    }
  }
  CImg<unsigned char> original(im);
  CImg<unsigned char> image(im);
  /* noise(sigma,noise_type) 
   *0 -> gaussian, 1 -> uniform, 2 -> Salt & Pepper,3 -> Poisson, 4 -> Rician
   */
  if( noi >= 0 && noi <= 4){
    image.noise(h,noi);
  }
  //means(image,patchSize);
  CImg<unsigned char> filter = nlMeans(image,patchSize,h);
  CImgList<unsigned char> visu(original,image,filter);
  visu.display("test");
  return 0;
}
