#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "CImg.h"
#include "utils.h"
#include "operator.h"

using namespace cimg_library;
using namespace std;
using namespace utils;
using namespace operatorMorpho;

double weight(vector<float> gsvI, vector<float> gsvJ, double alpha, float h,vector<float> kernel){
  double eucli = euclideanDistanceGauss(gsvI,gsvJ,h);
  return alpha*eucli;
}

CImg<unsigned char> ope_dila(CImg<unsigned char> &I, int size,double alpha, float h,double s){
  CImg<unsigned char> zero = dilaPadding(I, size, 3*size);
  int n = I.width(), m = I.height();
  CImg<unsigned char> It(n,m,1,I.spectrum(),0);
  int si = (size-1)/2, se = (size*3-1)/2, padd = si+se;
  vector<float> kernel = elementStruct(size);
  vector<float> gsvI, gsvJ;
  double maxW = 0, minW = 0;
  cout << "c'est l'heure de la dilatation " << endl;
  for (int i = 0; i < n; i++){
    for(int j = 0; j < m ; j++){
      gsvI = getGrayScaleVector(zero,i+padd,j+padd,si);
      vector<float> calcul;
      double max = -999999999999;

      for(int ki = -se; ki <= se; ki++){
	for(int kj = -se; kj <= se; kj++){
	  /*if(ki == 0 && kj == 0){
	    continue;
	  }
	  else{*/
	    gsvJ = getGrayScaleVector(zero,i+padd+ki,j+padd+kj,si);
	    double w = -weight(gsvI,gsvJ,alpha,h,kernel);
	    //cout << w << endl;
	    if(ki == -se && kj == -se){
	      maxW = w;
	      minW = w;
	    }
	    if(ki == 0 && kj == 0){
	      continue;
	    }
	    if(w > maxW){
	      maxW = w;
	    }
	    else if(w < minW){
	      minW = w;
	    }
	    if( w > -s){
	      if(zero(i+ki+padd,j+kj+padd)+w > max){
		max = zero(i+ki+padd,j+kj+padd)+w;
	      }
	    }
	}
      }
      //cout << "max, min poids: "<< maxW << ", "<< minW << endl;
      It(i,j) = max;
    }
  }
  return It;
}

CImg<unsigned char> ope_ero(CImg<unsigned char> &I, int size,double alpha, float h,double s){
  CImg<unsigned char> zero = eroPadding(I, size, 3*size);
  int n = I.width(), m = I.height();
  CImg<unsigned char> It(n,m,1,I.spectrum(),0);
  int si = (size-1)/2, se = (size*3-1)/2, padd = si+se;
  vector<float> kernel = elementStruct(size);
  vector<float> gsvI, gsvJ;
  double maxW = 0, minW = 0;
  cout << "c'est l'heure de l'erosion " << endl;
  for (int i = 0; i < n; i++){
    for(int j = 0; j < m ; j++){
      gsvI = getGrayScaleVector(zero,i+padd,j+padd,si);
      vector<float> calcul;
      double min = 999999;

      for(int ki = -se; ki <= se; ki++){
	for(int kj = -se; kj <= se; kj++){
	  /*if(ki == 0 && kj == 0){
	    continue;
	  }
	  else{*/
	    gsvJ = getGrayScaleVector(zero,i+padd+ki,j+padd+kj,si);
	    double w = -weight(gsvI,gsvJ,alpha,h,kernel);
	    //cout << w << endl;
	    if(ki == -se && kj == -se){
	      maxW = w;
	      minW = w;
	    }
	    if(ki == 0 && kj == 0){
	      continue;
	    }
	    if(w > maxW){
	      maxW = w;
	    }
	    else if(w < minW){
	      minW = w;
	    }
	    if( w > -s){
	      if(zero(i+ki+padd,j+kj+padd)+w < min){
		min = zero(i+ki+padd,j+kj+padd)+w;
	      }
	    }
	}
      }
      //cout << "max, min poids: "<< maxW << ", "<< minW << endl;
      It(i,j) = min;
    }
  }
  return It;
}

CImg<unsigned char> ouverture(CImg<unsigned char> I,int patchSize, float h, double alpha, double s){
  CImg<unsigned char> final = ope_ero(I,patchSize,alpha,h,s);
  final = ope_dila(final,patchSize,alpha,h,s);
  final = ope_dila(final,patchSize,alpha,h,s);
  final = ope_ero(final,patchSize,alpha,h,s);
  return final;
}

CImg<unsigned char> fermeture(CImg<unsigned char> I,int patchSize, float h, double alpha, double s){
  CImg<unsigned char> final = ope_dila(I,patchSize,alpha,h,s);
  final = ope_ero(final,patchSize,alpha,h,s);
  final = ope_ero(final,patchSize,alpha,h,s);
  final = ope_dila(final,patchSize,alpha,h,s);
  return final;
}


CImg<unsigned char> ouv_ferm(CImg<unsigned char> I, CImg<unsigned char> J){
  int n = I.width(), m = I.height();
  CImg<unsigned char> final(n,m,1,I.spectrum(),0);
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      final(i,j) = (I(i,j)+J(i,j))/2;
    }
  }
  return final;
}

int main(int argc, char **argv){
  const int patchSize = cimg_option("-s",3,"Patch Size");
  const char *im = cimg_option("-i","Picture/house2.jpg","Picture");
  const int noise = cimg_option("-n",0,"Noise type");
  const float h = cimg_option("-h",10,"Filtering parameter");
  const float sigma = cimg_option("-sig",10,"Sigma noise");
  const double alpha = cimg_option("-a",1.0,"Coef alpha");
  const double s = cimg_option("-seuil", 10.0,"Threshold");
  CImg<unsigned char> original(im);
  CImg<unsigned char> noisi(original);
  original.channel(0);
  noisi.noise(sigma,noise);
  cout << "cherchons ou ca bug !" << endl;
  CImg<unsigned char> dila = ope_dila(noisi,patchSize,alpha,h,s);
  CImg<unsigned char> ero = ope_ero(noisi,patchSize,alpha,h,s);
  CImg<unsigned char> ouv = ouverture(original,patchSize,alpha,h,s);
  CImg<unsigned char> ferm = fermeture(original,patchSize,alpha,h,s);
  CImg<unsigned char> fin = ouv_ferm(ouv,ferm);
  CImgList<unsigned char> main_visu(original,noisi,dila,ero,ouv,ferm,fin);
  cout << endl;
  cout << "noisi : "<< noisi.MSE(original) << endl;
  cout << "dilatation : " << dila.MSE(original) << endl;
  cout << "erosion : " << ero.MSE(original) << endl;
  cout << "ouverture : " << ouv.MSE(original) << endl;
  cout << "fermeture : " << ferm.MSE(original) << endl;
  cout << "final : " << fin.MSE(original) << endl;
  cout << "DB noisi : " << noisi.PSNR(original,255) << endl; 
  cout << "DB dilatation : " << dila.PSNR(original,255) << endl;
  cout << "DB erosion : " << ero.PSNR(original,255) << endl;
  cout << "DB ouverture : " << ouv.PSNR(original,255) << endl;
  cout << "DB fermeture : " << ferm.PSNR(original,255) << endl;
  cout << "DB final : " << fin.PSNR(original,255) << endl;
  cout << endl;

  main_visu.display();
  return 0;
}
