#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "CImg.h"
#include "utils.h"

using namespace cimg_library;
using namespace std;
using namespace utils;

/**
 * Construit un element structurant.
 * Ici un carré de taille size*size
 **/
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

/**
 * Fonction de calcul de poids
 * W = alpha*euclideanDistanceGauss (from utils)
 * pour l'instant l'élément structurant n'est qu'un carré de taille patchSize.
 **/
double weight(vector<float> gsvI, vector<float> gsvJ, double alpha, float h,vector<float> kernel){
  double eucli = euclideanDistanceGauss(gsvI,gsvJ,h);
  return alpha*eucli;
}


/**
 * Operation de dilatation
 * 
 **/
CImg<unsigned char> ope_dilatation(CImg<unsigned char> &I, int size,double alpha, float h,double s){
  // padding de l'image a traité ou les bords sont a la valeur minimal d'un pixel : 0
  CImg<unsigned char> padding = dilaPadding(I, size, 3*size);

  // on crée l'image qui va stocker notre resultat après dilatation
  int n = I.width(), m = I.height();
  CImg<unsigned char> It(n,m,1,I.spectrum(),0);

  // on set up les variables qui vont être utile plus tard :
  // Pour l'instant pas d'élément structurant différents d'un carré de taille "size"
  //  patchSize      ,    search window  ,  padding
  int si = (size-1)/2, se = (size*3-1)/2, padd = si+se;
  vector<float> kernel = elementStruct(size);
  vector<float> gsvI, gsvJ;
  double maxW = -999999999999, minW = 99999999999; // variable pour le poids max et min de la dilatation.
  
  cout << "c'est l'heure de la dilatation " << endl;

  // on parcours l'image
  for (int i = 0; i < n; i++){
    for(int j = 0; j < m ; j++){
      gsvI = getGrayScaleVector(padding,i+padd,j+padd,si); // on stock le vecteur niveau de gris au pixel (i,j)
      double max = 0; // variable représentant le plus grand pixel 

      // on parcours la fenêtre de recherche
      for(int ki = -se; ki <= se; ki++){
	for(int kj = -se; kj <= se; kj++){
	  // on stock le vecteur niveau de gris (i+ki,j+kj) en prenant compte du padding
	  gsvJ = getGrayScaleVector(padding,i+padd+ki,j+padd+kj,si);
	  // calcul le poids
	  double w = -weight(gsvI,gsvJ,alpha,h,kernel);

	  // on stock le poids Max et Min
	  if(w > maxW){
	    maxW = w;
	  }
	  else if(w < minW){
	    minW = w;
	  }

	  // on test si le poids dépasse le seuil.
	  if( w > -s){
	    // si le pixel assossié au poids est plus grand que le pixel défini dans max
	    if(padding(i+ki+padd,j+kj+padd)+w > max){
	      max = padding(i+ki+padd,j+kj+padd)+w; // on le remplace
	    }
	  }
	}
      }
      // a la fin de la recherche on remplace la valeur (i,j) par la valeur max de son voisinage
      It(i,j) = max;
    }
  }
  cout << "max, min poids: "<< maxW << ", "<< minW << endl;
  return It;
}

