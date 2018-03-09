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
  int search = 3*patchSize-1;
  CImg<unsigned char> F(n+search,m+search,1,I.spectrum(),0);
  //F.rand(0,255);
  F.draw_image(search/2,search/2,0,0,I);
  return F;
}

/*
 * distance euclidienne
 */
double weightTild(vector<float>& gsvI, vector<float>& gsvJ, float h,float a){
  double eucli = euclideanDistance(gsvI,gsvJ,a);
  return exp(-((eucli*eucli))/(h*h));
}

/*
 * Constente de normalisation (pas sur de la formule)
 */
double normaliseConstant(CImg<unsigned char>& I, vector<float>& gsvI,int patchSize,int h,int x, int y){
  vector<float> gsvJ;
  double constant = 0;
  int size = (patchSize-1)/2;
  float a = 0;
  for(int i = -patchSize; i < patchSize; i++){
    for(int j = -patchSize; j < patchSize; j++){
      gsvJ = getGrayScaleVector(I,x+i,y+j,size);
      a = standarDeviationGauss(i,j,h);
      constant += weightTild(gsvI,gsvJ,h,a);
      //cout << i << ", " << constant << endl;
    }
  }
  return constant;
}

/*
 * fonction calcul poids ( pour l'instant je ne suis pas sur de la fonc)
 */
double weight(vector<float>& gsvI, vector<float>& gsvJ, int h, double c,double a){
  double wt = weightTild(gsvI,gsvJ,h,a);
  return wt/c;
}

double filterPixel(CImg<unsigned char>& I, vector<float> gsvI, int patchSize, int h,int x, int y){
  int size = (patchSize-1)/2;
  int search = (patchSize*3)+1;
  double w = 0, somme = 0;
  double c = normaliseConstant(I,gsvI,patchSize,h,x,y);
  vector<float> gsvJ;
  double sommeW = 0;
  float a = 0;
  for(int i= -patchSize; i < patchSize; i++){
    for(int j = -patchSize; j < patchSize; j++){
      gsvJ = getGrayScaleVector(I,x+i,y+j,size);
      a = standarDeviationGauss(i,j,h);
      w = weight(gsvI,gsvJ,h,c,a);
      sommeW += w;
      somme += w*I(x+i,y+j);
    }
  }
  //cout << sommeW << " somme poids au pixel : " << x << ", " << y << endl;
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
  int search = patchSize*3+1;
  vector<float> gsvI;
  for(int x = 0; x < n; x++){
    for(int y = 0; y < m; y++){
      gsvI = getGrayScaleVector(I,x+search/2,y+search/2,size);
      F(x,y) = filterPixel(I,gsvI,patchSize,h,x+search/2,y+search/2);
    }
  }
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
      cout << "le bruit choisie est : " ;
      if( noi == 0){
	cout << "gaussien " << endl;
      }
      if( noi == 1){
	cout << "uniforme " << endl;
      }
      if( noi == 2){
	cout << "poivre et sel" << endl;
      }
      if( noi == 3){
	cout << "poisson " << endl;
      }
      if( noi == 4){
	cout << "Rician " << endl;
      }
      continue;
    }
    if (arg.compare("-h") ==0){
      i++;
      h = atoi(argv[i]);
      cout << "sigma bruit = " << h << endl;
      continue;
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
  CImg<unsigned char> filter = nlMeans(image,patchSize,h);
  CImgList<unsigned char> visu(original,image,filter);
  visu.display("test");
  return 0;
}
