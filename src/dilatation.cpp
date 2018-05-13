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

CImg<unsigned char> dilatation(CImg<unsigned char> &I,vector<float> kernel,int size){
  CImg<unsigned char> It(I);
  int n = I.width(), m = I.height();
  
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      for(int ki = -size; ki < size; ki++){
	for(int kj = -size; kj < size; kj++){
	  if(i+ki < 0 || j+kj < 0){
	    continue;
	  }
	  if(i+ki > n || j+kj > m){
	    continue;
	  }
	  if(kernel[(ki+size)+(kj+size)*size]*I(i+ki,j+kj) == 255){
	      It(i,j) = 255;
	  }
	}
      }
    }
  }
  return It;
}

CImg<unsigned char> erosion(CImg<unsigned char> &I, vector<float> kernel,int size){
  CImg<unsigned char> It(I);
  int n = I.width(), m = I.height();
  
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      for(int ki = -size; ki < size; ki++){
	for(int kj = -size; kj < size; kj++){
	   if(i+ki < 0 || j+kj < 0){
	    continue;
	  }
	  if(i+ki > n || j+kj > m){
	    continue;
	  }
	  if(kernel[(ki+size)+(kj+size)*size]*I(i+ki,j+kj) == 0){
	      It(i,j) = 0;
	  }
	}
      }
    }
  }
  return It;
}

vector<float> elementStruct(int size){
  vector<float> element;
  for(int i = -size; i < size; i++){
    for(int j = -size; j < size; j++){
      element.push_back(1);
    }
  }
  return element;
}

int main(int argc, char** argv){
  const int valKernel = cimg_option("-k",2,"kenerl size");
  const char *im = cimg_option("-i","Binaire/index.jpeg","Binary picture");

  CImg<unsigned char> original(im);
  original.channel(0);
  vector<unsigned char> elem = elementStruct(valKernel);
  CImg<unsigned char> dila = dilatation(original,elem,valKernel);
  CImg<unsigned char> ero = erosion(original,elem,valKernel);
  CImg<unsigned char> ouver = dilatation(ero,elem,valKernel);
  CImg<unsigned char> ferme = erosion(dila,elem,valKernel);
  CImgList<unsigned char> visu(original,dila,ero,ouver,ferme);
  visu.display();
  return 0;
}