CImg<unsigned char> ope_erosion(CImg<unsigned char> &I, int size,double alpha, float h,double s){
  // padding de l'image a traité ou les bords sont a la valeur maximal d'un pixel : 255
  CImg<unsigned char> zero = eroPadding(I, size, 3*size);

  // on crée l'image qui va stocker notre resultat après dilatation
  int n = I.width(), m = I.height();
  CImg<unsigned char> It(n,m,1,I.spectrum(),0);
  
  // on set up les variables qui vont être utile plus tard :
  // Pour l'instant pas d'élément structurant différents d'un carré de taille "size"
  //  patchSize      ,    search window  ,  padding
  int si = (size-1)/2, se = (size*3-1)/2, padd = si+se;
  vector<float> kernel = elementStruct(size);
  vector<float> gsvI, gsvJ;
 double maxW = -999999999999, minW = 99999999999;
     
  
  cout << "c'est l'heure de l'erosion " << endl;

  // on parcours l'image
  for (int i = 0; i < n; i++){
    for(int j = 0; j < m ; j++){
      gsvI = getGrayScaleVector(zero,i+padd,j+padd,si); // on stock le vecteur niveau de gris au pixel (i,j)
      double min = 255; // variable représentant le plus petit pixel du voisinage.
      
      for(int ki = -se; ki <= se; ki++){
	for(int kj = -se; kj <= se; kj++){
	  // on stock le vecteur niveau de gris (i+ki,j+kj) en prenant compte du padding (padd)
	  gsvJ = getGrayScaleVector(zero,i+padd+ki,j+padd+kj,si);
	  // on calcul le poids
	  double w = -weight(gsvI,gsvJ,alpha,h,kernel);

	  if(w > maxW){
	    maxW = w; 
	  }
	  else if(w < minW){
	    minW = w;
	  }
	  
	  // on test si le poids est au dessus du seuil.
	  if( w > -s){
	    // si le pixel assossié au poids est plus petit que le pixel défini dans min
	    if(zero(i+ki+padd,j+kj+padd)+w < min){
	      min = zero(i+ki+padd,j+kj+padd)+w; // on le remplace
	    }
	  }
	}
      }
      It(i,j) = min;
    }
  }
  cout << "max, min poids: "<< maxW << ", "<< minW << endl;
  
  return It;
}

CImg<unsigned char> gammaPhi(CImg<unsigned char> &I,int patchSize,double alpha, float h, double s){
  CImg<unsigned char> final = ope_erosion(I,patchSize,alpha,h,s);
  final = ope_dilatation(final,patchSize,alpha,h,s);
  final = ope_dilatation(final,patchSize,alpha,h,s);
  final = ope_erosion(final,patchSize,alpha,h,s);
  return final;
}

CImg<unsigned char> phiGamma(CImg<unsigned char> &I,int patchSize, double alpha, float h, double s){
  CImg<unsigned char> final = ope_dilatation(I,patchSize,alpha,h,s);
  final = ope_erosion(final,patchSize,alpha,h,s);
  final = ope_erosion(final,patchSize,alpha,h,s);
  final = ope_dilatation(final,patchSize,alpha,h,s);
  return final;
}


CImg<unsigned char> ouv_ferm(CImg<unsigned char> &I, CImg<unsigned char> &J){
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
  // option :
  const int patchSize = cimg_option("-s",3,"Patch Size");
  const char *im = cimg_option("-i","Picture/house2.jpg","Picture");
  const int noise = cimg_option("-n",0,"Noise type");
  const float h = cimg_option("-h",10,"Filtering parameter");
  const float sigma = cimg_option("-sig",10,"Sigma noise");
  const double alpha = cimg_option("-a",1.0,"Coef alpha");
  const double s = cimg_option("-seuil", 10.0,"Threshold");

  // calcul d'image
  CImg<unsigned char> original(im);
  CImg<unsigned char> noisi(original);
  original.channel(0);
  noisi.noise(sigma,noise);
  CImg<unsigned char> gamma = gammaPhi(noisi,patchSize,alpha,h,s);
  CImg<unsigned char> phi = phiGamma(noisi,patchSize,alpha,h,s);
  cout << "calcul final" << endl;
  CImg<unsigned char> fin = ouv_ferm(gamma,phi);
  CImgList<unsigned char> main_visu(original,noisi,gamma,phi,fin);

  // affiche les MSE et PSNR de chaque image par rapport a l'original.
  cout << endl;
  cout << "noisi : "<< noisi.MSE(original) << endl;
  cout << "Gamma Phi : " << gamma.MSE(original) << endl;
  cout << "Phi Gamma : " << phi.MSE(original) << endl;
  cout << "final : " << fin.MSE(original) << endl;
  cout << "DB noisi : " << noisi.PSNR(original,255) << endl; 
  cout << "DB Gamma Phi : " << gamma.PSNR(original,255) << endl;
  cout << "DB Phi Gamma : " << phi.PSNR(original,255) << endl;
  cout << "DB final : " << fin.PSNR(original,255) << endl;
  cout << endl;
 // affiche les images.
  main_visu.display();
  return 0;
}
